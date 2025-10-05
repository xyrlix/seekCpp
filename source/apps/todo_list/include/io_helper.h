#pragma once

/**
 * @file    : io_helper.h
 * @brief   : Using to handle user's input and output
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 2025-10-05 09:51:33
 * @copyright: Copyright (c) 2025 Seek Dao
 */

class IoHelperBase {
   public:
    /**
     * @brief Constructor of IoHelper class
     */
    IoHelperBase() = default;

    /**
     * @brief Destructor of IoHelper class
     */
    virtual ~IoHelperBase() = default;

    /**
     * @brief Get user's input
     * @return std::string
     */
    virtual std::string get_input() = 0;

    /**
     * @brief Show menu to user
     */
    virtual void show_menu() = 0;

    /**
     * @brief Show message to user
     * @param message
     */
    virtual void show_message(std::string message) = 0;

    /**
     * @brief Show error message to user
     * @param message
     */
    virtual void show_error(std::string message) = 0;

    /**
     * @brief Check if user want to exit
     * @return bool
     */
    virtualbool check_exit() = 0;

   private:
    bool is_exit;  // flag to check if user want to exit
};

class IoHelperCommandLine : public IoHelperBase {
   public:
    /**
     * @brief Constructor of IoHelper class
     */
    IoHelperCommandLine() = default;

    /**
     * @brief Destructor of IoHelper class
     */
    ~IoHelperCommandLine() = default;

    /**
     * @brief Get user's input
     * @return std::string
     */
    std::string get_input() override;

    /**
     * @brief Show menu to user
     */
    void show_menu() override;

    /**
     * @brief Show message to user
     * @param message
     */
    void show_message(std::string message) override;

    /**
     * @brief Show error message to user
     * @param message
     */
    void show_error(std::string message) override;

    /**
     * @brief Check if user want to exit
     * @return bool
     */
    bool check_exit() override;

   private:
    std::string input_buffer;  // buffer to store user's input
};

class IoHelperGUI : public IoHelperBase {
   public:
    /**
     * @brief Constructor of IoHelper class
     */
    IoHelperGUI() = default;

    /**
     * @brief Destructor of IoHelper class
     */
    ~IoHelperGUI() = default;

    /**
     * @brief Get user's input
     * @return std::string
     */
    std::string get_input() override;

    /**
     * @brief Show menu to user
     */
    void show_menu() override;

    /**
     * @brief Show message to user
     * @param message
     */
    void show_message(std::string message) override;

    /**
     * @brief Show error message to user
     * @param message
     */
    void show_error(std::string message) override;

    /**
     * @brief Check if user want to exit
     * @return bool
     */
    bool check_exit() override;

   private:
    std::string input_buffer;  // buffer to store user's input
};