/**
 * @file    : data_def.h
 * @brief   : Using to define for todo_list common header files.
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 2025-10-05 04:30:32
 * @copyright: Copyright (c) 2025 Seek Dao
 */

// 定义待办数据结构
struct TodoTaskNode {
    uint32_t id;                // 任务id
    bool is_completed;          // 是否完成
    std::string name;           // 任务名称
    std::string descript;       // 描述
    std::int64_t begin_ts_sec;  // 任务开始时间
    std::int64_t end_ts_sec;    // 任务结束时间
};