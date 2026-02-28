/*
 * @file    : network_utils.cpp
 * @brief   : 网络通信工具类的实现
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 2025-10-05 04:30:32
 * @copyright: Copyright (c) 2025 Seek Dao
 */

#include "network_utils.h"
#include <iostream>
#include <cstring>
#include <stdexcept>
#include <cstdint>
#include <string>
#include <vector>
#include <unistd.h>
#include <errno.h>

namespace todo_manager {
namespace network {

/**
 * @brief 发送消息到套接字，包含长度前缀
 * @param socket 目标套接字文件描述符
 * @param message 要发送的消息
 */
void send_message(int socket, const std::string& message) {
    try {
        // 先发送消息长度
        uint32_t length = static_cast<uint32_t>(message.size());
        uint32_t network_length = htonl(length); // 转换为网络字节序
        char header[4];
        memcpy(header, &network_length, 4);
        
        // 发送头部
        size_t total_sent = 0;
        while (total_sent < 4) {
            ssize_t sent = ::send(socket, header + total_sent, 4 - total_sent, 0);
            if (sent < 0) {
                throw std::runtime_error("Failed to send message header: " + std::string(strerror(errno)));
            }
            total_sent += sent;
        }
        
        // 再发送消息体
        const char* buffer = message.c_str();
        total_sent = 0;
        size_t message_size = message.size();
        
        while (total_sent < message_size) {
            ssize_t sent = ::send(socket, buffer + total_sent, message_size - total_sent, 0);
            if (sent < 0) {
                throw std::runtime_error("Failed to send message body: " + std::string(strerror(errno)));
            }
            total_sent += sent;
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to send message: " + std::string(e.what()));
    }
}

/**
 * @brief 从套接字接收消息，读取长度前缀和消息体
 * @param socket 源套接字文件描述符
 * @return 接收到的消息
 */
std::string receive_message(int socket) {
    try {
        // 先读取消息长度
        char header[4];
        size_t total_read = 0;
        
        while (total_read < 4) {
            ssize_t read = ::read(socket, header + total_read, 4 - total_read);
            if (read <= 0) {
                if (read == 0) {
                    return "Connection closed by peer.";
                }
                throw std::runtime_error("Failed to read message header: " + std::string(strerror(errno)));
            }
            total_read += read;
        }
        
        // 解析长度，从网络字节序转换为主机字节序
        uint32_t network_length;
        memcpy(&network_length, header, 4);
        uint32_t msg_length = ntohl(network_length);
        
        // 读取完整的消息体
        std::vector<char> buffer(msg_length);
        total_read = 0;
        
        while (total_read < msg_length) {
            ssize_t read = ::read(socket, buffer.data() + total_read, msg_length - total_read);
            if (read <= 0) {
                if (read == 0) {
                    return "Connection closed by peer.";
                }
                throw std::runtime_error("Failed to read message body: " + std::string(strerror(errno)));
            }
            total_read += read;
        }
        
        // 将字符缓冲区转换为字符串
        return std::string(buffer.begin(), buffer.end());
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to receive message: " + std::string(e.what()));
    }
}

/**
 * @brief 安全地将字符串转换为网络字节序的长度前缀
 * @param length 消息长度
 * @return 包含网络字节序长度的缓冲区
 */
std::vector<char> create_message_header(uint32_t length) {
    std::vector<char> header(4);
    uint32_t network_length = htonl(length);
    memcpy(header.data(), &network_length, 4);
    return header;
}

/**
 * @brief 从网络字节序的长度前缀中解析出消息长度
 * @param header 包含网络字节序长度的缓冲区
 * @return 解析出的消息长度
 */
uint32_t parse_message_header(const std::vector<char>& header) {
    if (header.size() != 4) {
        throw std::runtime_error("Invalid message header size");
    }
    
    uint32_t network_length;
    memcpy(&network_length, header.data(), 4);
    return ntohl(network_length);
}

} // namespace network
} // namespace todo_manager