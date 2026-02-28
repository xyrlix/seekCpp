/*
 * @file    : network_utils.h
 * @brief   : 网络通信工具类
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 2025-10-05 04:30:32
 * @copyright: Copyright (c) 2025 Seek Dao
 */

#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <cstdint>   // 用于uint32_t类型
#include <string>    // 用于std::string
#include <vector>    // 用于std::vector
#include <arpa/inet.h>    // 用于htonl/ntohl
#include <sys/socket.h>   // 用于socket相关操作

namespace todo_manager {
namespace network {

/**
 * @brief 发送消息到套接字，包含长度前缀
 * @param socket 目标套接字文件描述符
 * @param message 要发送的消息
 */
void send_message(int socket, const std::string& message);

/**
 * @brief 从套接字接收消息，读取长度前缀和消息体
 * @param socket 源套接字文件描述符
 * @return 接收到的消息
 */
std::string receive_message(int socket);

/**
 * @brief 安全地将字符串转换为网络字节序的长度前缀
 * @param length 消息长度
 * @return 包含网络字节序长度的缓冲区
 */
std::vector<char> create_message_header(uint32_t length);

/**
 * @brief 从网络字节序的长度前缀中解析出消息长度
 * @param header 包含网络字节序长度的缓冲区
 * @return 解析出的消息长度
 */
uint32_t parse_message_header(const std::vector<char>& header);

} // namespace network
} // namespace todo_manager

#endif // NETWORK_UTILS_H