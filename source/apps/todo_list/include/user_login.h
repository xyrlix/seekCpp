#pragma once

/**
 * @file    : login.h
 * @brief   : Using to handle sign in or register user
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 2025-10-05 05:23:05
 * @copyright: Copyright (c) 2025 Seek Dao
 */

#include "data_def.h"

/**
 * @class   : UserLoginBase
 * @brief   : Base class for user login
 * @details : This class provides the basic interface for user login.
 */
class UserLoginBase {
   public:
    /**
     * @brief   : Constructor
     */
    UserLoginBase();

    /**
     * @brief   : Destructor
     */
    virtual ~UserLoginBase();

    /**
     * @brief   : Sign in user
     * @param   : user_name - user name
     * @param   : password - password
     * @return  : true - sign in success, false - sign in failed
     */
    virtual bool sign_in(const std::string& user_name, const std::string& password) = 0;

    /**
     * @brief   : Sign out user
     * @param   : user_name - user name
     * @return  : true - sign out success, false - sign out failed
     */
    virtual bool sign_out(const std::string& user_name) = 0;

    /**
     * @brief   : Register user
     * @param   : user_name - user name
     * @param   : password - password
     * @return  : true - register success, false - register failed
     */
    virtual bool register_user(const std::string& user_name, const std::string& password) = 0;

    /**
     * @brief   : Check user login status
     * @param   : user_name - user name
     * @param   : password - password
     * @return  : true - login success, false - login failed
     */
    virtual bool check_user_login_status(const std::string& user_name, const std::string& password) = 0;

    /**
     * @brief   : Check if user name is valid
     * @param   : user_name - user name
     * @return  : true - valid, false - invalid
     */
    virtual bool is_valid_user_name(const std::string& user_name) = 0;

    /**
     * @brief   : Check if password is valid
     * @param   : password - password
     * @return  : true - valid, false - invalid
     */
    virtual bool is_valid_password(const std::string& password) = 0;

    /**
     * @brief   : Check if user name is already registered
     * @param   : user_name - user name
     * @return  : true - registered, false - not registered
     */
    virtual bool is_user_registered(const std::string& user_name) = 0;

    /**
     * @brief   : Add user to database
     * @param   : user_name - user name
     * @param   : password - password
     * @return  : true - add success, false - add failed
     */
    virtual bool add_user_to_save_data(const std::string& user_name, const std::string& password) = 0;

    /**
     * @brief   : Get user node by user name
     * @param   : user_name - user name
     * @return  : user node - user node if found, nullptr - user node not found
     */
    virtual UserNode* get_user_node_by_user_name(const std::string& user_name) = 0;

    /**
     * @brief   : Get user node by user name and password
     * @param   : user_name - user name
     * @param   : password - password
     * @return  : user node - user node if found, nullptr - user node not found
     */
    virtual UserNode* get_user_node_by_user_name_and_password(const std::string& user_name, const std::string& password) = 0;

    /**
     * @brief   : Get user node by login timestamp
     * @param   : login_ts_sec - login timestamp in seconds
     * @return  : user node - user node if found, nullptr - user node not found
     */
    virtual UserNode* get_user_node_by_login_ts_sec(std::int64_t login_ts_sec) = 0;

    /**
     * @brief   : Update user login timestamp
     * @param   : user_node - user node
     * @return  : true - update success, false - update failed
     */
    virtual bool update_user_login_ts_sec(UserNode* user_node) = 0;
};

/**
 * @class   : UserLoginDatabase
 * @brief   : User login using database
 * @details : This class provides the user login using database.
 */
class UserLoginDatabase : public UserLoginBase {};

/**
 * @class   : UserLoginFileSystem
 * @brief   : User login using file system
 * @details : This class provides the user login using file system.
 */
class UserLoginFileSystem : public UserLoginBase {
   public:
    /**
     * @brief   : Constructor
     */
    UserLoginFileSystem();

    /**
     * @brief   : Destructor
     */
    ~UserLoginFileSystem();

    /**
     * @brief   : Sign in user
     * @param   : user_name - user name
     * @param   : password - password
     * @return  : true - sign in success, false - sign in failed
     */
    bool sign_in(const std::string& user_name, const std::string& password) override;

    /**
     * @brief   : Sign out user
     * @param   : user_name - user name
     * @return  : true - sign out success, false - sign out failed
     */
    bool sign_out(const std::string& user_name) override;

    /**
     * @brief   : Register user
     * @param   : user_name - user name
     * @param   : password - password
     * @return  : true - register success, false - register failed
     */
    bool register_user(const std::string& user_name, const std::string& password) override;

    /**
     * @brief   : Check user login status
     * @param   : user_name - user name
     * @param   : password - password
     * @return  : true - login success, false - login failed
     */
    bool check_user_login_status(const std::string& user_name, const std::string& password) override;

   private:
    

   private:
    std::vector<UserNode> user_list_;  // user list
    std::mutex user_list_mutex_;       // user list mutex
};