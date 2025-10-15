/**
 * @file    : socket_factory.h
 * @brief   : Socket对象工厂函数声明
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 2025-10-05 04:30:32
 * @copyright: Copyright (c) 2025 Seek Dao
 */

#ifndef SOCKET_FACTORY_H
#define SOCKET_FACTORY_H

#include "socket.h"
#include <memory>

/**
 * @brief 创建一个新的Socket对象
 * @return 指向Socket对象的unique_ptr
 */
std::unique_ptr<Socket> create_socket();

#endif // SOCKET_FACTORY_H