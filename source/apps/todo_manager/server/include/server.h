/*
 * @file    : server.h
 * @brief   : 定义服务器类
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 2025-10-05 04:30:32
 * @copyright: Copyright (c) 2025 Seek Dao
 */

#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <cstring>
#include "user.h"
#include "task.h"
#include "message.h"
#include "database.h"

class Server {
public:
    Server(const std::string& port);
    ~Server();
    void start();
    void stop();

private:
    // 客户端会话上下文
    struct ClientSession {
        int socket; // 套接字文件描述符
        std::string username;
        std::thread session_thread;
        std::atomic<bool> running; // 会话线程运行标志
    };
    
    void initialize_database();
    void accept_connections();
    void handle_client(std::shared_ptr<ClientSession> session);
    Message handle_message(const Message& msg, const std::shared_ptr<ClientSession>& session);

    // 数据存储
    std::unordered_map<std::string, User> users_;
    std::unordered_map<std::string, std::vector<Task>> user_tasks_;
    int next_task_id_ = 1; // 用于分配唯一的任务ID
    
    // 数据库对象
    Database database_;
    
    // 服务器套接字和状态
    int server_socket_; // 服务器套接字文件描述符
    std::string port_;
    std::atomic<bool> running_; // 服务器运行标志
    std::thread accept_thread_; // 接受连接的线程
    std::mutex sessions_mutex_; // 保护会话列表的互斥锁
    std::vector<std::shared_ptr<ClientSession>> sessions_; // 活动会话列表
};

#endif // SERVER_H