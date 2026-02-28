/**
 * @file    : server_impl_linux.h
 * @brief   : Linux平台的服务器实现
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 2025-10-05 04:30:32
 * @copyright: Copyright (c) 2025 Seek Dao
 */

#ifndef SERVER_IMPL_LINUX_H
#define SERVER_IMPL_LINUX_H

#include "server_impl.h"

#ifdef _WIN32
#error This file is for Linux platform only
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>
#include <cstring>
#endif

// 前向声明
class Socket;

/**
 * @brief Linux平台的服务器实现
 */
class ServerImplLinux : public ServerImpl {
public:
    ServerImplLinux(Server* parent);
    ~ServerImplLinux() override;
    
    // 禁止拷贝
    ServerImplLinux(const ServerImplLinux&) = delete;
    ServerImplLinux& operator=(const ServerImplLinux&) = delete;
    
    // 允许移动
    ServerImplLinux(ServerImplLinux&&) noexcept;
    ServerImplLinux& operator=(ServerImplLinux&&) noexcept;
    
    // 实现基类的纯虚函数
    bool start(uint16_t port, size_t max_connections) override;
    void stop() override;
    bool is_running() const override;
    
    // 客户端管理
    int send_to_client(ClientConnection::Id client_id, const void* data, size_t size) override;
    size_t broadcast(const void* data, size_t size) override;
    void disconnect_client(ClientConnection::Id client_id) override;
    size_t get_connected_clients_count() const override;
    
private:
    // 设置套接字为非阻塞模式
    bool set_non_blocking(int socket_fd, bool non_blocking);
    
    // 处理连接接受
    std::unique_ptr<Socket> accept_connection();
    
    // 服务器主循环
    void server_loop();
    
    // 套接字描述符
    int server_socket_fd_;
    
    // 服务器地址信息
    struct sockaddr_in server_addr_;
    
    // 最大连接数
    size_t max_connections_;
};

#endif // SERVER_IMPL_LINUX_H