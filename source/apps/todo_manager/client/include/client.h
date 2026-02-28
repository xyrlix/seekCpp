/*
 * @file    : client.h
 * @brief   : 定义客户端类
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 2025-10-05 04:30:32
 * @copyright: Copyright (c) 2025 Seek Dao
 */

#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include "message.h"
#include <nlohmann/json.hpp>
#include "socket.h"
#include "spdlog/spdlog.h"

class Client {
public:
    Client(const std::string& host, const std::string& port);
    ~Client();
    void run();

private:
    // 处理接收到的数据
    void handle_received_data(const std::string& data);
    // 显示任务列表
    void display_tasks(const nlohmann::json& tasks_json);
    // 处理命令行输入
    void handle_input();
    // 断开连接
    void disconnect();
    
    // 用户管理相关方法
    void handle_register();
    void handle_login();
    void handle_logout();
    
    // 任务管理相关方法
    void handle_add_task();
    void handle_list_tasks();
    void handle_complete_task();
    void handle_delete_task();
    
    // 用户界面相关方法
    void display_menu();
    void display_help();

    // 网络连接相关
    std::string host_;
    int port_;
    Socket socket_;
    bool connected_ = false;

    // 用户信息
    std::string username_;
    bool authenticated_ = false;

    // 日志器
    std::shared_ptr<spdlog::logger> logger_;
};

#endif // CLIENT_H