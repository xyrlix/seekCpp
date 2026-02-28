#include "socket.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

Socket::Socket() : socket_(-1), state_(SocketState::Disconnected), receive_thread_running_(false) {
    // 初始化套接字
}

Socket::~Socket() {
    disconnect();
}

bool Socket::connect(const std::string& host, uint16_t port, int timeout_ms) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // 如果已经连接，先断开
    if (is_connected()) {
        disconnect();
    }

    // 创建套接字
    socket_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socket_ < 0) {
        if (error_callback_) {
            error_callback_(SocketError::NetworkError);
        }
        return false;
    }

    // 设置服务器地址
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // 解析主机名
    struct hostent* server = gethostbyname(host.c_str());
    if (server == nullptr) {
        ::close(socket_);
        socket_ = -1;
        if (error_callback_) {
            error_callback_(SocketError::HostNotFound);
        }
        return false;
    }
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    // 设置非阻塞
    if (!set_non_blocking()) {
        ::close(socket_);
        socket_ = -1;
        if (error_callback_) {
            error_callback_(SocketError::NetworkError);
        }
        return false;
    }

    // 尝试连接
    state_ = SocketState::Connecting;
    int result = ::connect(socket_, (struct sockaddr*)&server_addr, sizeof(server_addr));
    
    if (result < 0) {
        if (errno == EINPROGRESS) {
            // 处理连接超时
            if (!handle_connection_timeout(timeout_ms)) {
                return false;
            }
        } else {
            // 其他错误
            ::close(socket_);
            socket_ = -1;
            state_ = SocketState::Disconnected;
            if (error_callback_) {
                error_callback_(SocketError::ConnectionRefused);
            }
            return false;
        }
    }

    // 恢复为阻塞模式
    int flags = fcntl(socket_, F_GETFL, 0);
    if (flags >= 0) {
        fcntl(socket_, F_SETFL, flags & ~O_NONBLOCK);
    }

    // 连接成功
    state_ = SocketState::Connected;
    
    // 启动接收线程
    receive_thread_running_ = true;
    receive_thread_ = std::thread(&Socket::receive_loop, this);

    // 调用连接回调
    if (connect_callback_) {
        connect_callback_(true, SocketError::None);
    }

    return true;
}

void Socket::disconnect() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (state_ != SocketState::Disconnected) {
        state_ = SocketState::Disconnecting;
        
        // 停止接收线程
        receive_thread_running_ = false;
        if (receive_thread_.joinable()) {
            receive_thread_.join();
        }
        
        // 关闭套接字
        if (socket_ >= 0) {
            ::close(socket_);
            socket_ = -1;
        }
        
        state_ = SocketState::Disconnected;
        
        // 调用断开连接回调
        if (disconnect_callback_) {
            disconnect_callback_();
        }
    }
}

bool Socket::send(const std::string& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!is_connected()) {
        if (error_callback_) {
            error_callback_(SocketError::NetworkError);
        }
        return false;
    }

    const char* buffer = data.c_str();
    size_t total_sent = 0;
    size_t remaining = data.size();
    
    while (total_sent < data.size()) {
        ssize_t sent = ::send(socket_, buffer + total_sent, remaining, 0);
        if (sent < 0) {
            // 发送失败
            handle_receive_error(errno);
            return false;
        } else if (sent == 0) {
            // 连接关闭
            disconnect();
            return false;
        }
        
        total_sent += sent;
        remaining -= sent;
    }
    
    return true;
}

bool Socket::is_connected() const {
    return state_ == SocketState::Connected;
}

SocketState Socket::get_state() const {
    return state_;
}

void Socket::set_connection_handler(const ConnectCallback& callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    connect_callback_ = callback;
}

void Socket::set_disconnection_handler(const DisconnectCallback& callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    disconnect_callback_ = callback;
}

void Socket::set_receive_handler(const ReceiveCallback& callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    receive_callback_ = callback;
}

void Socket::set_error_handler(const ErrorCallback& callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    error_callback_ = callback;
}

void Socket::receive_loop() {
    char buffer[4096];
    
    while (receive_thread_running_ && is_connected()) {
        ssize_t bytes_read = ::recv(socket_, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_read > 0) {
            // 接收到数据
            buffer[bytes_read] = '\0';
            std::string data(buffer, bytes_read);
            
            // 调用接收回调
            if (receive_callback_) {
                receive_callback_(data);
            }
        } else if (bytes_read == 0) {
            // 连接关闭
            disconnect();
            break;
        } else {
            // 发生错误
            handle_receive_error(errno);
            break;
        }
    }
}

bool Socket::set_non_blocking() {
    int flags = fcntl(socket_, F_GETFL, 0);
    if (flags < 0) {
        return false;
    }
    if (fcntl(socket_, F_SETFL, flags | O_NONBLOCK) < 0) {
        return false;
    }
    return true;
}

bool Socket::handle_connection_timeout(int timeout_ms) {
    fd_set fdset;
    struct timeval tv;
    FD_ZERO(&fdset);
    FD_SET(socket_, &fdset);
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    int result = select(socket_ + 1, nullptr, &fdset, nullptr, &tv);
    if (result == 0) {
        // 超时
        ::close(socket_);
        socket_ = -1;
        state_ = SocketState::Disconnected;
        if (error_callback_) {
            error_callback_(SocketError::Timeout);
        }
        return false;
    } else if (result < 0) {
        // 发生错误
        ::close(socket_);
        socket_ = -1;
        state_ = SocketState::Disconnected;
        if (error_callback_) {
            error_callback_(SocketError::NetworkError);
        }
        return false;
    }

    // 检查连接是否成功
    int so_error;
    socklen_t len = sizeof(so_error);
    getsockopt(socket_, SOL_SOCKET, SO_ERROR, &so_error, &len);
    if (so_error != 0) {
        ::close(socket_);
        socket_ = -1;
        state_ = SocketState::Disconnected;
        if (error_callback_) {
            error_callback_(SocketError::ConnectionRefused);
        }
        return false;
    }

    return true;
}

void Socket::handle_receive_error(int error) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // 如果不是非阻塞模式下的暂时错误
    if (error != EAGAIN && error != EWOULDBLOCK) {
        disconnect();
        if (error_callback_) {
            error_callback_(SocketError::NetworkError);
        }
    }
}