/**
 * @file    : user.h
 * @brief   : 定义用户结构体
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 2025-10-05 04:30:32
 * @copyright: Copyright (c) 2025 Seek Dao
 */

#ifndef USER_H
#define USER_H

#include <string>
#include <ctime>

struct User {
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

#endif // USER_H