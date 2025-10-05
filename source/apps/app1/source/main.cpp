/**
 * @file    : main.cpp
 * @brief   : demo app1
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 2025-10-04 13:32:42
 * @copyright: Copyright (c) 2025 Seek Dao
 */

#include <iostream>

#include "../include/app1.h"
#include "module1.h"

int main() {
    std::cout << module1::hello() << std::endl;
    return 0;
}