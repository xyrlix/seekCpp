/**
 * @file    : database.h
 * @brief   : 定义数据库管理类
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 2025-10-05 04:30:32
 * @copyright: Copyright (c) 2025 Seek Dao
 */

#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>
#include <vector>
#include <mutex>
#include <functional>
#include "task.h"
#include "user.h"

class Database {
public:
    Database(const std::string& db_path);
    ~Database();
    
    // 初始化数据库
    bool initialize();
    
    // 创建数据库表结构
    bool create_database_tables();
    
    // 加载所有用户
    bool load_all_users(std::unordered_map<std::string, User>& users);
    
    // 用户相关操作
    bool addUser(const User& user);
    bool getUser(const std::string& username, User& user);
    bool updateUserLastLogin(const std::string& username);
    bool userExists(const std::string& username);
    
    // 任务相关操作
    int addTask(const Task& task);
    bool getTasks(const std::string& username, std::vector<Task>& tasks);
    bool getTask(int task_id, const std::string& username, Task& task);
    bool updateTask(const Task& task);
    bool deleteTask(int task_id, const std::string& username);
    bool markTaskCompleted(int task_id, const std::string& username);
    
    // 任务搜索和过滤
    bool searchTasks(const std::string& username, const std::string& keyword,
                    std::vector<Task>& tasks);
    bool filterTasks(const std::string& username, bool completed_only,
                    TaskPriority priority, const std::string& category,
                    std::vector<Task>& tasks);
    
    // 任务排序
    enum class SortField {
        ID,
        CREATED_AT,
        DUE_TIME,
        PRIORITY,
        TITLE
    };
    
    enum class SortOrder {
        ASCENDING,
        DESCENDING
    };
    
    bool sortTasks(std::vector<Task>& tasks, SortField field, SortOrder order);
    
private:
    // 执行SQL语句（不返回结果）
    bool execute(const std::string& sql);
    
    // 执行SQL查询（返回结果）
    bool query(const std::string& sql, std::function<bool(sqlite3_stmt*)> callback);
    
    // 数据库连接
    sqlite3* db_;
    std::string db_path_;
    std::mutex db_mutex_; // 用于线程同步
};

#endif // DATABASE_H