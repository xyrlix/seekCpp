/**
 * @file    : socket.h
 * @brief   : C++20 套接字抽象类
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 2025-10-05 04:30:32
 * @copyright: Copyright (c) 2025 Seek Dao
 */

#ifndef SOCKET_H
#define SOCKET_H

#include <string>
#include <memory>
#include <cstdint>
#include <cstddef>
#include <vector>
#include <functional>
#include <future>
#include <mutex>
#include <condition_variable>

// 前向声明
class SocketImpl;

/**
 * @brief 套接字抽象类，提供跨平台的TCP套接字功能
 */
class Socket {
public:
    enum class State {
        Disconnected,
        Connecting,
        Connected,
        Disconnecting
    };

    enum class Error {
        None,
        SystemError,
        ConnectionRefused,
        TimedOut,
        ConnectionClosed,
        InvalidAddress,
        InvalidArgument,
        NotConnected,
        InvalidState
    };

    using ErrorHandler = std::function<void(Error, const std::string&)>;
    using DataHandler = std::function<void(const std::vector<std::uint8_t>&)>;
    using ConnectionHandler = std::function<void()>;

    Socket();
    ~Socket();

    // 禁止拷贝
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    // 允许移动
    Socket(Socket&&) noexcept;
    Socket& operator=(Socket&&) noexcept;

    /**
     * @brief 连接到指定主机和端口
     * @param host 主机名或IP地址
     * @param port 端口号
     * @return 是否连接成功
     */
    bool connect(const std::string& host, std::uint16_t port);

    /**
     * @brief 异步连接到指定主机和端口
     * @param host 主机名或IP地址
     * @param port 端口号
     * @return 包含连接结果的future
     */
    std::future<bool> connect_async(const std::string& host, std::uint16_t port);

    /**
     * @brief 发送数据
     * @param data 要发送的数据
     * @param size 数据大小
     * @return 实际发送的字节数，-1表示错误
     */
    int send(const void* data, std::size_t size);

    /**
     * @brief 异步发送数据
     * @param data 要发送的数据
     * @param size 数据大小
     * @return 包含发送结果的future
     */
    std::future<int> send_async(const void* data, std::size_t size);

    /**
     * @brief 接收数据
     * @param buffer 接收缓冲区
     * @param size 缓冲区大小
     * @return 实际接收的字节数，-1表示错误
     */
    int receive(void* buffer, std::size_t size);

    /**
     * @brief 异步接收数据
     * @param buffer 接收缓冲区
     * @param size 缓冲区大小
     * @return 包含接收结果的future
     */
    std::future<int> receive_async(void* buffer, std::size_t size);

    /**
     * @brief 断开连接
     */
    void disconnect();

    /**
     * @brief 检查套接字是否已连接
     * @return 是否已连接
     */
    bool is_connected() const;

    /**
     * @brief 获取当前套接字状态
     * @return 套接字状态
     */
    State get_state() const;

    /**
     * @brief 设置数据接收回调函数
     * @param handler 数据接收处理函数
     */
    void set_data_handler(DataHandler handler);

    /**
     * @brief 设置连接建立回调函数
     * @param handler 连接建立处理函数
     */
    void set_connection_handler(ConnectionHandler handler);

    /**
     * @brief 设置断开连接回调函数
     * @param handler 断开连接处理函数
     */
    void set_disconnection_handler(ConnectionHandler handler);

    /**
     * @brief 设置错误处理回调函数
     * @param handler 错误处理函数
     */
    void set_error_handler(ErrorHandler handler);

    /**
     * @brief 获取远程端点的地址
     * @return 远程端点地址
     */
    std::string get_remote_address() const;

    /**
     * @brief 获取远程端点的端口
     * @return 远程端点端口
     */
    uint16_t get_remote_port() const;

    /**
     * @brief 获取本地端点的地址
     * @return 本地端点地址
     */
    std::string get_local_address() const;

    /**
     * @brief 获取本地端点的端口
     * @return 本地端点端口
     */
    uint16_t get_local_port() const;

    /**
     * @brief 设置底层套接字描述符（仅用于内部使用）
     * @param socket_fd 套接字描述符
     * @param remote_addr 远程地址
     * @param remote_port 远程端口
     * @return 是否设置成功
     */
    bool set_socket_descriptor(int socket_fd, const std::string& remote_addr, uint16_t remote_port);

    /**
     * @brief 获取最后一个错误信息
     * @return 包含错误类型和错误信息的pair
     */
    std::pair<Error, std::string> get_last_error() const;

private:
    std::unique_ptr<SocketImpl> impl_;
    
    // 回调函数
    DataHandler data_handler_;
    ConnectionHandler connection_handler_;
    ConnectionHandler disconnection_handler_;
    ErrorHandler error_handler_;

    // 允许 SocketImpl 访问这些私有成员
    friend class SocketImpl;
};

#endif // SOCKET_H