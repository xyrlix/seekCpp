/**
 * @file    : server_impl.cpp
 * @brief   : ServerImpl类的实现和工厂函数
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 2025-10-05 04:30:32
 * @copyright: Copyright (c) 2025 Seek Dao
 */

#include "../include/server_impl.h"

// 根据平台选择合适的服务器实现
#ifdef _WIN32
#error Windows platform is not supported yet
#else
#include "../include/server_impl_linux.h"
#include "../include/socket.h"
#endif

ServerImpl::ServerImpl(Server* parent) 
    : parent_(parent), running_(false), 
      state_(Server::State::Stopped), 
      next_client_id_(1), max_connections_(0) {
}

ServerImpl::~ServerImpl() {
    // 不要在基类析构函数中调用纯虚函数
    // stop();
}

ServerImpl::ServerImpl(ServerImpl&& other) noexcept 
    : parent_(other.parent_), running_(other.running_.load()), 
      state_(other.state_), last_error_(std::move(other.last_error_)), 
      clients_(std::move(other.clients_)), 
      next_client_id_(other.next_client_id_.load()),
      max_connections_(other.max_connections_) {
    // 移动线程和同步原语
    if (main_thread_.joinable()) {
        // 不能直接移动线程，也不能在构造函数中调用纯虚函数
        // stop();
    }
}

ServerImpl& ServerImpl::operator=(ServerImpl&& other) noexcept {
    if (this != &other) {
        stop();
        
        parent_ = other.parent_;
        running_ = other.running_.load();
        state_ = other.state_;
        last_error_ = std::move(other.last_error_);
        clients_ = std::move(other.clients_);
        next_client_id_ = other.next_client_id_.load();
        
        // 不能直接移动线程，需要先停止
        if (main_thread_.joinable()) {
            stop();
        }
    }
    return *this;
}

void ServerImpl::set_state(Server::State state) {
    std::lock_guard<std::mutex> lock(mutex_);
    state_ = state;
}

Server::State ServerImpl::get_state() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return state_;
}

void ServerImpl::set_last_error(const std::string& error_msg) {
    std::lock_guard<std::mutex> lock(mutex_);
    last_error_ = error_msg;
}

std::string ServerImpl::get_last_error() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return last_error_;
}

void ServerImpl::add_client(std::unique_ptr<ClientConnection> client) {
    if (!client) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // 检查连接数是否超过限制
    if (clients_.size() >= max_connections_) {
        invoke_error_handler("Max connections reached");
        return;
    }
    
    // 获取客户端ID
    ClientConnection::Id client_id = client->get_id();
    
    // 存储客户端连接
    clients_[client_id] = std::move(client);
    
    // 调用连接回调
    invoke_connection_handler(client_id);
}

/**
 * @brief 创建适合当前平台的服务器实现
 */
std::unique_ptr<ServerImpl> create_server_impl(Server* parent) {
#ifdef _WIN32
#error Windows platform is not supported yet
#else
    return std::make_unique<ServerImplLinux>(parent);
#endif
}