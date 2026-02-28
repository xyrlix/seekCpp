#ifndef SOCKET_H
#define SOCKET_H

#include <string>
#include <vector>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <cstring>
#include <netdb.h>
#include "spdlog/spdlog.h"

// 前向声明
class Socket;

// 定义Socket状态枚举
enum class SocketState {
    Disconnected,
    Connecting,
    Connected,
    Disconnecting
};

// 定义Socket错误枚举
enum class SocketError {
    None,
    ConnectionRefused,
    HostNotFound,
    NetworkError,
    Timeout,
    UnknownError
};

/**
 * @class Socket
 * @brief 提供TCP套接字通信功能的类
 */
class Socket {
public:
    using ConnectCallback = std::function<void(bool, SocketError)>;
    using DisconnectCallback = std::function<void()>;
    using ReceiveCallback = std::function<void(const std::string&)>;
    using ErrorCallback = std::function<void(SocketError)>;
    
    /**
     * @brief 构造函数
     */
    Socket();
    
    /**
     * @brief 析构函数
     */
    ~Socket();
    
    /**
     * @brief 连接到服务器
     * @param host 服务器主机名或IP地址
     * @param port 服务器端口号
     * @param timeout_ms 连接超时时间（毫秒），默认为5000
     * @return 连接是否成功
     */
    bool connect(const std::string& host, uint16_t port, int timeout_ms = 5000);
    
    /**
     * @brief 断开连接
     */
    void disconnect();
    
    /**
     * @brief 发送数据
     * @param data 要发送的数据
     * @return 发送是否成功
     */
    bool send(const std::string& data);
    
    /**
     * @brief 检查套接字是否已连接
     * @return 是否已连接
     */
    bool is_connected() const;
    
    /**
     * @brief 获取当前套接字状态
     * @return 套接字状态
     */
    SocketState get_state() const;
    
    /**
     * @brief 设置连接回调函数
     * @param callback 连接回调函数
     */
    void set_connection_handler(const ConnectCallback& callback);
    
    /**
     * @brief 设置断开连接回调函数
     * @param callback 断开连接回调函数
     */
    void set_disconnection_handler(const DisconnectCallback& callback);
    
    /**
     * @brief 设置接收数据回调函数
     * @param callback 接收数据回调函数
     */
    void set_receive_handler(const ReceiveCallback& callback);
    
    /**
     * @brief 设置错误回调函数
     * @param callback 错误回调函数
     */
    void set_error_handler(const ErrorCallback& callback);
    
private:
    // 内部使用的套接字文件描述符
    int socket_;
    
    // 套接字状态
    std::atomic<SocketState> state_;
    
    // 线程相关
    std::thread receive_thread_;
    std::atomic<bool> receive_thread_running_;
    
    // 回调函数
    ConnectCallback connect_callback_;
    DisconnectCallback disconnect_callback_;
    ReceiveCallback receive_callback_;
    ErrorCallback error_callback_;
    
    // 互斥锁
    std::mutex mutex_;
    std::condition_variable cv_;
    
    /**
     * @brief 接收数据的内部循环
     */
    void receive_loop();
    
    /**
     * @brief 将套接字设置为非阻塞模式
     * @return 是否设置成功
     */
    bool set_non_blocking();
    
    /**
     * @brief 处理连接超时
     * @param timeout_ms 超时时间（毫秒）
     * @return 连接是否成功
     */
    bool handle_connection_timeout(int timeout_ms);
    
    /**
     * @brief 处理接收错误
     * @param error 错误码
     */
    void handle_receive_error(int error);
};

#endif // SOCKET_H