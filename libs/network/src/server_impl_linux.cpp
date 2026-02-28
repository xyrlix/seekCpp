/**
 * @file    : server_impl_linux.cpp
 * @brief   : Linux平台的服务器实现
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 2025-10-05 04:30:32
 * @copyright: Copyright (c) 2025 Seek Dao
 */

#include "../include/server_impl_linux.h"
#include "../include/socket.h"
#include "../include/socket_factory.h"
#include <string>
#include <cstring>
#include <cstdint>
#include <cstddef>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <memory>
#include <mutex>
#include <thread>
#include <condition_variable>

ServerImplLinux::ServerImplLinux(Server* parent) 
    : ServerImpl(parent), server_socket_fd_(-1), max_connections_(0) {
    std::memset(&server_addr_, 0, sizeof(server_addr_));
}

ServerImplLinux::~ServerImplLinux() {
    stop();
}

ServerImplLinux::ServerImplLinux(ServerImplLinux&& other) noexcept 
    : ServerImpl(std::move(static_cast<ServerImpl&>(other))), server_socket_fd_(other.server_socket_fd_), 
      max_connections_(other.max_connections_) {
    // 移动地址信息
    server_addr_ = other.server_addr_;
    
    // 重置源对象的套接字描述符
    other.server_socket_fd_ = -1;
    other.max_connections_ = 0;
    std::memset(&other.server_addr_, 0, sizeof(other.server_addr_));
}

ServerImplLinux& ServerImplLinux::operator=(ServerImplLinux&& other) noexcept {
    if (this != &other) {
        stop();
        
        // 移动基类成员
        ServerImpl::operator=(std::move(static_cast<ServerImpl&>(other)));
        
        // 移动套接字描述符和配置
        server_socket_fd_ = other.server_socket_fd_;
        server_addr_ = other.server_addr_;
        max_connections_ = other.max_connections_;
        
        // 重置源对象的套接字描述符
        other.server_socket_fd_ = -1;
        other.max_connections_ = 0;
        std::memset(&other.server_addr_, 0, sizeof(other.server_addr_));
    }
    return *this;
}

bool ServerImplLinux::start(std::uint16_t port, std::size_t max_connections) {
    if (is_running()) {
        set_last_error("Server is already running");
        return false;
    }
    
    // 创建服务器套接字
    server_socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_fd_ < 0) {
        set_last_error("Failed to create server socket: " + std::string(std::strerror(errno)));
        return false;
    }
    
    // 设置地址重用
    int opt = 1;
    if (setsockopt(server_socket_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        set_last_error("Failed to set socket options: " + std::string(std::strerror(errno)));
        ::close(server_socket_fd_);
        server_socket_fd_ = -1;
        return false;
    }
    
    // 绑定地址和端口
    server_addr_.sin_family = AF_INET;
    server_addr_.sin_addr.s_addr = INADDR_ANY;
    server_addr_.sin_port = htons(port);
    
    if (bind(server_socket_fd_, reinterpret_cast<struct sockaddr*>(&server_addr_), sizeof(server_addr_)) < 0) {
        set_last_error("Failed to bind socket to port " + std::to_string(port) + ": " + std::string(std::strerror(errno)));
        ::close(server_socket_fd_);
        server_socket_fd_ = -1;
        return false;
    }
    
    // 开始监听
    if (listen(server_socket_fd_, SOMAXCONN) < 0) {
        set_last_error("Failed to start listening: " + std::string(std::strerror(errno)));
        ::close(server_socket_fd_);
        server_socket_fd_ = -1;
        return false;
    }
    
    // 设置为非阻塞模式
    if (!set_non_blocking(server_socket_fd_, true)) {
        set_last_error("Failed to set non-blocking mode: " + std::string(std::strerror(errno)));
        ::close(server_socket_fd_);
        server_socket_fd_ = -1;
        return false;
    }
    
    // 保存配置
    max_connections_ = max_connections;
    
    set_state(Server::State::Starting);
    
    // 启动服务器主线程
    running_ = true;
    main_thread_ = std::thread(&ServerImplLinux::server_loop, this);
    
    set_state(Server::State::Running);
    
    return true;
}

void ServerImplLinux::stop() {
    {   
        std::lock_guard<std::mutex> lock(mutex_);
        if (!running_) {
            return;
        }
        
        running_ = false;
        
        // 关闭服务器套接字
        if (server_socket_fd_ >= 0) {
            ::close(server_socket_fd_);
            server_socket_fd_ = -1;
        }
        
        // 断开所有客户端连接
        for (auto& pair : clients_) {
            pair.second->disconnect();
        }
        clients_.clear();
    }
    
    // 通知等待中的线程
    cv_.notify_one();
    
    // 等待主线程结束
    if (main_thread_.joinable()) {
        main_thread_.join();
    }
    
    set_state(Server::State::Stopped);
}

bool ServerImplLinux::is_running() const {
    return running_ && get_state() == Server::State::Running;
}

int ServerImplLinux::send_to_client(ClientConnection::Id client_id, const void* data, size_t size) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = clients_.find(client_id);
    if (it == clients_.end() || !it->second->is_active()) {
        return -1;
    }
    
    return it->second->send(data, size);
}

size_t ServerImplLinux::broadcast(const void* data, size_t size) {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t sent_count = 0;
    
    // 直接遍历，使用const引用避免拷贝
    for (const auto& pair : clients_) {
        if (pair.second->is_active() && pair.second->send(data, size) > 0) {
            sent_count++;
        }
    }
    
    return sent_count;
}

void ServerImplLinux::disconnect_client(ClientConnection::Id client_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = clients_.find(client_id);
    if (it != clients_.end()) {
        it->second->disconnect();
        clients_.erase(it);
    }
}

size_t ServerImplLinux::get_connected_clients_count() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return clients_.size();
}

bool ServerImplLinux::set_non_blocking(int socket_fd, bool non_blocking) {
    if (socket_fd < 0) {
        return false;
    }
    
    int flags = fcntl(socket_fd, F_GETFL, 0);
    if (flags < 0) {
        return false;
    }
    
    if (non_blocking) {
        flags |= O_NONBLOCK;
    } else {
        flags &= ~O_NONBLOCK;
    }
    
    return fcntl(socket_fd, F_SETFL, flags) >= 0;
}

std::unique_ptr<Socket> ServerImplLinux::accept_connection() {
    if (server_socket_fd_ < 0) {
        return nullptr;
    }
    
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    
    int client_socket_fd = accept(server_socket_fd_, reinterpret_cast<struct sockaddr*>(&client_addr), &client_addr_len);
    if (client_socket_fd < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            set_last_error("Failed to accept connection: " + std::string(strerror(errno)));
        }
        return nullptr;
    }
    
    // 设置客户端套接字为非阻塞模式
    if (!set_non_blocking(client_socket_fd, true)) {
        set_last_error("Failed to set client socket to non-blocking mode: " + std::string(strerror(errno)));
        ::close(client_socket_fd);
        return nullptr;
    }
    
    // 创建 Socket 对象
    auto socket = create_socket();
    
    // 获取客户端地址和端口
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    uint16_t client_port = ntohs(client_addr.sin_port);
    
    // 设置底层套接字描述符
    if (!socket->set_socket_descriptor(client_socket_fd, client_ip, client_port)) {
        set_last_error("Failed to set socket descriptor for client connection");
        ::close(client_socket_fd);
        return nullptr;
    }
    
    return socket;
}

void ServerImplLinux::server_loop() {
    struct pollfd poll_fd;
    poll_fd.fd = server_socket_fd_;
    poll_fd.events = POLLIN;
    
    while (running_) {
        int ret = poll(&poll_fd, 1, 100); // 100ms timeout
        
        if (ret < 0) {
            // 错误发生
            if (errno != EINTR) {
                invoke_error_handler("Poll error: " + std::string(strerror(errno)));
                break;
            }
            continue;
        }
        
        if (ret == 0) {
            // 超时，继续循环
            continue;
        }
        
        if (poll_fd.revents & POLLIN) {
            // 有连接请求
            auto socket = accept_connection();
            if (socket) {
                // 创建客户端连接对象
                auto client_id = next_client_id_++;
                auto client = std::make_unique<ClientConnection>(client_id, std::move(socket));
                
                // 添加到客户端列表
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    clients_[client_id] = std::move(client);
                }
                
                // 触发连接回调
                invoke_connection_handler(client_id);
                
                // 检查是否达到最大连接数
                if (max_connections_ > 0 && get_connected_clients_count() >= max_connections_) {
                    invoke_error_handler("Maximum number of connections reached");
                    // 注意：这里可以选择不再接受新连接，但我们继续接受，让底层处理
                }
            }
        }
        
        if (poll_fd.revents & POLLERR || poll_fd.revents & POLLHUP || poll_fd.revents & POLLNVAL) {
            // 发生错误
            invoke_error_handler("Server socket error or hangup");
            break;
        }
    }
    
    // 清理资源
    if (server_socket_fd_ >= 0) {
        ::close(server_socket_fd_);
        server_socket_fd_ = -1;
    }
}