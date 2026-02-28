#include "server.h"
#include "message.h"
#include "task.h"
#include "user.h"
#include "database.h"
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <random>
#include <cstring>
#include <sstream>
#include <functional>
#include <cerrno>
#include <cstdlib>

// 辅助函数：分割字符串
std::vector<std::string> split_string(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// 辅助函数：哈希密码
std::string hash_password(const std::string& password) {
    // 使用简单的哈希算法（在实际应用中应使用更安全的算法）
    std::hash<std::string> hasher;
    return std::to_string(hasher(password));
}

Server::Server(const std::string& port)
    : port_(port), running_(false), database_("todo.db") {
    // 初始化套接字
    if ((server_socket_ = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    // 设置套接字选项以允许地址重用
    int opt = 1;
    if (setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                   &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
    // 绑定套接字到地址和端口
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(std::stoi(port_));
    
    if (bind(server_socket_, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    // 初始化数据库
    initialize_database();
}

Server::~Server() {
    stop();
}

void Server::start() {
    // 开始监听连接请求
    if (listen(server_socket_, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    std::cout << "Server listening on port " << port_ << std::endl;
    
    // 启动服务器主循环
    running_ = true;
    accept_thread_ = std::thread(&Server::accept_connections, this);
}

void Server::stop() {
    running_ = false;
    
    // 关闭所有客户端会话
    std::lock_guard<std::mutex> lock(sessions_mutex_);
    for (auto& session : sessions_) {
        session->running = false;
    }
    
    // 等待会话线程结束
    for (auto& session : sessions_) {
        if (session->session_thread.joinable()) {
            session->session_thread.join();
        }
    }
    
    // 关闭服务器套接字
    close(server_socket_);
    
    // 等待主循环线程结束
    if (accept_thread_.joinable()) {
        accept_thread_.join();
    }
    
    std::cout << "Server stopped" << std::endl;
}

void Server::initialize_database() {
    // 初始化数据库（这里简单实现，实际应该使用Database类）
    if (!database_.initialize()) {
        std::cerr << "Failed to initialize database" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    // 加载所有用户
    if (!database_.load_all_users(users_)) {
        std::cerr << "Failed to load users from database" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    std::cout << "Server initialized with " << users_.size() << " users" << std::endl;
}

void Server::accept_connections() {
    while (running_) {
        // 接受新的连接
        struct sockaddr_in address;
        int addrlen = sizeof(address);
        
        // 使用poll设置超时，避免死锁在accept上
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(server_socket_, &read_fds);
        
        struct timeval timeout;
        timeout.tv_sec = 1;  // 1秒超时
        timeout.tv_usec = 0;
        
        int activity = select(server_socket_ + 1, &read_fds, NULL, NULL, &timeout);
        
        if (activity < 0 && (errno != EINTR)) {
            perror("select error");
            continue;
        }
        
        // 检查是否有新的连接
        if (activity > 0 && FD_ISSET(server_socket_, &read_fds)) {
            int new_socket = accept(server_socket_, (struct sockaddr *)&address, 
                                   (socklen_t*)&addrlen);
            
            if (new_socket < 0) {
                perror("accept");
                continue;
            }
            
            // 为新连接创建一个会话
            auto new_session = std::make_shared<ClientSession>();
            new_session->socket = new_socket;
            new_session->running = true;
            
            // 启动会话线程
            new_session->session_thread = std::thread(&Server::handle_client, this, new_session);
            
            // 将新会话添加到会话列表
            { 
                std::lock_guard<std::mutex> lock(sessions_mutex_);
                sessions_.push_back(new_session);
            }
            
            std::cout << "New connection established" << std::endl;
        }
    }
}

Message Server::handle_message(const Message& msg, const std::shared_ptr<ClientSession>& session) {
    Message response;
    response.type = MessageType::ERROR;
    response.body = "Unknown command";
    
    try {
        switch (msg.type) {
            case MessageType::LOGIN_REQUEST:
                {
                    // 解析登录信息（格式：username:password）
                    std::vector<std::string> parts = split_string(msg.body, ':');
                    
                    if (parts.size() != 2) {
                        response.body = "Invalid login format. Use username:password";
                        break;
                    }
                    
                    std::string username = parts[0];
                    std::string password = parts[1];
                    
                    // 检查用户是否存在
                    auto it = users_.find(username);
                    if (it == users_.end()) {
                        response.body = "User not found";
                        break;
                    }
                    
                    // 检查密码是否正确
                    if (it->second.password_hash != hash_password(password)) {
                        response.body = "Invalid password";
                        break;
                    }
                    
                    // 登录成功
                    session->username = username;
                    response.type = MessageType::LOGIN_RESPONSE;
                    response.body = "Login successful";
                    
                    std::cout << "User " << username << " logged in" << std::endl;
                    
                    // 加载用户的任务列表
                    std::vector<Task> tasks;
                    if (database_.getTasks(username, tasks)) {
                        user_tasks_[username] = tasks;
                    } else {
                        std::cerr << "Failed to load tasks for user " << username << std::endl;
                        user_tasks_[username].clear();
                    }
                    
                    break;
                }
                
            case MessageType::REGISTER_REQUEST:
                {
                    // 解析注册信息（格式：username:password）
                    std::vector<std::string> parts = split_string(msg.body, ':');
                    
                    if (parts.size() != 2) {
                        response.body = "Invalid register format. Use username:password";
                        break;
                    }
                    
                    std::string username = parts[0];
                    std::string password = parts[1];
                    
                    // 检查用户名是否已存在
                    if (users_.find(username) != users_.end()) {
                        response.body = "Username already exists";
                        break;
                    }
                    
                    // 创建新用户
                    User new_user;
                    new_user.username = username;
                    new_user.password_hash = hash_password(password);
                    
                    // 将用户添加到数据库
                    if (!database_.addUser(new_user)) {
                        response.body = "Failed to register user";
                        break;
                    }
                    
                    // 添加到内存中的用户列表
                    users_[username] = new_user;
                    
                    // 为新用户初始化空的任务列表
                    user_tasks_[username] = std::vector<Task>();
                    
                    // 注册成功
                    response.type = MessageType::REGISTER_RESPONSE;
                    response.body = "Registration successful";
                    
                    std::cout << "New user registered: " << username << std::endl;
                    
                    break;
                }
                
            case MessageType::ADD_TASK_REQUEST:
                {
                    // 检查用户是否已登录
                    if (session->username.empty()) {
                        response.body = "You must be logged in to add tasks";
                        break;
                    }
                    
                    // 创建新任务
                    Task new_task;
                    new_task.id = next_task_id_++;
                    new_task.description = msg.body;
                    new_task.is_completed = false;
                    
                    // 添加到数据库
                    int task_id = database_.addTask(new_task);
                    if (task_id == -1) {
                        response.body = "Failed to save task";
                        break;
                    }
                    
                    new_task.id = task_id;
                    
                    // 添加到用户的任务列表
                    user_tasks_[session->username].push_back(new_task);
                    
                    // 添加成功
                    response.type = MessageType::ADD_TASK_RESPONSE;
                    response.body = "Task added successfully";
                    
                    std::cout << "Task added for user " << session->username << ": " << new_task.description << std::endl;
                    
                    break;
                }
                
            case MessageType::LIST_TASKS_REQUEST:
                {
                    // 检查用户是否已登录
                    if (session->username.empty()) {
                        response.body = "You must be logged in to list tasks";
                        break;
                    }
                    
                    // 构建任务列表响应
                    std::string tasks_json = "[";
                    const auto& tasks = user_tasks_[session->username];
                    for (size_t i = 0; i < tasks.size(); ++i) {
                        tasks_json += "{\"id\": " + std::to_string(tasks[i].id) + ", \"description\": \"" + 
                                      tasks[i].description + "\", \"completed\": " + 
                                      (tasks[i].is_completed ? "true" : "false") + "}";
                        if (i < tasks.size() - 1) {
                            tasks_json += ",";
                        }
                    }
                    tasks_json += "]";
                    
                    response.type = MessageType::LIST_TASKS_RESPONSE;
                    response.body = tasks_json;
                    
                    break;
                }
                
            case MessageType::MARK_TASK_COMPLETED_REQUEST:
                {
                    // 检查用户是否已登录
                    if (session->username.empty()) {
                        response.body = "You must be logged in to complete tasks";
                        break;
                    }
                    
                    // 解析任务ID
                    int task_id = std::stoi(msg.body);
                    
                    // 查找并标记任务为已完成
                    bool task_found = false;
                    auto& tasks = user_tasks_[session->username];
                    for (auto& task : tasks) {
                        if (task.id == task_id) {
                            task.is_completed = true;
                            task_found = true;
                            
                            // 更新数据库
                            if (!database_.markTaskCompleted(task_id, session->username)) {
                                response.body = "Failed to update task";
                                task.is_completed = false; // 回滚
                                break;
                            }
                            
                            break;
                        }
                    }
                    
                    if (!task_found) {
                        response.body = "Task not found";
                        break;
                    }
                    
                    response.type = MessageType::MARK_TASK_COMPLETED_RESPONSE;
                    response.body = "Task marked as completed";
                    
                    std::cout << "Task " << task_id << " marked as completed by user " << session->username << std::endl;
                    
                    break;
                }
                
            case MessageType::DELETE_TASK_REQUEST:
                {
                    // 检查用户是否已登录
                    if (session->username.empty()) {
                        response.body = "You must be logged in to delete tasks";
                        break;
                    }
                    
                    // 解析任务ID
                    int task_id = std::stoi(msg.body);
                    
                    // 查找并删除任务
                    auto& tasks = user_tasks_[session->username];
                    auto it = std::remove_if(tasks.begin(), tasks.end(), 
                        [task_id](const Task& task) { return task.id == task_id; });
                    
                    if (it == tasks.end()) {
                        response.body = "Task not found";
                        break;
                    }
                    
                    // 从数据库删除
                    if (!database_.deleteTask(task_id, session->username)) {
                        response.body = "Failed to delete task";
                        break;
                    }
                    
                    tasks.erase(it, tasks.end());
                    
                    response.type = MessageType::DELETE_TASK_RESPONSE;
                    response.body = "Task deleted";
                    
                    std::cout << "Task " << task_id << " deleted by user " << session->username << std::endl;
                    
                    break;
                }
                
            default:
                response.body = "Unsupported message type";
                break;
        }
    } catch (const std::exception& e) {
        response.body = std::string("Error processing request: ") + e.what();
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    return response;
}

void Server::handle_client(std::shared_ptr<ClientSession> session) {
    try {
        while (session->running) {
            // 使用poll设置超时，避免死锁在recv上
            fd_set read_fds;
            FD_ZERO(&read_fds);
            FD_SET(session->socket, &read_fds);
            
            struct timeval timeout;
            timeout.tv_sec = 1;  // 1秒超时
            timeout.tv_usec = 0;
            
            int activity = select(session->socket + 1, &read_fds, NULL, NULL, &timeout);
            
            if (activity < 0 && (errno != EINTR)) {
                perror("select error");
                break;
            }
            
            // 检查是否有数据可读
            if (activity > 0 && FD_ISSET(session->socket, &read_fds)) {
                // 接收数据
                char buffer[1024] = {0};
                int valread = read(session->socket, buffer, 1024);
                
                if (valread <= 0) {
                    // 连接关闭
                    break;
                }
                
                // 处理接收到的数据
                std::string data(buffer, valread);
                
                // 解析消息
                Message msg = Message::deserialize(data);
                
                // 处理消息
                Message response = handle_message(msg, session);
                
                // 发送响应
                std::string response_data = response.serialize();
                send(session->socket, response_data.c_str(), response_data.size(), 0);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error handling client: " << e.what() << std::endl;
    }
    
    // 清理会话资源
    close(session->socket);
    
    // 从会话列表中移除
    std::lock_guard<std::mutex> lock(sessions_mutex_);
    auto it = std::remove(sessions_.begin(), sessions_.end(), session);
    if (it != sessions_.end()) {
        sessions_.erase(it);
        std::cout << "Client disconnected" << std::endl;
    }
}