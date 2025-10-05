#pragma once

/**
 * @file    : data_def.h
 * @brief   : Using to define for todo_list common header files.
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 2025-10-05 04:30:32
 * @copyright: Copyright (c) 2025 Seek Dao
 */

struct TaskNode {
    uint32_t id;                // task id
    bool is_completed;          // Is it completed?
    std::string name;           // Task Name
    std::string descript;       // Task Description
    std::int64_t begin_ts_sec;  // Task start time
    std::int64_t end_ts_sec;    // Task end time
};

struct UserNode {
    std::string username;       // username
    std::string location;       // Login location
    std::int64_t login_ts_sec;  // login time
};