/**
 * @file    : socket_impl.cpp
 * @brief   : SocketImpl类的实现和工厂函数
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 2025-10-05 04:30:32
 * @copyright: Copyright (c) 2025 Seek Dao
 */

#include "../include/socket_impl.h"
#include <string>
#include <memory>

// 根据平台选择合适的套接字实现
#ifdef _WIN32
#error Windows platform is not supported yet
#else
#include "../include/socket_impl_linux.h"
#endif

SocketImpl::SocketImpl(Socket* parent) 
    : parent_(parent), running_(false), 
      state_(Socket::State::Disconnected), 
      last_error_(Socket::Error::None) {
}

SocketImpl::~SocketImpl() {
    stop_event_loop();
}

SocketImpl::SocketImpl(SocketImpl&& other) noexcept 
    : parent_(other.parent_), running_(other.running_.load()), 
      state_(other.state_), last_error_(other.last_error_), 
      last_error_msg_(std::move(other.last_error_msg_)) {
    // 移动线程和同步原语
    if (event_thread_.joinable()) {
        // 不能直接移动线程，需要先停止
        stop_event_loop();
    }
}

SocketImpl& SocketImpl::operator=(SocketImpl&& other) noexcept {
    if (this != &other) {
        stop_event_loop();
        
        parent_ = other.parent_;
        running_ = other.running_.load();
        state_ = other.state_;
        last_error_ = other.last_error_;
        last_error_msg_ = std::move(other.last_error_msg_);
        
        // 不能直接移动线程，需要先停止
        if (event_thread_.joinable()) {
            stop_event_loop();
        }
    }
    return *this;
}

void SocketImpl::set_state(Socket::State state) {
    std::lock_guard<std::mutex> lock(mutex_);
    state_ = state;
}

Socket::State SocketImpl::get_state() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return state_;
}

void SocketImpl::set_last_error(Socket::Error error, const std::string& error_msg) {
    std::lock_guard<std::mutex> lock(mutex_);
    last_error_ = error;
    last_error_msg_ = error_msg;
}

std::pair<Socket::Error, std::string> SocketImpl::get_last_error() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return {last_error_, last_error_msg_};
}

void SocketImpl::start_event_loop() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!running_) {
        running_ = true;
        event_thread_ = std::thread(&SocketImpl::run_event_loop, this);
    }
}

void SocketImpl::stop_event_loop() {
    {   
        std::lock_guard<std::mutex> lock(mutex_);
        if (!running_) {
            return;
        }
        running_ = false;
    }
    
    // 通知等待中的线程
    cv_.notify_one();
    
    // 等待线程结束
    if (event_thread_.joinable()) {
        event_thread_.join();
    }
}

/**
 * @brief 创建适合当前平台的套接字实现
 */
std::unique_ptr<SocketImpl> create_socket_impl(Socket* parent) {
#ifdef _WIN32
#error Windows platform is not supported yet
#else
    return std::make_unique<SocketImplLinux>(parent);
#endif
}