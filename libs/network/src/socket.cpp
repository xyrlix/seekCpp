/**
 * @file    : socket.cpp
 * @brief   : Socket类的实现
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 2025-10-05 04:30:32
 * @copyright: Copyright (c) 2025 Seek Dao
 */

#include "../include/socket.h"
#include "../include/socket_impl.h"
#include <stdexcept>
#include <iostream>
#include <string>
#include <cstdint>
#include <cstddef>

Socket::Socket() : impl_(create_socket_impl(this)) {
}

Socket::~Socket() {
    disconnect();
}

Socket::Socket(Socket&& other) noexcept : impl_(std::move(other.impl_)) {
    // 更新实现中的父指针
    if (impl_) {
        // 这个需要在实现中处理
    }
}

Socket& Socket::operator=(Socket&& other) noexcept {
    if (this != &other) {
        disconnect();
        impl_ = std::move(other.impl_);
        // 更新实现中的父指针
        if (impl_) {
            // 这个需要在实现中处理
        }
    }
    return *this;
}

bool Socket::connect(const std::string& host, uint16_t port) {
    if (!impl_) {
        throw std::runtime_error("Socket implementation not initialized");
    }
    return impl_->connect(host, port);
}

std::future<bool> Socket::connect_async(const std::string& host, uint16_t port) {
    return std::async(std::launch::async, [this, host, port]() {
        return this->connect(host, port);
    });
}

int Socket::send(const void* data, size_t size) {
    if (!impl_) {
        throw std::runtime_error("Socket implementation not initialized");
    }
    return impl_->send(data, size);
}

std::future<int> Socket::send_async(const void* data, size_t size) {
    return std::async(std::launch::async, [this, data, size]() {
        return this->send(data, size);
    });
}

int Socket::receive(void* buffer, size_t size) {
    if (!impl_) {
        throw std::runtime_error("Socket implementation not initialized");
    }
    return impl_->receive(buffer, size);
}

std::future<int> Socket::receive_async(void* buffer, size_t size) {
    return std::async(std::launch::async, [this, buffer, size]() {
        return this->receive(buffer, size);
    });
}

void Socket::disconnect() {
    if (impl_) {
        impl_->disconnect();
    }
}

bool Socket::is_connected() const {
    return impl_ && impl_->is_connected();
}

Socket::State Socket::get_state() const {
    if (!impl_) {
        return State::Disconnected;
    }
    return impl_->get_state();
}

void Socket::set_data_handler(DataHandler handler) {
    data_handler_ = std::move(handler);
}

void Socket::set_connection_handler(ConnectionHandler handler) {
    connection_handler_ = std::move(handler);
}

void Socket::set_disconnection_handler(ConnectionHandler handler) {
    disconnection_handler_ = std::move(handler);
}

void Socket::set_error_handler(ErrorHandler handler) {
    error_handler_ = std::move(handler);
}

std::string Socket::get_remote_address() const {
    if (!impl_) {
        return "";
    }
    return impl_->get_remote_address();
}

uint16_t Socket::get_remote_port() const {
    if (!impl_) {
        return 0;
    }
    return impl_->get_remote_port();
}

std::string Socket::get_local_address() const {
    if (!impl_) {
        return "";
    }
    return impl_->get_local_address();
}

uint16_t Socket::get_local_port() const {
    if (!impl_) {
        return 0;
    }
    return impl_->get_local_port();
}

bool Socket::set_socket_descriptor(int socket_fd, const std::string& remote_addr, uint16_t remote_port) {
    if (!impl_) {
        throw std::runtime_error("Socket implementation not initialized");
    }
    return impl_->set_socket_descriptor(socket_fd, remote_addr, remote_port);
}

std::pair<Socket::Error, std::string> Socket::get_last_error() const {
    if (!impl_) {
        return {Error::InvalidState, "Socket implementation not initialized"};
    }
    return impl_->get_last_error();
}