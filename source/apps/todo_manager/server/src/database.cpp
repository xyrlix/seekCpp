/**
 * @file    : database.cpp
 * @brief   : 数据库管理类实现
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 2025-10-05 04:30:32
 * @copyright: Copyright (c) 2025 Seek Dao
 */

#include "../include/database.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cstring>
#include <algorithm>
#include <functional>
#include <unordered_map>

Database::Database(const std::string& db_path) : db_(nullptr), db_path_(db_path) {
}

Database::~Database() {
    if (db_ != nullptr) {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}

bool Database::initialize() {
    std::lock_guard<std::mutex> lock(db_mutex_);
    
    // 打开数据库连接
    int rc = sqlite3_open(db_path_.c_str(), &db_);
    if (rc != SQLITE_OK) {
        std::cerr << "无法打开数据库: " << sqlite3_errmsg(db_) << std::endl;
        sqlite3_close(db_);
        db_ = nullptr;
        return false;
    }
    
    // 创建用户表
    const char* create_users_table = 
        "CREATE TABLE IF NOT EXISTS users (" \
        "username TEXT PRIMARY KEY, " \
        "password_hash TEXT NOT NULL, " \
        "email TEXT, " \
        "phone_number TEXT, " \
        "email_verified INTEGER DEFAULT 0, " \
        "phone_verified INTEGER DEFAULT 0, " \
        "created_at INTEGER NOT NULL, " \
        "last_login INTEGER NOT NULL, " \
        "avatar_path TEXT" \
        ");";
    
    if (!execute(create_users_table)) {
        return false;
    }
    
    // 创建任务表
    const char* create_tasks_table = 
        "CREATE TABLE IF NOT EXISTS tasks (" \
        "id INTEGER PRIMARY KEY AUTOINCREMENT, " \
        "username TEXT NOT NULL, " \
        "title TEXT NOT NULL, " \
        "description TEXT, " \
        "is_completed INTEGER DEFAULT 0, " \
        "created_at INTEGER NOT NULL, " \
        "due_time INTEGER DEFAULT 0, " \
        "priority INTEGER DEFAULT 0, " \
        "category TEXT DEFAULT '', " \
        "tags TEXT DEFAULT '', " \
        "completed_at INTEGER DEFAULT 0, " \
        "FOREIGN KEY(username) REFERENCES users(username)" \
        ");";
    
    if (!execute(create_tasks_table)) {
        return false;
    }
    
    std::cout << "数据库初始化成功" << std::endl;
    return true;
}

bool Database::addUser(const User& user) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    
    const char* sql = 
        "INSERT INTO users (username, password_hash, email, phone_number, " \
        "email_verified, phone_verified, created_at, last_login, avatar_path) " \
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "预处理SQL语句失败: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, user.username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, user.password_hash.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, user.email.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, user.phone_number.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 5, user.email_verified ? 1 : 0);
    sqlite3_bind_int(stmt, 6, user.phone_verified ? 1 : 0);
    sqlite3_bind_int64(stmt, 7, static_cast<sqlite3_int64>(user.created_at));
    sqlite3_bind_int64(stmt, 8, static_cast<sqlite3_int64>(user.last_login));
    sqlite3_bind_text(stmt, 9, user.avatar_path.c_str(), -1, SQLITE_TRANSIENT);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "添加用户失败: " << sqlite3_errmsg(db_) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    
    sqlite3_finalize(stmt);
    return true;
}

bool Database::getUser(const std::string& username, User& user) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    
    const char* sql = "SELECT * FROM users WHERE username = ?;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        std::cerr << "预处理SQL语句失败: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return false; // 用户不存在
    }
    
    user.username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    user.password_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    user.email = sqlite3_column_text(stmt, 2) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)) : "";
    user.phone_number = sqlite3_column_text(stmt, 3) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)) : "";
    user.email_verified = sqlite3_column_int(stmt, 4) != 0;
    user.phone_verified = sqlite3_column_int(stmt, 5) != 0;
    user.created_at = static_cast<std::time_t>(sqlite3_column_int64(stmt, 6));
    user.last_login = static_cast<std::time_t>(sqlite3_column_int64(stmt, 7));
    user.avatar_path = sqlite3_column_text(stmt, 8) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8)) : "";
    
    sqlite3_finalize(stmt);
    return true;
}

bool Database::updateUserLastLogin(const std::string& username) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    
    const char* sql = "UPDATE users SET last_login = ? WHERE username = ?;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        std::cerr << "预处理SQL语句失败: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_int64(stmt, 1, static_cast<sqlite3_int64>(std::time(nullptr)));
    sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_TRANSIENT);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "更新用户最后登录时间失败: " << sqlite3_errmsg(db_) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    
    sqlite3_finalize(stmt);
    return true;
}

bool Database::userExists(const std::string& username) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    
    const char* sql = "SELECT COUNT(*) FROM users WHERE username = ?;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        std::cerr << "预处理SQL语句失败: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return false;
    }
    
    int count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    
    return count > 0;
}

int Database::addTask(const Task& task) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    
    // 构建标签字符串
    std::string tags_str;
    for (size_t i = 0; i < task.tags.size(); ++i) {
        tags_str += task.tags[i];
        if (i < task.tags.size() - 1) {
            tags_str += ",";
        }
    }
    
    // 使用SQLite的AUTOINCREMENT特性自动生成ID
    const char* sql = 
        "INSERT INTO tasks (username, title, description, is_completed, " \
        "created_at, due_time, priority, category, tags, completed_at) " \
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "预处理SQL语句失败: " << sqlite3_errmsg(db_) << std::endl;
        return -1;
    }
    
    sqlite3_bind_text(stmt, 1, task.username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, task.title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, task.description.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, task.is_completed ? 1 : 0);
    sqlite3_bind_int64(stmt, 5, static_cast<sqlite3_int64>(task.created_at));
    sqlite3_bind_int64(stmt, 6, static_cast<sqlite3_int64>(task.due_time));
    sqlite3_bind_int(stmt, 7, static_cast<int>(task.priority));
    sqlite3_bind_text(stmt, 8, task.category.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 9, tags_str.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 10, static_cast<sqlite3_int64>(task.completed_at));
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "添加任务失败: " << sqlite3_errmsg(db_) << std::endl;
        sqlite3_finalize(stmt);
        return -1;
    }
    
    // 获取最后插入的任务ID
    int task_id = static_cast<int>(sqlite3_last_insert_rowid(db_));
    sqlite3_finalize(stmt);
    return task_id;
}

bool Database::getTasks(const std::string& username, std::vector<Task>& tasks) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    
    const char* sql = "SELECT * FROM tasks WHERE username = ?;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        std::cerr << "预处理SQL语句失败: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        Task task;
        task.id = sqlite3_column_int(stmt, 0);
        task.username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        task.title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        task.description = sqlite3_column_text(stmt, 3) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)) : "";
        task.is_completed = sqlite3_column_int(stmt, 4) != 0;
        task.created_at = static_cast<std::time_t>(sqlite3_column_int64(stmt, 5));
        task.due_time = static_cast<std::time_t>(sqlite3_column_int64(stmt, 6));
        task.priority = static_cast<TaskPriority>(sqlite3_column_int(stmt, 7));
        task.category = sqlite3_column_text(stmt, 8) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8)) : "";
        
        // 解析标签
        const char* tags_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
        if (tags_str && strlen(tags_str) > 0) {
            std::stringstream ss(tags_str);
            std::string tag;
            while (std::getline(ss, tag, ',')) {
                if (!tag.empty()) {
                    task.tags.push_back(tag);
                }
            }
        }
        
        task.completed_at = static_cast<std::time_t>(sqlite3_column_int64(stmt, 10));
        
        tasks.push_back(task);
    }
    
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        std::cerr << "获取任务列表失败: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    return true;
}

bool Database::getTask(int task_id, const std::string& username, Task& task) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    
    const char* sql = "SELECT * FROM tasks WHERE id = ? AND username = ?;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        std::cerr << "预处理SQL语句失败: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, task_id);
    sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_TRANSIENT);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return false; // 任务不存在
    }
    
    task.id = sqlite3_column_int(stmt, 0);
    task.username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    task.title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    task.description = sqlite3_column_text(stmt, 3) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)) : "";
    task.is_completed = sqlite3_column_int(stmt, 4) != 0;
    task.created_at = static_cast<std::time_t>(sqlite3_column_int64(stmt, 5));
    task.due_time = static_cast<std::time_t>(sqlite3_column_int64(stmt, 6));
    task.priority = static_cast<TaskPriority>(sqlite3_column_int(stmt, 7));
    task.category = sqlite3_column_text(stmt, 8) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8)) : "";
    
    // 解析标签
    const char* tags_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
    if (tags_str && strlen(tags_str) > 0) {
        std::stringstream ss(tags_str);
        std::string tag;
        while (std::getline(ss, tag, ',')) {
            if (!tag.empty()) {
                task.tags.push_back(tag);
            }
        }
    }
    
    task.completed_at = static_cast<std::time_t>(sqlite3_column_int64(stmt, 10));
    
    sqlite3_finalize(stmt);
    return true;
}

bool Database::updateTask(const Task& task) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    
    // 构建标签字符串
    std::string tags_str;
    for (size_t i = 0; i < task.tags.size(); ++i) {
        tags_str += task.tags[i];
        if (i < task.tags.size() - 1) {
            tags_str += ",";
        }
    }
    
    const char* sql = 
        "UPDATE tasks SET title = ?, description = ?, is_completed = ?, " \
        "due_time = ?, priority = ?, category = ?, tags = ?, completed_at = ? " \
        "WHERE id = ? AND username = ?;";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "预处理SQL语句失败: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, task.title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, task.description.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, task.is_completed ? 1 : 0);
    sqlite3_bind_int64(stmt, 4, static_cast<sqlite3_int64>(task.due_time));
    sqlite3_bind_int(stmt, 5, static_cast<int>(task.priority));
    sqlite3_bind_text(stmt, 6, task.category.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, tags_str.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 8, static_cast<sqlite3_int64>(task.completed_at));
    sqlite3_bind_int(stmt, 9, task.id);
    sqlite3_bind_text(stmt, 10, task.username.c_str(), -1, SQLITE_TRANSIENT);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "更新任务失败: " << sqlite3_errmsg(db_) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    
    sqlite3_finalize(stmt);
    return true;
}

bool Database::deleteTask(int task_id, const std::string& username) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    
    const char* sql = "DELETE FROM tasks WHERE id = ? AND username = ?;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "预处理SQL语句失败: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, task_id);
    sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_TRANSIENT);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "删除任务失败: " << sqlite3_errmsg(db_) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    
    sqlite3_finalize(stmt);
    return true;
}

bool Database::markTaskCompleted(int task_id, const std::string& username) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    
    const char* sql = 
        "UPDATE tasks SET is_completed = 1, completed_at = ? " \
        "WHERE id = ? AND username = ?;";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        std::cerr << "预处理SQL语句失败: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    sqlite3_bind_int64(stmt, 1, static_cast<sqlite3_int64>(std::time(nullptr)));
    sqlite3_bind_int(stmt, 2, task_id);
    sqlite3_bind_text(stmt, 3, username.c_str(), -1, SQLITE_TRANSIENT);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "标记任务完成失败: " << sqlite3_errmsg(db_) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    
    sqlite3_finalize(stmt);
    return true;
}

bool Database::searchTasks(const std::string& username, const std::string& keyword, 
                          std::vector<Task>& tasks) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    
    std::string sql = "SELECT * FROM tasks WHERE username = ? AND (title LIKE ? OR description LIKE ?);";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        std::cerr << "预处理SQL语句失败: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    std::string search_pattern = "%" + keyword + "%";
    
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, search_pattern.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, search_pattern.c_str(), -1, SQLITE_TRANSIENT);
    
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        Task task;
        task.id = sqlite3_column_int(stmt, 0);
        task.username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        task.title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        task.description = sqlite3_column_text(stmt, 3) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)) : "";
        task.is_completed = sqlite3_column_int(stmt, 4) != 0;
        task.created_at = static_cast<std::time_t>(sqlite3_column_int64(stmt, 5));
        task.due_time = static_cast<std::time_t>(sqlite3_column_int64(stmt, 6));
        task.priority = static_cast<TaskPriority>(sqlite3_column_int(stmt, 7));
        task.category = sqlite3_column_text(stmt, 8) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8)) : "";
        
        // 解析标签
        const char* tags_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
        if (tags_str && strlen(tags_str) > 0) {
            std::stringstream ss(tags_str);
            std::string tag;
            while (std::getline(ss, tag, ',')) {
                if (!tag.empty()) {
                    task.tags.push_back(tag);
                }
            }
        }
        
        task.completed_at = static_cast<std::time_t>(sqlite3_column_int64(stmt, 10));
        
        tasks.push_back(task);
    }
    
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        std::cerr << "搜索任务失败: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    return true;
}

bool Database::filterTasks(const std::string& username, bool completed_only, 
                          TaskPriority priority, const std::string& category, 
                          std::vector<Task>& tasks) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    
    std::stringstream sql_ss;
    sql_ss << "SELECT * FROM tasks WHERE username = ?";
    
    if (completed_only) {
        sql_ss << " AND is_completed = 1";
    }
    
    if (priority != TaskPriority::LOW) {
        sql_ss << " AND priority >= " << static_cast<int>(priority);
    }
    
    if (!category.empty()) {
        sql_ss << " AND category = ?";
    }
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql_ss.str().c_str(), -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        std::cerr << "预处理SQL语句失败: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    int param_index = 1;
    sqlite3_bind_text(stmt, param_index++, username.c_str(), -1, SQLITE_TRANSIENT);
    
    if (!category.empty()) {
        sqlite3_bind_text(stmt, param_index++, category.c_str(), -1, SQLITE_TRANSIENT);
    }
    
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        Task task;
        task.id = sqlite3_column_int(stmt, 0);
        task.username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        task.title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        task.description = sqlite3_column_text(stmt, 3) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)) : "";
        task.is_completed = sqlite3_column_int(stmt, 4) != 0;
        task.created_at = static_cast<std::time_t>(sqlite3_column_int64(stmt, 5));
        task.due_time = static_cast<std::time_t>(sqlite3_column_int64(stmt, 6));
        task.priority = static_cast<TaskPriority>(sqlite3_column_int(stmt, 7));
        task.category = sqlite3_column_text(stmt, 8) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8)) : "";
        
        // 解析标签
        const char* tags_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
        if (tags_str && strlen(tags_str) > 0) {
            std::stringstream ss(tags_str);
            std::string tag;
            while (std::getline(ss, tag, ',')) {
                if (!tag.empty()) {
                    task.tags.push_back(tag);
                }
            }
        }
        
        task.completed_at = static_cast<std::time_t>(sqlite3_column_int64(stmt, 10));
        
        tasks.push_back(task);
    }
    
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        std::cerr << "过滤任务失败: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    return true;
}

bool Database::sortTasks(std::vector<Task>& tasks, SortField field, SortOrder order) {
    try {
        auto comparator = [field, order](const Task& a, const Task& b) {
            bool result = false;
            
            switch (field) {
                case SortField::ID:
                    result = a.id < b.id;
                    break;
                case SortField::CREATED_AT:
                    result = a.created_at < b.created_at;
                    break;
                case SortField::DUE_TIME:
                    result = a.due_time < b.due_time;
                    break;
                case SortField::PRIORITY:
                    result = static_cast<int>(a.priority) < static_cast<int>(b.priority);
                    break;
                case SortField::TITLE:
                    result = a.title < b.title;
                    break;
            }
            
            return order == SortOrder::ASCENDING ? result : !result;
        };
        
        std::sort(tasks.begin(), tasks.end(), comparator);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "排序任务失败: " << e.what() << std::endl;
        return false;
    }
}


bool Database::execute(const std::string& sql) {
    char* err_msg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err_msg);
    
    if (rc != SQLITE_OK) {
        std::cerr << "SQL错误: " << err_msg << std::endl;
        sqlite3_free(err_msg);
        return false;
    }
    
    return true;
}

bool Database::query(const std::string& sql, std::function<bool(sqlite3_stmt*)> callback) {
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        std::cerr << "预处理SQL语句失败: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    bool result = true;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        if (!callback(stmt)) {
            result = false;
            break;
        }
    }
    
    if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
        std::cerr << "执行SQL查询失败: " << sqlite3_errmsg(db_) << std::endl;
        result = false;
    }
    
    sqlite3_finalize(stmt);
    return result;
}

// 创建数据库表结构
bool Database::create_database_tables() {
    std::lock_guard<std::mutex> lock(db_mutex_);
    
    // 检查数据库是否已打开
    if (db_ == nullptr) {
        std::cerr << "数据库未打开" << std::endl;
        return false;
    }
    
    // 创建users表
    const char* create_users_table = 
        "CREATE TABLE IF NOT EXISTS users (" 
        "username TEXT PRIMARY KEY, " 
        "password_hash TEXT NOT NULL, " 
        "email TEXT, " 
        "phone_number TEXT, " 
        "email_verified INTEGER DEFAULT 0, " 
        "phone_verified INTEGER DEFAULT 0, " 
        "created_at INTEGER DEFAULT (CAST(strftime('%s', 'now') AS INTEGER)), " 
        "last_login INTEGER DEFAULT (CAST(strftime('%s', 'now') AS INTEGER)), " 
        "avatar_path TEXT" 
        ");";
    
    if (!execute(create_users_table)) {
        std::cerr << "创建users表失败" << std::endl;
        return false;
    }
    
    // 创建tasks表
    const char* create_tasks_table = 
        "CREATE TABLE IF NOT EXISTS tasks (" 
        "id INTEGER PRIMARY KEY AUTOINCREMENT, " 
        "username TEXT NOT NULL, " 
        "title TEXT NOT NULL, " 
        "description TEXT, " 
        "is_completed INTEGER DEFAULT 0, " 
        "created_at INTEGER DEFAULT (CAST(strftime('%s', 'now') AS INTEGER)), " 
        "due_time INTEGER DEFAULT 0, " 
        "priority INTEGER DEFAULT 0, " 
        "category TEXT DEFAULT '', " 
        "tags TEXT DEFAULT '', " 
        "completed_at INTEGER DEFAULT 0, " 
        "FOREIGN KEY(username) REFERENCES users(username) ON DELETE CASCADE" 
        ");";
    
    if (!execute(create_tasks_table)) {
        std::cerr << "创建tasks表失败" << std::endl;
        return false;
    }
    
    // 创建用于存储下一个任务ID的表
    const char* create_task_id_table = 
        "CREATE TABLE IF NOT EXISTS task_id_counter (" 
        "id INTEGER PRIMARY KEY DEFAULT 1, " 
        "next_id INTEGER DEFAULT 1" 
        ");";
    
    if (!execute(create_task_id_table)) {
        std::cerr << "创建task_id_counter表失败" << std::endl;
        return false;
    }
    
    // 初始化task_id_counter表
    const char* init_task_id = 
        "INSERT OR IGNORE INTO task_id_counter (id, next_id) VALUES (1, 1);";
    
    if (!execute(init_task_id)) {
        std::cerr << "初始化task_id_counter表失败" << std::endl;
        return false;
    }
    
    std::cout << "数据库表结构创建成功" << std::endl;
    return true;
}

// 加载所有用户
bool Database::load_all_users(std::unordered_map<std::string, User>& users) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    
    // 检查数据库是否已打开
    if (db_ == nullptr) {
        std::cerr << "数据库未打开" << std::endl;
        return false;
    }
    
    // 查询用户表
    const char* sql = "SELECT username, password_hash, email, phone_number, email_verified, "
                     "phone_verified, created_at, last_login, avatar_path FROM users;";
    
    bool result = query(sql, [&users](sqlite3_stmt* stmt) {
        User user;
        user.username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        user.password_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        user.email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        user.phone_number = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        user.email_verified = sqlite3_column_int(stmt, 4) != 0;
        user.phone_verified = sqlite3_column_int(stmt, 5) != 0;
        user.created_at = sqlite3_column_int64(stmt, 6);
        user.last_login = sqlite3_column_int64(stmt, 7);
        user.avatar_path = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        
        users[user.username] = user;
        return true;
    });
    
    if (result) {
        std::cout << "成功加载 " << users.size() << " 个用户数据" << std::endl;
    }
    
    return result;
}