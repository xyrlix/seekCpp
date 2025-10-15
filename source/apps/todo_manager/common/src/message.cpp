#include "message.h"
#include <sstream>
#include <stdexcept>
#include <iostream>

std::string Message::serialize() const {
    try {
        json j;
        j["type"] = type;
        j["body"] = body;
        return j.dump();
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to serialize message: " + std::string(e.what()));
    }
}

Message Message::deserialize(const std::string& data) {
    try {
        json j = json::parse(data);
        Message msg;
        msg.type = j["type"].get<MessageType>();
        msg.body = j["body"].get<std::string>();
        return msg;
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to parse message: " + std::string(e.what()));
    }
}

std::string Message::messageTypeToString(MessageType type) {
    switch (type) {
        case MessageType::LOGIN_REQUEST: return "LOGIN_REQUEST";
        case MessageType::LOGIN_RESPONSE: return "LOGIN_RESPONSE";
        case MessageType::REGISTER_REQUEST: return "REGISTER_REQUEST";
        case MessageType::REGISTER_RESPONSE: return "REGISTER_RESPONSE";
        case MessageType::ADD_TASK_REQUEST: return "ADD_TASK_REQUEST";
        case MessageType::ADD_TASK_RESPONSE: return "ADD_TASK_RESPONSE";
        case MessageType::LIST_TASKS_REQUEST: return "LIST_TASKS_REQUEST";
        case MessageType::LIST_TASKS_RESPONSE: return "LIST_TASKS_RESPONSE";
        case MessageType::DELETE_TASK_REQUEST: return "DELETE_TASK_REQUEST";
        case MessageType::DELETE_TASK_RESPONSE: return "DELETE_TASK_RESPONSE";
        case MessageType::MARK_TASK_COMPLETED_REQUEST: return "MARK_TASK_COMPLETED_REQUEST";
        case MessageType::MARK_TASK_COMPLETED_RESPONSE: return "MARK_TASK_COMPLETED_RESPONSE";
        case MessageType::SEARCH_TASKS_REQUEST: return "SEARCH_TASKS_REQUEST";
        case MessageType::SEARCH_TASKS_RESPONSE: return "SEARCH_TASKS_RESPONSE";
        case MessageType::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

MessageType Message::stringToMessageType(const std::string& typeStr) {
    if (typeStr == "LOGIN_REQUEST") return MessageType::LOGIN_REQUEST;
    if (typeStr == "LOGIN_RESPONSE") return MessageType::LOGIN_RESPONSE;
    if (typeStr == "REGISTER_REQUEST") return MessageType::REGISTER_REQUEST;
    if (typeStr == "REGISTER_RESPONSE") return MessageType::REGISTER_RESPONSE;
    if (typeStr == "ADD_TASK_REQUEST") return MessageType::ADD_TASK_REQUEST;
    if (typeStr == "ADD_TASK_RESPONSE") return MessageType::ADD_TASK_RESPONSE;
    if (typeStr == "LIST_TASKS_REQUEST") return MessageType::LIST_TASKS_REQUEST;
    if (typeStr == "LIST_TASKS_RESPONSE") return MessageType::LIST_TASKS_RESPONSE;
    if (typeStr == "DELETE_TASK_REQUEST") return MessageType::DELETE_TASK_REQUEST;
    if (typeStr == "DELETE_TASK_RESPONSE") return MessageType::DELETE_TASK_RESPONSE;
    if (typeStr == "MARK_TASK_COMPLETED_REQUEST") return MessageType::MARK_TASK_COMPLETED_REQUEST;
    if (typeStr == "MARK_TASK_COMPLETED_RESPONSE") return MessageType::MARK_TASK_COMPLETED_RESPONSE;
    if (typeStr == "SEARCH_TASKS_REQUEST") return MessageType::SEARCH_TASKS_REQUEST;
    if (typeStr == "SEARCH_TASKS_RESPONSE") return MessageType::SEARCH_TASKS_RESPONSE;
    if (typeStr == "ERROR") return MessageType::ERROR;
    
    // 默认返回ERROR类型
    return MessageType::ERROR;
}