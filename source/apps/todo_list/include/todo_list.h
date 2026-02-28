/**
 * @file    : todo_list.h
 * @brief   : 待办事项列表头文件
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 1970-01-01 08:00:00
 * @copyright: Copyright (c) 2025 Seek Dao
 */

#ifndef TODO_LIST_H
#define TODO_LIST_H

#include <string>
#include <vector>
#include <unordered_map>
#include <ctime>

namespace todo_list {

// 用户节点结构体
struct UserNode {
    std::string username;
    std::string password_hash; // 存储密码的哈希值，而不是明文
    std::string email;         // 用户邮箱
    std::string phone_number;  // 手机号码
    bool email_verified;       // 邮箱是否已验证
    bool phone_verified;       // 手机号是否已验证
    std::time_t created_at;    // 创建时间
    std::time_t last_login;    // 最后登录时间
    std::string avatar_path;   // 头像路径
};

// 任务优先级枚举
enum class TaskPriority {
    LOW = 0,
    MEDIUM = 1,
    HIGH = 2,
    URGENT = 3
};

// 任务节点结构体
struct TaskNode {
    int id;
    std::string username; // 所属用户
    std::string title;
    std::string description;
    bool is_completed;
    std::time_t created_at;
    std::time_t due_time; // 截止时间，用于定时提醒
    TaskPriority priority; // 任务优先级
    std::string category; // 任务分类
    std::vector<std::string> tags; // 任务标签
    std::time_t completed_at; // 完成时间
};

class TodoListManager {
public:
    TodoListManager();
    ~TodoListManager();

    // 显示菜单
    void show_menu();

    // 运行待办事项列表管理器
    void run();

    // 获取用户输入的任务信息
    void get_input_task(TaskNode& task);

    // 打印任务
    void print_task(const TaskNode& task);

    // 添加任务
    bool add_task(const TaskNode& task);

    // 删除任务
    bool delete_task(int task_id, const std::string& username);

    // 更新任务
    bool update_task(int task_id, const std::string& username, const TaskNode& updated_task);

    // 获取用户任务列表
    std::vector<TaskNode> get_user_tasks(const std::string& username);

private:
    std::unordered_map<std::string, std::vector<TaskNode>> user_tasks_; // 用户任务映射
    UserNode current_user_; // 当前登录用户
};

} // namespace todo_list

#endif // TODO_LIST_H
