/**
 * @file    : main.cpp
 * @brief   : demo app1
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 2025-10-04 13:32:42
 * @copyright: Copyright (c) 2025 Seek Dao
 */

#include <iostream>

#include "../include/todo_list.h"

int main() {
    
    // 创建待办事项列表管理器
    todo_list::TodoListManager todo_manager;
    todo_manager.run();
    return 0;
}