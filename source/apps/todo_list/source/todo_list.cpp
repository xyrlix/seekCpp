/**
 * @file    : todo_list.cpp
 * @brief   : 待办事项列表实现文件
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 1970-01-01 08:00:00
 * @copyright: Copyright (c) 2025 Seek Dao
 */

#include "todo_list.h"
#include <iostream>
#include <sstream>

namespace todo_list {

    TodoListManager::TodoListManager() {
        std::cout << "TodoListManager constructor" << std::endl;
    }

    TodoListManager::~TodoListManager() {   
        std::cout << "TodoListManager destructor" << std::endl;
    }

    // 显示菜单
    void TodoListManager::show_menu() {
        // system("clear");
        std::cout << "TodoListManager show_menu" << std::endl;
        std::cout << "1. Add Task" << std::endl;
        std::cout << "2. Delete Task" << std::endl;
        std::cout << "3. Update Task" << std::endl;
        std::cout << "4. Get User Tasks" << std::endl;
        std::cout << "5. Exit" << std::endl;
        std::cout << "Enter your choice: ";
    }

    // 获取用户输入的任务信息
    void TodoListManager::get_input_task(TaskNode& task) {
        // system("clear");
        std::cout << "TodoListManager get_input_task" << std::endl;
        std::cout << "Enter task ID: ";
        std::cin >> task.id;
        std::cin.ignore(); // 忽略输入缓冲区中的换行符
        
        std::cout << "Enter username: ";
        std::getline(std::cin, task.username);
        
        std::cout << "Enter task title: ";
        std::getline(std::cin, task.title);
        
        std::cout << "Enter task description: ";
        std::getline(std::cin, task.description);
        
        std::cout << "Enter task priority (0-3, where 0=LOW, 1=MEDIUM, 2=HIGH, 3=URGENT): ";
        int priority_value;
        std::cin >> priority_value;
        task.priority = static_cast<TaskPriority>(priority_value);
        std::cin.ignore(); // 忽略输入缓冲区中的换行符
        
        std::cout << "Enter task category: ";
        std::getline(std::cin, task.category);
        
        std::cout << "Enter task tags (comma-separated): ";
        std::string tag;
        std::getline(std::cin, tag);
        std::stringstream ss(tag);
        while (std::getline(ss, tag, ',')) {
            task.tags.push_back(tag);
        }
        
        // 设置其他默认值
        task.is_completed = false;
        task.created_at = std::time(nullptr);
        task.completed_at = 0;
        
        // 简单处理due_time，实际应用中可能需要更复杂的日期解析
        task.due_time = 0;
        std::string due_time_str;
        std::cout << "Enter task due time (YYYY-MM-DD HH:MM) or leave empty for no due time: ";
        std::getline(std::cin, due_time_str);
        if (!due_time_str.empty()) {
            struct std::tm tm = {};
            strptime(due_time_str.c_str(), "%Y-%m-%d %H:%M", &tm);
            task.due_time = mktime(&tm);
        }

        print_task(task);
    }

    // 运行待办事项列表管理器
    void TodoListManager::run() {
        std::cout << "TodoListManager run" << std::endl;
        TaskNode task;
        while (true) {
            // 打印菜单
            show_menu();
            
            // 读取用户输入
            int choice;
            std::cin >> choice;
            
            // 处理用户输入
            switch (choice) {
                case 1: {
                    // 添加任务
                    get_input_task(task);
                    if (add_task(task)) {
                        print_task(task);
                    } else {
                        std::cout << "Failed to add task ID " << task.id << " for user " << task.username << std::endl;
                    }
                    break;
                }
                case 2: {
                    // 删除任务
                    print_task(task);
                    delete_task(task.id, task.username);
                    break;
                }
                case 3: {
                    // 更新任务
                    get_input_task(task);
                    print_task(task);
                    update_task(task.id, task.username, task);
                    break;
                }
                case 4: {
                    // 获取用户任务列表
                    std::vector<TaskNode> tasks = get_user_tasks(task.username);
                    for (const auto& task : tasks) {
                        print_task(task);
                    }
                    break;
                }
                case 5: {
                    // 退出程序
                    std::cout << "Exiting TodoListManager" << std::endl;
                    return;
                }
                default: {
                    std::cout << "Invalid choice. Please try again." << std::endl;
                    break;
                }
            }
        }
    }

    // 添加任务
    bool TodoListManager::add_task(const TaskNode& task) {
        std::cout << "TodoListManager add_task" << std::endl;
        // 检查任务ID是否已存在
        if (user_tasks_.find(task.username) != user_tasks_.end()) {
            for (const auto& existing_task : user_tasks_[task.username]) {
                if (existing_task.id == task.id) {
                    std::cout << "Task ID " << task.id << " already exists for user " << task.username << std::endl;
                    return false;
                }
            }
        }
        user_tasks_[task.username].push_back(task);
        std::cout << "Task ID " << task.id << " added for user " << task.username << std::endl;
        return true;
    }
    
    // 删除任务
    bool TodoListManager::delete_task(int task_id, const std::string& username) {
        std::cout << "TodoListManager delete_task" << std::endl;
        // 检查任务ID是否存在
        if (user_tasks_.find(username) == user_tasks_.end()) {
            std::cout << "User " << username << " has no tasks" << std::endl;
            return false;
        }
        for (auto it = user_tasks_[username].begin(); it != user_tasks_[username].end(); ++it) {
            if (it->id == task_id) {
                user_tasks_[username].erase(it);
                std::cout << "Task ID " << task_id << " deleted for user " << username << std::endl;
                return true;
            }
        }
        std::cout << "Task ID " << task_id << " not found for user " << username << std::endl;
        return false;
    }

    // 更新任务
    bool TodoListManager::update_task(int task_id, const std::string& username, const TaskNode& updated_task) {
        std::cout << "TodoListManager update_task" << std::endl;        
        // 检查任务ID是否存在
        if (user_tasks_.find(username) == user_tasks_.end()) {
            std::cout << "User " << username << " has no tasks" << std::endl;
            return false;
        }
        for (auto& task : user_tasks_[username]) {
            if (task.id == task_id) {
                task = updated_task;
                std::cout << "Task ID " << task_id << " updated for user " << username << std::endl;
                return true;
            }
        }
        std::cout << "Task ID " << task_id << " not found for user " << username << std::endl;
        return false;
    }

    // 获取用户任务列表
    std::vector<TaskNode> TodoListManager::get_user_tasks(const std::string& username) {
        std::cout << "TodoListManager get_user_tasks" << std::endl;
        // 检查用户是否存在任务
        if (user_tasks_.find(username) == user_tasks_.end()) {
            std::cout << "User " << username << " has no tasks" << std::endl;
            return {};
        }
        return user_tasks_[username];
    }

    // 打印任务
    void TodoListManager::print_task(const TaskNode& task) {
        std::cout << "TodoListManager print_task" << std::endl;
        std::cout << "Task ID: " << task.id << std::endl;
        std::cout << "Username: " << task.username << std::endl;
        std::cout << "Title: " << task.title << std::endl;
        std::cout << "Description: " << task.description << std::endl;
        std::cout << "Status: " << (task.is_completed ? "Completed" : "Pending") << std::endl;
        
        // 打印优先级
        std::cout << "Priority: ";
        switch (task.priority) {
            case TaskPriority::LOW:
                std::cout << "LOW";
                break;
            case TaskPriority::MEDIUM:
                std::cout << "MEDIUM";
                break;
            case TaskPriority::HIGH:
                std::cout << "HIGH";
                break;
            case TaskPriority::URGENT:
                std::cout << "URGENT";
                break;
        }
        std::cout << std::endl;
        
        std::cout << "Category: " << task.category << std::endl;
        
        // 打印标签
        std::cout << "Tags: ";
        for (size_t i = 0; i < task.tags.size(); ++i) {
            std::cout << task.tags[i];
            if (i < task.tags.size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << std::endl;
        
        // 打印创建时间
        std::cout << "Created at: " << std::asctime(std::localtime(&task.created_at));
        
        // 打印截止时间
        if (task.due_time > 0) {
            std::cout << "Due time: " << std::asctime(std::localtime(&task.due_time));
        } else {
            std::cout << "Due time: None" << std::endl;
        }
        
        // 打印完成时间
        if (task.is_completed && task.completed_at > 0) {
            std::cout << "Completed at: " << std::asctime(std::localtime(&task.completed_at));
        }
        
        std::cout << "------------------------------" << std::endl;
    }
} // namespace todo_list