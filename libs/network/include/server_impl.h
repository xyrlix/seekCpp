/**
 * @file    : server_impl.h
 * @brief   : 服务器实现的内部接口
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 2025-10-05 04:30:32
 * @copyright: Copyright (c) 2025 Seek Dao
 */

#ifndef SERVER_IMPL_H
#define SERVER_IMPL_H

#include "server.h"
#include <atomic>
#include <unordered_map>
#include <queue>
#include <memory>

// 前向声明
class ServerImpl;

/**
 * @brief 服务器实现类的内部接口
 */
class ServerImpl {
public:
    ServerImpl(Server* parent); // 只声明，不实现
    virtual ~ServerImpl();
    
    // 禁止拷贝
    ServerImpl(const ServerImpl&) = delete;
    ServerImpl& operator=(const ServerImpl&) = delete;
    
    // 允许移动
    ServerImpl(ServerImpl&&) noexcept;
    ServerImpl& operator=(ServerImpl&&) noexcept;
    
    // 服务器操作
    virtual bool start(uint16_t port, size_t max_connections) = 0;
    virtual void stop() = 0;
    virtual bool is_running() const = 0;
    
    // 客户端管理
    virtual int send_to_client(ClientConnection::Id client_id, const void* data, size_t size) = 0;
    virtual size_t broadcast(const void* data, size_t size) = 0;
    virtual void disconnect_client(ClientConnection::Id client_id) = 0;
    virtual size_t get_connected_clients_count() const = 0;
    
    // 线程安全的状态访问
    void set_state(Server::State state);
    Server::State get_state() const;
    
    // 错误处理
    void set_last_error(const std::string& error_msg);
    std::string get_last_error() const;
    
    // 回调函数调用
    void invoke_data_handler(ClientConnection::Id client_id, const std::vector<uint8_t>& data) {
        if (parent_ && parent_->data_handler_) {
            parent_->data_handler_(client_id, data);
        }
    }
    
    void invoke_connection_handler(ClientConnection::Id client_id) {
        if (parent_ && parent_->connection_handler_) {
            parent_->connection_handler_(client_id);
        }
    }
    
    void invoke_disconnection_handler(ClientConnection::Id client_id) {
        if (parent_ && parent_->disconnection_handler_) {
            parent_->disconnection_handler_(client_id);
        }
    }
    
    void invoke_error_handler(const std::string& error_msg) {
        if (parent_ && parent_->error_handler_) {
            parent_->error_handler_(error_msg);
        }
    }
    
    // 添加客户端连接
    void add_client(std::unique_ptr<ClientConnection> client);
    
protected:
    // 父服务器指针
    Server* parent_;
    
    // 线程控制
    std::atomic<bool> running_;
    std::thread main_thread_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    
    // 状态信息
    Server::State state_;
    std::string last_error_;
    
    // 客户端连接管理
    std::unordered_map<ClientConnection::Id, std::unique_ptr<ClientConnection>> clients_;
    std::atomic<ClientConnection::Id> next_client_id_;
    
    // 最大连接数
    size_t max_connections_;
};

/**
 * @brief 创建适合当前平台的服务器实现
 */
std::unique_ptr<ServerImpl> create_server_impl(Server* parent);

#endif // SERVER_IMPL_H