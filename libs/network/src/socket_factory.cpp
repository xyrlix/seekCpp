/**
 * @file    : socket_factory.cpp
 * @brief   : Socket对象工厂函数
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 2025-10-05 04:30:32
 * @copyright: Copyright (c) 2025 Seek Dao
 */

#include "../include/socket.h"

/**
 * @brief 创建一个新的Socket对象
 * @return 指向Socket对象的unique_ptr
 */
std::unique_ptr<Socket> create_socket() {
    return std::make_unique<Socket>();
}