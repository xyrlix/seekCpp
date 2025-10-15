/**
 * @file    : server.h
 * @brief   : C++20 TCP服务器类
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 2025-10-05 04:30:32
 * @copyright: Copyright (c) 2025 Seek Dao
 */

#ifndef SERVER_H
#define SERVER_H

#include "socket.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include <vector>
#include <atomic>
#include <thread>
#include <mutex>

// 前向声明
class ServerImpl;
class ClientConnection;

/**
 * @brief 客户端连接类，表示与服务器连接的客户端
 */
class ClientConnection {
public:
    using Id = uint64_t;
    
    ClientConnection(Id id, std::unique_ptr<Socket> socket);
    ~ClientConnection();
    
    // 禁止拷贝
    ClientConnection(const ClientConnection&) = delete;
    ClientConnection& operator=(const ClientConnection&) = delete;
    
    // 允许移动
    ClientConnection(ClientConnection&&) noexcept;
    ClientConnection& operator=(ClientConnection&&) noexcept;
    
    /**
     * @brief 获取客户端连接ID
     * @return 客户端连接ID
     */
    Id get_id() const;
    
    /**
     * @brief 发送数据给客户端
     * @param data 要发送的数据
     * @param size 数据大小
     * @return 实际发送的字节数，-1表示错误
     */
    int send(const void* data, size_t size);
    
    /**
     * @brief 异步发送数据给客户端
     * @param data 要发送的数据
     * @param size 数据大小
     * @return 包含发送结果的future
     */
    std::future<int> send_async(const void* data, size_t size);
    
    /**
     * @brief 断开与客户端的连接
     */
    void disconnect();
    
    /**
     * @brief 检查连接是否活跃
     * @return 连接是否活跃
     */
    bool is_active() const;
    
    /**
     * @brief 获取客户端的IP地址
     * @return 客户端的IP地址
     */
    std::string get_client_address() const;
    
    /**
     * @brief 获取客户端的端口号
     * @return 客户端的端口号
     */
    uint16_t get_client_port() const;
    
    /**
     * @brief 设置数据接收回调函数
     * @param handler 数据接收处理函数
     */
    void set_data_handler(std::function<void(Id, const std::vector<uint8_t>&)> handler);
    
    /**
     * @brief 设置断开连接回调函数
     * @param handler 断开连接处理函数
     */
    void set_disconnection_handler(std::function<void(Id)> handler);
    
private:
    Id id_;
    std::unique_ptr<Socket> socket_;
    std::function<void(Id, const std::vector<uint8_t>&)> data_handler_;
    std::function<void(Id)> disconnection_handler_;
};

/**
 * @brief TCP服务器类，用于接受和管理客户端连接
 */
class Server {
public:
    enum class State {
        Stopped,
        Starting,
        Running,
        Stopping
    };
    
    using DataHandler = std::function<void(ClientConnection::Id, const std::vector<uint8_t>&)>;
    using ConnectionHandler = std::function<void(ClientConnection::Id)>;
    using ErrorHandler = std::function<void(const std::string&)>;
    
    Server();
    ~Server();
    
    // 禁止拷贝
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;
    
    // 允许移动
    Server(Server&&) noexcept;
    Server& operator=(Server&&) noexcept;
    
    /**
     * @brief 开始监听指定端口
     * @param port 端口号
     * @param max_connections 最大连接数（默认为100）
     * @return 是否成功开始监听
     */
    bool start(uint16_t port, size_t max_connections = 100);
    
    /**
     * @brief 停止服务器
     */
    void stop();
    
    /**
     * @brief 检查服务器是否正在运行
     * @return 服务器是否正在运行
     */
    bool is_running() const;
    
    /**
     * @brief 获取服务器状态
     * @return 服务器状态
     */
    State get_state() const;
    
    /**
     * @brief 向指定客户端发送数据
     * @param client_id 客户端ID
     * @param data 要发送的数据
     * @param size 数据大小
     * @return 实际发送的字节数，-1表示错误
     */
    int send_to_client(ClientConnection::Id client_id, const void* data, size_t size);
    
    /**
     * @brief 向所有客户端广播数据
     * @param data 要发送的数据
     * @param size 数据大小
     * @return 成功发送的客户端数量
     */
    size_t broadcast(const void* data, size_t size);
    
    /**
     * @brief 断开与指定客户端的连接
     * @param client_id 客户端ID
     */
    void disconnect_client(ClientConnection::Id client_id);
    
    /**
     * @brief 获取当前连接的客户端数量
     * @return 连接的客户端数量
     */
    size_t get_connected_clients_count() const;
    
    /**
     * @brief 设置数据接收回调函数
     * @param handler 数据接收处理函数
     */
    void set_data_handler(DataHandler handler);
    
    /**
     * @brief 设置客户端连接回调函数
     * @param handler 客户端连接处理函数
     */
    void set_connection_handler(ConnectionHandler handler);
    
    /**
     * @brief 设置客户端断开连接回调函数
     * @param handler 客户端断开连接处理函数
     */
    void set_disconnection_handler(ConnectionHandler handler);
    
    /**
     * @brief 设置错误处理回调函数
     * @param handler 错误处理函数
     */
    void set_error_handler(ErrorHandler handler);
    
private:
    std::unique_ptr<ServerImpl> impl_;
    
    // 回调函数
    DataHandler data_handler_;
    ConnectionHandler connection_handler_;
    ConnectionHandler disconnection_handler_;
    ErrorHandler error_handler_;

    // 允许 ServerImpl 访问这些私有成员
    friend class ServerImpl;
};

#endif // SERVER_H