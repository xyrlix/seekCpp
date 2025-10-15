/*
 * @file    : main.cpp
 * @brief   : 服务器程序入口
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 2025-10-05 04:30:32
 * @copyright: Copyright (c) 2025 Seek Dao
 */

#include "server.h"
#include <iostream>
#include <csignal>
#include <thread>

// 全局服务器对象指针，用于信号处理
Server* server_ptr = nullptr;

// 信号处理函数
void signal_handler(int signal) {
    std::cout << "Received signal " << signal << ", shutting down server..." << std::endl;
    if (server_ptr) {
        server_ptr->stop();
    }
    // 退出程序
    exit(0);
}

int main() {
    try {
        // 设置信号处理
        signal(SIGINT, signal_handler);
        signal(SIGTERM, signal_handler);
        
        // 创建服务器对象，监听12345端口
        Server s("12345");
        server_ptr = &s;
        
        // 启动服务器
        s.start();
        
        std::cout << "Server running. Press Ctrl+C to stop." << std::endl;
        
        // 主线程等待
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}