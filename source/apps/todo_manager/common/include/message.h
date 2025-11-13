/**
 * @file    : message.h
 * @brief   : 定义消息结构体
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 2025-10-05 04:30:32
 * @copyright: Copyright (c) 2025 Seek Dao
 */

#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <vector>
#include "task.h"
#include "user.h"
#include <nlohmann/json.hpp>

// 使用nlohmann::json的别名
using json = nlohmann::json;

// 定义消息类型
enum class MessageType {
    LOGIN_REQUEST,
    LOGIN_RESPONSE,
    REGISTER_REQUEST,
    REGISTER_RESPONSE,
    ADD_TASK_REQUEST,
    ADD_TASK_RESPONSE,
    LIST_TASKS_REQUEST,
    LIST_TASKS_RESPONSE,
    DELETE_TASK_REQUEST,
    DELETE_TASK_RESPONSE,
    MARK_TASK_COMPLETED_REQUEST,
    MARK_TASK_COMPLETED_RESPONSE,
    SEARCH_TASKS_REQUEST,
    SEARCH_TASKS_RESPONSE,
    ERROR
};

// 消息包装器
class Message {
public:
    MessageType type;
    std::string body;

    // 将消息序列化以便于网络传输（JSON格式）
    std::string serialize() const;

    // 从网络数据解析出消息（JSON格式）
    static Message deserialize(const std::string& data);

    // 辅助方法：将MessageType转换为字符串
    static std::string messageTypeToString(MessageType type);
    
    // 辅助方法：将字符串转换为MessageType
    static MessageType stringToMessageType(const std::string& typeStr);
};

// 为TaskPriority和MessageType添加JSON序列化支持
namespace nlohmann {
    template <>
    struct adl_serializer<TaskPriority> {
        static void to_json(json& j, const TaskPriority& p) {
            j = static_cast<int>(p);
        }
        
        static void from_json(const json& j, TaskPriority& p) {
            p = static_cast<TaskPriority>(j.get<int>());
        }
    };
    
    template <>
    struct adl_serializer<MessageType> {
        static void to_json(json& j, const MessageType& t) {
            j = Message::messageTypeToString(t);
        }
        
        static void from_json(const json& j, MessageType& t) {
            t = Message::stringToMessageType(j.get<std::string>());
        }
    };
}

#endif // MESSAGE_H