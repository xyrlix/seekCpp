#pragma once

/**
 * @file    : store_helper.h
 * @brief   : Using abstract class to define the interface for store helper.
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 2025-10-05 10:26:48
 * @copyright: Copyright (c) 2025 Seek Dao
 */

#include <string>

class StoreHelperBase {
   public:
    StoreHelperBase() = default;

    virtual ~StoreHelperBase() = default;
    virtual bool save(const std::string& data) = 0;
    virtual bool load(std::string& data) = 0;
};

class FileHelper : public StoreHelperBase {
   public:
    FileHelper(const std::string& fileName) : m_fileName(fileName) {}
    ~FileHelper() override;
    bool save(const std::string& data) override;
    bool load(std::string& data) override;

   private:
    std::string m_fileName;
};

class DatabaseHelper : public StoreHelperBase {
   public:
    DatabaseHelper(const std::string& connectionString) : m_connectionString(connectionString) {}
    ~DatabaseHelper() override = default;
    bool save(const std::string& data) override;
    bool load(std::string& data) override;

   private:
    std::string m_connectionString;
};
