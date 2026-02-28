/**
 * @file    : task.h
 * @brief   : 定义任务结构体
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 2025-10-05 04:30:32
 * @copyright: Copyright (c) 2025 Seek Dao
 */

#ifndef TASK_H
#define TASK_H

#include <string>
#include <ctime>
#include <vector>

// 任务优先级枚举
enum class TaskPriority {
    LOW = 0,
    MEDIUM = 1,
    HIGH = 2,
    URGENT = 3
};

struct Task {
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

#endif // TASK_H