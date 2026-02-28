#include "client.h"
#include "socket.h"
#include "message.h"
#include "task.h"
#include "user.h"
#include <functional>
#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <algorithm>
#include <regex>
#include <chrono>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <nlohmann/json.hpp>

Client::Client(const std::string& host, const std::string& port)
    : host_(host), port_(std::stoi(port)), socket_(), connected_(false), authenticated_(false) {
    // 初始化日志器
    logger_ = spdlog::get("client");
    if (!logger_) {
        logger_ = spdlog::stdout_color_mt("client");
    }
    
    // 设置Socket回调
    socket_.set_connection_handler([this](bool success, SocketError error) {
        if (success) {
            logger_->info("Connected to server");
            connected_ = true;
        } else {
            logger_->error("Failed to connect to server");
            connected_ = false;
        }
    });
    
    socket_.set_disconnection_handler([this]() {
        logger_->info("Disconnected from server");
        connected_ = false;
        authenticated_ = false;
    });
    
    socket_.set_error_handler([this](SocketError error) {
        logger_->error("Network error occurred");
    });
    
    socket_.set_receive_handler([this](const std::string& data) {
        handle_received_data(data);
    });
}

Client::~Client() {
    disconnect();
}

void Client::run() {
    try {
        // 连接到服务器
        if (!socket_.connect(host_, static_cast<uint16_t>(port_))) {
            auto error = socket_.get_state() == SocketState::Disconnected ? 
                        "Failed to connect" : "Connection in progress";
            logger_->error("{}", error);
            return;
        }
        
        // 启动输入处理线程
        std::thread input_thread(&Client::handle_input, this);
        
        // 主线程保持运行，等待连接关闭
        while (socket_.is_connected()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        // 等待输入线程完成
        if (input_thread.joinable()) {
            input_thread.join();
        }
        
        logger_->info("Client program exiting");
    } catch (const std::exception& e) {
        logger_->error("Failed to connect to server: {}", e.what());
    }
}

void Client::disconnect() {
    socket_.disconnect();
    connected_ = false;
    authenticated_ = false;
}

void Client::handle_received_data(const std::string& data) {
    try {
        // 解析接收到的消息
        Message resp_msg = Message::deserialize(data);
        
        logger_->debug("Received message: {} body: {}", 
                      Message::messageTypeToString(resp_msg.type), 
                      resp_msg.body);
        
        // 处理响应
        switch (resp_msg.type) {
            case MessageType::REGISTER_RESPONSE:
                if (resp_msg.body == "Registration successful") {
                    logger_->info("Registration successful!");
                } else {
                    logger_->error("Registration failed: {}", resp_msg.body);
                }
                break;
                
            case MessageType::LOGIN_RESPONSE:
                if (resp_msg.body == "Login successful") {
                    logger_->info("Login successful!");
                    authenticated_ = true;
                } else {
                    logger_->error("Login failed: {}", resp_msg.body);
                    authenticated_ = false;
                }
                break;
                
            case MessageType::ADD_TASK_RESPONSE:
                if (resp_msg.body == "Task added successfully") {
                    logger_->info("Task added successfully!");
                } else {
                    logger_->error("Failed to add task: {}", resp_msg.body);
                }
                break;
                
            case MessageType::LIST_TASKS_RESPONSE:
                try {
                    // 解析任务列表
                    nlohmann::json tasks_json = nlohmann::json::parse(resp_msg.body);
                    display_tasks(tasks_json);
                } catch (const std::exception& e) {
                    logger_->error("Failed to parse task list: {}", e.what());
                }
                break;
                
            case MessageType::MARK_TASK_COMPLETED_RESPONSE:
                if (resp_msg.body == "Task marked as completed") {
                    logger_->info("Task marked as completed!");
                } else {
                    logger_->error("Failed to complete task: {}", resp_msg.body);
                }
                break;
                
            case MessageType::DELETE_TASK_RESPONSE:
                if (resp_msg.body == "Task deleted") {
                    logger_->info("Task deleted successfully!");
                } else {
                    logger_->error("Failed to delete task: {}", resp_msg.body);
                }
                break;
                
            case MessageType::ERROR:
                logger_->error("Server error: {}", resp_msg.body);
                break;
                
            default:
                logger_->warn("Received unknown message type: {}", Message::messageTypeToString(resp_msg.type));
                break;
        }
    } catch (const std::exception& e) {
        logger_->error("Failed to handle received data: {}", e.what());
    }
}

void Client::handle_input() {
    std::string input;
    
    while (socket_.is_connected()) {
        // 显示菜单
        display_menu();
        
        // 获取用户输入
        std::getline(std::cin, input);
        
        // 处理输入
        if (input.empty()) {
            continue;
        }
        
        // 转换为小写以便比较
        std::transform(input.begin(), input.end(), input.begin(), 
                      [](unsigned char c){ return std::tolower(c); });
        
        // 处理命令
        if (input == "q" || input == "quit" || input == "exit") {
            break;
        } else if (input == "c" || input == "connect") {
            if (!socket_.is_connected()) {
                socket_.connect(host_, static_cast<uint16_t>(port_), 5000);
            } else {
                logger_->info("Already connected to server");
            }
        } else if (input == "d" || input == "disconnect") {
            disconnect();
        } else if (input == "r" || input == "register") {
            handle_register();
        } else if (input == "l" || input == "login") {
            handle_login();
        } else if (input == "o" || input == "logout") {
            handle_logout();
        } else if (input == "a" || input == "add") {
            if (authenticated_) {
                handle_add_task();
            } else {
                logger_->error("You must be logged in to add tasks");
            }
        } else if (input == "t" || input == "tasks") {
            if (authenticated_) {
                handle_list_tasks();
            } else {
                logger_->error("You must be logged in to view tasks");
            }
        } else if (input == "m" || input == "mark") {
            if (authenticated_) {
                handle_complete_task();
            } else {
                logger_->error("You must be logged in to mark tasks");
            }
        } else if (input == "x" || input == "delete") {
            if (authenticated_) {
                handle_delete_task();
            } else {
                logger_->error("You must be logged in to delete tasks");
            }
        } else if (input == "h" || input == "help") {
            display_help();
        } else {
            logger_->error("Unknown command. Please try again.");
        }
        
        std::cout << "Press Enter to continue...";
        std::cin.get();
    }
}

void Client::handle_register() {
    std::string username, password, confirm_password;
    
    std::cout << "Register a new account" << std::endl;
    std::cout << "---------------------" << std::endl;
    
    std::cout << "Username: ";
    std::getline(std::cin, username);
    
    std::cout << "Password: ";
    std::getline(std::cin, password);
    
    std::cout << "Confirm Password: ";
    std::getline(std::cin, confirm_password);
    
    // 验证输入
    if (username.empty() || password.empty()) {
        logger_->error("Username and password cannot be empty");
        return;
    }
    
    if (password != confirm_password) {
        logger_->error("Passwords do not match");
        return;
    }
    
    // 创建注册消息
    Message msg;
    msg.type = MessageType::REGISTER_REQUEST;
    msg.body = username + ":" + password;
    
    // 发送消息
    if (socket_.is_connected()) {
        std::string data = msg.serialize();
        socket_.send(data);
        
        logger_->debug("Sent message: {} body: {}", 
                      Message::messageTypeToString(msg.type), 
                      msg.body);
    }
}

void Client::handle_login() {
    std::string username, password;
    
    std::cout << "Login to your account" << std::endl;
    std::cout << "--------------------" << std::endl;
    
    std::cout << "Username: ";
    std::getline(std::cin, username);
    
    std::cout << "Password: ";
    std::getline(std::cin, password);
    
    // 验证输入
    if (username.empty() || password.empty()) {
        logger_->error("Username and password cannot be empty");
        return;
    }
    
    // 创建登录消息
    Message msg;
    msg.type = MessageType::LOGIN_REQUEST;
    msg.body = username + ":" + password;
    
    // 发送消息
    if (socket_.is_connected()) {
        std::string data = msg.serialize();
        socket_.send(data);
        
        logger_->debug("Sent message: {} body: {}", 
                      Message::messageTypeToString(msg.type), 
                      msg.body);
    }
}

void Client::handle_logout() {
    // 创建登出消息
    Message msg;
    msg.type = MessageType::ERROR;  // 临时使用ERROR类型代替登出请求
    msg.body = "";
    
    // 发送消息
    if (socket_.is_connected()) {
        std::string data = msg.serialize();
        socket_.send(data);
        
        logger_->debug("Sent message: {} body: {}", 
                      Message::messageTypeToString(msg.type), 
                      msg.body);
    }
}

void Client::handle_add_task() {
    std::string description;
    
    std::cout << "Add a new task" << std::endl;
    std::cout << "---------------" << std::endl;
    
    std::cout << "Task description: ";
    std::getline(std::cin, description);
    
    // 验证输入
    if (description.empty()) {
        logger_->error("Task description cannot be empty");
        return;
    }
    
    // 创建添加任务消息
    Message msg;
    msg.type = MessageType::ADD_TASK_REQUEST;
    msg.body = description;
    
    // 发送消息
    if (socket_.is_connected()) {
        std::string data = msg.serialize();
        socket_.send(data);
        
        logger_->debug("Sent message: {} body: {}", 
                      Message::messageTypeToString(msg.type), 
                      msg.body);
    }
}

void Client::handle_list_tasks() {
    // 创建列出任务消息
    Message msg;
    msg.type = MessageType::LIST_TASKS_REQUEST;
    msg.body = "";
    
    // 发送消息
    if (socket_.is_connected()) {
        std::string data = msg.serialize();
        socket_.send(data);
        
        logger_->debug("Sent message: {} body: {}", 
                      Message::messageTypeToString(msg.type), 
                      msg.body);
    }
}

void Client::handle_complete_task() {
    std::string task_id_str;
    
    std::cout << "Mark task as completed" << std::endl;
    std::cout << "----------------------" << std::endl;
    
    std::cout << "Enter task ID: ";
    std::getline(std::cin, task_id_str);
    
    // 验证输入
    if (!std::all_of(task_id_str.begin(), task_id_str.end(), ::isdigit)) {
        logger_->error("Invalid task ID. Please enter a number.");
        return;
    }
    
    // 创建完成任务消息
    Message msg;
    msg.type = MessageType::MARK_TASK_COMPLETED_REQUEST;
    msg.body = task_id_str;
    
    // 发送消息
    if (socket_.is_connected()) {
        std::string data = msg.serialize();
        socket_.send(data);
        
        logger_->debug("Sent message: {} body: {}", 
                      Message::messageTypeToString(msg.type), 
                      msg.body);
    }
}

void Client::handle_delete_task() {
    if (!authenticated_) {
        logger_->error("You must login first!");
        return;
    }
    
    std::string task_id_str;
    std::cout << "Enter task ID to delete: ";
    std::getline(std::cin, task_id_str);
    
    // 创建删除任务消息
    Message msg;
    msg.type = MessageType::DELETE_TASK_REQUEST;
    msg.body = task_id_str;
    
    // 发送消息
    if (socket_.is_connected()) {
        std::string data = msg.serialize();
        socket_.send(data);
        
        logger_->debug("Sent message: {} body: {}", 
                      Message::messageTypeToString(msg.type), 
                      msg.body);
    }
}

void Client::display_menu() {
    int _unused = system("clear"); (void)_unused; // 清空屏幕
    
    std::cout << "===========================================" << std::endl;
    std::cout << "           TODO MANAGER CLIENT             " << std::endl;
    std::cout << "===========================================" << std::endl;
    std::cout << "Status: " << (socket_.is_connected() ? "Connected" : "Disconnected") << std::endl;
    if (socket_.is_connected()) {
        std::cout << "Authentication: " << (authenticated_ ? "Authenticated" : "Not Authenticated") << std::endl;
    }
    std::cout << "===========================================" << std::endl;
    std::cout << "Commands:                                  " << std::endl;
    std::cout << "[C] Connect                                " << std::endl;
    std::cout << "[D] Disconnect                             " << std::endl;
    std::cout << "[R] Register                               " << std::endl;
    std::cout << "[L] Login                                  " << std::endl;
    std::cout << "[O] Logout                                 " << std::endl;
    if (authenticated_) {
        std::cout << "[A] Add Task                               " << std::endl;
        std::cout << "[T] List Tasks                             " << std::endl;
        std::cout << "[M] Mark Task as Completed                 " << std::endl;
        std::cout << "[X] Delete Task                            " << std::endl;
    }
    std::cout << "[H] Help                                   " << std::endl;
    std::cout << "[Q] Quit                                   " << std::endl;
    std::cout << "===========================================" << std::endl;
    std::cout << "Enter command: ";
}

void Client::display_help() {
    int _unused = system("clear"); (void)_unused; // 清空屏幕
    
    std::cout << "===========================================" << std::endl;
    std::cout << "               HELP MENU                   " << std::endl;
    std::cout << "===========================================" << std::endl;
    std::cout << "C - Connect to the server                  " << std::endl;
    std::cout << "D - Disconnect from the server             " << std::endl;
    std::cout << "R - Register a new account                 " << std::endl;
    std::cout << "L - Login to your account                  " << std::endl;
    std::cout << "O - Logout from your account               " << std::endl;
    std::cout << "A - Add a new task                         " << std::endl;
    std::cout << "T - List all your tasks                    " << std::endl;
    std::cout << "M - Mark a task as completed               " << std::endl;
    std::cout << "X - Delete a task                          " << std::endl;
    std::cout << "H - Display this help menu                 " << std::endl;
    std::cout << "Q - Quit the application                   " << std::endl;
    std::cout << "===========================================" << std::endl;
}

void Client::display_tasks(const nlohmann::json& tasks_json) {
    int _unused = system("clear"); (void)_unused; // 清空屏幕
    
    std::cout << "===========================================" << std::endl;
    std::cout << "               YOUR TASKS                  " << std::endl;
    std::cout << "===========================================" << std::endl;
    
    if (tasks_json.empty()) {
        std::cout << "No tasks found. Add a new task to get started." << std::endl;
    } else {
        // 找出最长描述的长度，用于对齐
        size_t max_desc_length = 0;
        for (const auto& task : tasks_json) {
            if (task.contains("description")) {
                std::string desc = task["description"];
                if (desc.length() > max_desc_length) {
                    max_desc_length = desc.length();
                }
            }
        }
        
        // 确保最大长度不超过终端宽度
        if (max_desc_length > 50) {
            max_desc_length = 50;
        }
        
        // 显示任务列表
        for (const auto& task : tasks_json) {
            int id = task.value("id", 0);
            std::string desc = task.value("description", "");
            bool completed = task.value("completed", false);
            
            // 截断过长的描述
            if (desc.length() > max_desc_length) {
                desc = desc.substr(0, max_desc_length - 3) + "...";
            }
            
            // 格式化输出
            std::cout << "[" << std::setw(3) << std::right << id << "] "
                      << std::setw(max_desc_length + 2) << std::left << desc
                      << "[" << (completed ? "✓" : " ") << "]" << std::endl;
        }
    }
    
    std::cout << "===========================================" << std::endl;
}