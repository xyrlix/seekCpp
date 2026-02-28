/**
 * @file    : socket_impl_linux.cpp
 * @brief   : Linux平台的套接字实现
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 2025-10-05 04:30:32
 * @copyright: Copyright (c) 2025 Seek Dao
 */

#include "../include/socket_impl_linux.h"
#include <string>
#include <cstring>
#include <cstdint>
#include <cstddef>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <mutex>
#include <thread>
#include <condition_variable>

SocketImplLinux::SocketImplLinux(Socket* parent) 
    : SocketImpl(parent), socket_fd_(-1) {
    std::memset(&remote_addr_, 0, sizeof(remote_addr_));
    std::memset(&local_addr_, 0, sizeof(local_addr_));
    
    // 创建套接字
    socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd_ < 0) {
        set_last_error(Socket::Error::SystemError, "Failed to create socket: " + std::string(std::strerror(errno)));
    }
}

SocketImplLinux::~SocketImplLinux() {
    disconnect();
}

SocketImplLinux::SocketImplLinux(SocketImplLinux&& other) noexcept 
    : SocketImpl(std::move(static_cast<SocketImpl&>(other))), socket_fd_(other.socket_fd_) {
    // 移动地址信息
    remote_addr_ = other.remote_addr_;
    local_addr_ = other.local_addr_;
    
    // 重置源对象的套接字描述符
    other.socket_fd_ = -1;
    std::memset(&other.remote_addr_, 0, sizeof(other.remote_addr_));
    std::memset(&other.local_addr_, 0, sizeof(other.local_addr_));
}

SocketImplLinux& SocketImplLinux::operator=(SocketImplLinux&& other) noexcept {
    if (this != &other) {
        disconnect();
        
        // 移动基类成员
        SocketImpl::operator=(std::move(static_cast<SocketImpl&>(other)));
        
        // 移动套接字描述符
        socket_fd_ = other.socket_fd_;
        remote_addr_ = other.remote_addr_;
        local_addr_ = other.local_addr_;
        
        // 重置源对象的套接字描述符
        other.socket_fd_ = -1;
        std::memset(&other.remote_addr_, 0, sizeof(other.remote_addr_));
        std::memset(&other.local_addr_, 0, sizeof(other.local_addr_));
    }
    return *this;
}

bool SocketImplLinux::connect(const std::string& host, std::uint16_t port) {
    if (socket_fd_ < 0) {
        set_last_error(Socket::Error::SystemError, "Invalid socket file descriptor");
        return false;
    }
    
    // 设置远程地址
    std::memset(&remote_addr_, 0, sizeof(remote_addr_));
    remote_addr_.sin_family = AF_INET;
    remote_addr_.sin_port = htons(port);
    
    // 解析主机名
    if (inet_pton(AF_INET, host.c_str(), &remote_addr_.sin_addr) <= 0) {
        set_last_error(Socket::Error::InvalidAddress, "Invalid address: " + host);
        return false;
    }
    
    set_state(Socket::State::Connecting);
    
    // 连接服务器
    if (::connect(socket_fd_, reinterpret_cast<struct sockaddr*>(&remote_addr_), sizeof(remote_addr_)) < 0) {
        set_last_error(Socket::Error::ConnectionRefused, "Failed to connect to " + host + ":" + std::to_string(port) + ": " + std::string(std::strerror(errno)));
        set_state(Socket::State::Disconnected);
        return false;
    }
    
    // 获取本地地址
    socklen_t addr_len = sizeof(local_addr_);
    if (getsockname(socket_fd_, reinterpret_cast<struct sockaddr*>(&local_addr_), &addr_len) < 0) {
        set_last_error(Socket::Error::SystemError, "Failed to get local address: " + std::string(std::strerror(errno)));
        disconnect();
        return false;
    }
    
    set_state(Socket::State::Connected);
    
    // 启动事件循环
    start_event_loop();
    
    return true;
}

int SocketImplLinux::send(const void* data, std::size_t size) {
    if (socket_fd_ < 0 || !is_connected()) {
        set_last_error(Socket::Error::NotConnected, "Socket is not connected");
        return -1;
    }
    
    int bytes_sent = ::send(socket_fd_, data, size, 0);
    if (bytes_sent < 0) {
        set_last_error(Socket::Error::SystemError, "Failed to send data: " + std::string(std::strerror(errno)));
        return -1;
    }
    
    return bytes_sent;
}

int SocketImplLinux::receive(void* buffer, std::size_t size) {
    if (socket_fd_ < 0 || !is_connected()) {
        set_last_error(Socket::Error::NotConnected, "Socket is not connected");
        return -1;
    }
    
    int bytes_received = ::recv(socket_fd_, buffer, size, 0);
    if (bytes_received < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            set_last_error(Socket::Error::SystemError, "Failed to receive data: " + std::string(std::strerror(errno)));
        }
        return -1;
    } else if (bytes_received == 0) {
        // 连接已关闭
        disconnect();
        return 0;
    }
    
    return bytes_received;
}

void SocketImplLinux::disconnect() {
    {   
        std::lock_guard<std::mutex> lock(mutex_);
        if (socket_fd_ >= 0) {
            ::close(socket_fd_);
            socket_fd_ = -1;
        }
    }
    
    stop_event_loop();
    set_state(Socket::State::Disconnected);
    invoke_disconnection_handler();
}

bool SocketImplLinux::is_connected() const {
    return socket_fd_ >= 0 && get_state() == Socket::State::Connected;
}

std::string SocketImplLinux::get_remote_address() const {
    char addr_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(remote_addr_.sin_addr), addr_str, INET_ADDRSTRLEN);
    return std::string(addr_str);
}

uint16_t SocketImplLinux::get_remote_port() const {
    return ntohs(remote_addr_.sin_port);
}

std::string SocketImplLinux::get_local_address() const {
    char addr_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(local_addr_.sin_addr), addr_str, INET_ADDRSTRLEN);
    return std::string(addr_str);
}

uint16_t SocketImplLinux::get_local_port() const {
    return ntohs(local_addr_.sin_port);
}

bool SocketImplLinux::set_socket_descriptor(int socket_fd, const std::string& remote_addr, uint16_t remote_port) {
    if (socket_fd < 0) {
        set_last_error(Socket::Error::InvalidArgument, "Invalid socket file descriptor");
        return false;
    }
    
    // 关闭现有的套接字（如果有）
    if (socket_fd_ >= 0) {
        ::close(socket_fd_);
    }
    
    // 设置新的套接字描述符
    socket_fd_ = socket_fd;
    
    // 设置远程地址信息
    memset(&remote_addr_, 0, sizeof(remote_addr_));
    remote_addr_.sin_family = AF_INET;
    remote_addr_.sin_port = htons(remote_port);
    
    // 解析远程地址
    if (inet_pton(AF_INET, remote_addr.c_str(), &remote_addr_.sin_addr) <= 0) {
        set_last_error(Socket::Error::InvalidAddress, "Invalid remote address: " + remote_addr);
        ::close(socket_fd_);
        socket_fd_ = -1;
        return false;
    }
    
    // 获取本地地址信息
    socklen_t addr_len = sizeof(local_addr_);
    if (getsockname(socket_fd_, reinterpret_cast<struct sockaddr*>(&local_addr_), &addr_len) < 0) {
        set_last_error(Socket::Error::SystemError, "Failed to get local address: " + std::string(strerror(errno)));
        ::close(socket_fd_);
        socket_fd_ = -1;
        return false;
    }
    
    // 设置为非阻塞模式
    if (!set_non_blocking(true)) {
        set_last_error(Socket::Error::SystemError, "Failed to set non-blocking mode: " + std::string(strerror(errno)));
        ::close(socket_fd_);
        socket_fd_ = -1;
        return false;
    }
    
    // 设置状态为已连接
    set_state(Socket::State::Connected);
    
    // 启动事件循环
    start_event_loop();
    
    return true;
}

bool SocketImplLinux::set_non_blocking(bool non_blocking) {
    if (socket_fd_ < 0) {
        return false;
    }
    
    int flags = fcntl(socket_fd_, F_GETFL, 0);
    if (flags < 0) {
        return false;
    }
    
    if (non_blocking) {
        flags |= O_NONBLOCK;
    } else {
        flags &= ~O_NONBLOCK;
    }
    
    return fcntl(socket_fd_, F_SETFL, flags) >= 0;
}

void SocketImplLinux::run_event_loop() {
    set_non_blocking(true);
    
    struct pollfd poll_fd;
    poll_fd.fd = socket_fd_;
    poll_fd.events = POLLIN;
    
    constexpr size_t BUFFER_SIZE = 4096;
    std::vector<uint8_t> buffer(BUFFER_SIZE);
    
    while (running_) {
        int ret = poll(&poll_fd, 1, 100); // 100ms timeout
        
        if (ret < 0) {
            // 错误发生
            if (errno != EINTR) {
                invoke_error_handler(Socket::Error::SystemError, "Poll error: " + std::string(strerror(errno)));
                break;
            }
            continue;
        }
        
        if (ret == 0) {
            // 超时，继续循环
            continue;
        }
        
        if (poll_fd.revents & POLLIN) {
            // 有数据可读
            int bytes_received = ::recv(socket_fd_, buffer.data(), buffer.size(), 0);
            if (bytes_received > 0) {
                // 调用数据处理回调
                std::vector<uint8_t> data(buffer.begin(), buffer.begin() + bytes_received);
                invoke_data_handler(data);
            } else if (bytes_received == 0) {
                // 连接已关闭
                invoke_error_handler(Socket::Error::ConnectionClosed, "Connection closed by peer");
                disconnect();
                break;
            } else {
                // 发生错误
                if (errno != EAGAIN && errno != EWOULDBLOCK) {
                    invoke_error_handler(Socket::Error::SystemError, "Receive error: " + std::string(strerror(errno)));
                    disconnect();
                    break;
                }
            }
        }
        
        if (poll_fd.revents & POLLERR || poll_fd.revents & POLLHUP || poll_fd.revents & POLLNVAL) {
            // 发生错误或连接关闭
            invoke_error_handler(Socket::Error::SystemError, "Socket error or hangup");
            disconnect();
            break;
        }
    }
}