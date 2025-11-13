/**
 * @file    : socket_impl.h
 * @brief   : 套接字实现的内部接口
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 2025-10-05 04:30:32
 * @copyright: Copyright (c) 2025 Seek Dao
 */

#ifndef SOCKET_IMPL_H
#define SOCKET_IMPL_H

#include "socket.h"
#include <string>
#include <cstdint>
#include <cstddef>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>

/**
 * @brief 套接字实现类的内部接口
 */
class SocketImpl {
public:
    SocketImpl(Socket* parent);
    virtual ~SocketImpl();

    // 禁止拷贝
    SocketImpl(const SocketImpl&) = delete;
    SocketImpl& operator=(const SocketImpl&) = delete;

    // 允许移动
    SocketImpl(SocketImpl&&) noexcept;
    SocketImpl& operator=(SocketImpl&&) noexcept;

    // 套接字操作
    virtual bool connect(const std::string& host, std::uint16_t port) = 0;
    virtual int send(const void* data, std::size_t size) = 0;
    virtual int receive(void* buffer, std::size_t size) = 0;
    virtual void disconnect() = 0;
    virtual bool is_connected() const = 0;

    // 地址信息
    virtual std::string get_remote_address() const = 0;
    virtual std::uint16_t get_remote_port() const = 0;
    virtual std::string get_local_address() const = 0;
    virtual std::uint16_t get_local_port() const = 0;

    // 设置底层套接字描述符
    virtual bool set_socket_descriptor(int socket_fd, const std::string& remote_addr, std::uint16_t remote_port) = 0;

    // 线程安全的状态访问
    void set_state(Socket::State state);
    Socket::State get_state() const;

    // 错误处理
    void set_last_error(Socket::Error error, const std::string& error_msg);
    std::pair<Socket::Error, std::string> get_last_error() const;

    // 回调函数调用
    void invoke_data_handler(const std::vector<std::uint8_t>& data) {
        if (parent_ && parent_->data_handler_) {
            parent_->data_handler_(data);
        }
    }
    
    void invoke_connection_handler() {
        if (parent_ && parent_->connection_handler_) {
            parent_->connection_handler_();
        }
    }
    
    void invoke_disconnection_handler() {
        if (parent_ && parent_->disconnection_handler_) {
            parent_->disconnection_handler_();
        }
    }
    
    void invoke_error_handler(Socket::Error error, const std::string& error_msg) {
        if (parent_ && parent_->error_handler_) {
            parent_->error_handler_(error, error_msg);
        }
    }

    // 异步操作支持
    void start_event_loop();
    void stop_event_loop();
    
protected:
    // 运行事件循环的纯虚方法，由子类实现
    virtual void run_event_loop() = 0;
    // 父套接字指针
    Socket* parent_;

    // 线程控制
    std::atomic<bool> running_;
    std::thread event_thread_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;

    // 状态信息
    Socket::State state_;
    Socket::Error last_error_;
    std::string last_error_msg_;
};

/**
 * @brief 创建适合当前平台的套接字实现
 */
std::unique_ptr<SocketImpl> create_socket_impl(Socket* parent);

#endif // SOCKET_IMPL_H