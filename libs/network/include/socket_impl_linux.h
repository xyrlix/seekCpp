/**
 * @file    : socket_impl_linux.h
 * @brief   : Linux平台的套接字实现
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 2025-10-05 04:30:32
 * @copyright: Copyright (c) 2025 Seek Dao
 */

#ifndef SOCKET_IMPL_LINUX_H
#define SOCKET_IMPL_LINUX_H

#include "socket_impl.h"
#include <string>
#include <cstdint>
#include <vector>

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

/**
 * @brief Linux平台的套接字实现
 */
class SocketImplLinux : public SocketImpl {
public:
    SocketImplLinux(Socket* parent);
    ~SocketImplLinux() override;

    // 禁止拷贝
    SocketImplLinux(const SocketImplLinux&) = delete;
    SocketImplLinux& operator=(const SocketImplLinux&) = delete;

    // 允许移动
    SocketImplLinux(SocketImplLinux&&) noexcept;
    SocketImplLinux& operator=(SocketImplLinux&&) noexcept;

    // 实现基类的纯虚函数
    bool connect(const std::string& host, std::uint16_t port) override;
    int send(const void* data, std::size_t size) override;
    int receive(void* buffer, std::size_t size) override;
    void disconnect() override;
    bool is_connected() const override;

    // 地址信息
    std::string get_remote_address() const override;
    std::uint16_t get_remote_port() const override;
    std::string get_local_address() const override;
    std::uint16_t get_local_port() const override;

    // 设置底层套接字描述符
    bool set_socket_descriptor(int socket_fd, const std::string& remote_addr, std::uint16_t remote_port) override;

private:
    // 设置套接字为非阻塞模式
    bool set_non_blocking(bool non_blocking);

    // 处理网络事件循环
    void run_event_loop() override;

    // 套接字描述符
    int socket_fd_;
    
    // 远程地址信息
    struct sockaddr_in remote_addr_;
    
    // 本地地址信息
    struct sockaddr_in local_addr_;
};

#endif // SOCKET_IMPL_LINUX_H