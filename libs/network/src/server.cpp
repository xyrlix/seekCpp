/**
 * @file    : server.cpp
 * @brief   : Server类的实现
 * @author  : xyrlix (xyrlix@outlook.com)
 * @date    : 2025-10-05 04:30:32
 * @copyright: Copyright (c) 2025 Seek Dao
 */

#include "../include/server.h"
#include "../include/server_impl.h"
#include <stdexcept>

Server::Server() : impl_(create_server_impl(this)) {
}

Server::~Server() {
    stop();
}

Server::Server(Server&& other) noexcept : impl_(std::move(other.impl_)) {
    // 更新实现中的父指针
    if (impl_) {
        // 这个需要在实现中处理
    }
}

Server& Server::operator=(Server&& other) noexcept {
    if (this != &other) {
        stop();
        impl_ = std::move(other.impl_);
        // 更新实现中的父指针
        if (impl_) {
            // 这个需要在实现中处理
        }
    }
    return *this;
}

bool Server::start(uint16_t port, size_t max_connections) {
    if (!impl_) {
        throw std::runtime_error("Server implementation not initialized");
    }
    return impl_->start(port, max_connections);
}

void Server::stop() {
    if (impl_) {
        impl_->stop();
    }
}

bool Server::is_running() const {
    return impl_ && impl_->is_running();
}

Server::State Server::get_state() const {
    if (!impl_) {
        return State::Stopped;
    }
    return impl_->get_state();
}

int Server::send_to_client(ClientConnection::Id client_id, const void* data, size_t size) {
    if (!impl_) {
        throw std::runtime_error("Server implementation not initialized");
    }
    return impl_->send_to_client(client_id, data, size);
}

size_t Server::broadcast(const void* data, size_t size) {
    if (!impl_) {
        throw std::runtime_error("Server implementation not initialized");
    }
    return impl_->broadcast(data, size);
}

void Server::disconnect_client(ClientConnection::Id client_id) {
    if (impl_) {
        impl_->disconnect_client(client_id);
    }
}

size_t Server::get_connected_clients_count() const {
    if (!impl_) {
        return 0;
    }
    return impl_->get_connected_clients_count();
}

void Server::set_data_handler(DataHandler handler) {
    data_handler_ = std::move(handler);
}

void Server::set_connection_handler(ConnectionHandler handler) {
    connection_handler_ = std::move(handler);
}

void Server::set_disconnection_handler(ConnectionHandler handler) {
    disconnection_handler_ = std::move(handler);
}

void Server::set_error_handler(ErrorHandler handler) {
    error_handler_ = std::move(handler);
}

// ClientConnection 类的实现

ClientConnection::ClientConnection(Id id, std::unique_ptr<Socket> socket) 
    : id_(id), socket_(std::move(socket)) {
    // 设置套接字的数据处理回调
    socket_->set_data_handler([this](const std::vector<uint8_t>& data) {
        if (data_handler_) {
            data_handler_(id_, data);
        }
    });
    
    // 设置套接字的断开连接回调
    socket_->set_disconnection_handler([this]() {
        if (disconnection_handler_) {
            disconnection_handler_(id_);
        }
    });
}

ClientConnection::~ClientConnection() {
    disconnect();
}

ClientConnection::ClientConnection(ClientConnection&& other) noexcept 
    : id_(other.id_), socket_(std::move(other.socket_)), 
      data_handler_(std::move(other.data_handler_)), 
      disconnection_handler_(std::move(other.disconnection_handler_)) {
    // 重置源对象的ID
    other.id_ = 0;
}

ClientConnection& ClientConnection::operator=(ClientConnection&& other) noexcept {
    if (this != &other) {
        disconnect();
        
        id_ = other.id_;
        socket_ = std::move(other.socket_);
        data_handler_ = std::move(other.data_handler_);
        disconnection_handler_ = std::move(other.disconnection_handler_);
        
        // 重置源对象的ID
        other.id_ = 0;
    }
    return *this;
}

ClientConnection::Id ClientConnection::get_id() const {
    return id_;
}

int ClientConnection::send(const void* data, size_t size) {
    if (!socket_ || !socket_->is_connected()) {
        return -1;
    }
    return socket_->send(data, size);
}

std::future<int> ClientConnection::send_async(const void* data, size_t size) {
    if (!socket_ || !socket_->is_connected()) {
        // 返回一个已完成的future，结果为-1
        std::promise<int> promise;
        promise.set_value(-1);
        return promise.get_future();
    }
    return socket_->send_async(data, size);
}

void ClientConnection::disconnect() {
    if (socket_) {
        socket_->disconnect();
    }
}

bool ClientConnection::is_active() const {
    return socket_ && socket_->is_connected();
}

std::string ClientConnection::get_client_address() const {
    if (!socket_ || !socket_->is_connected()) {
        return "";
    }
    return socket_->get_remote_address();
}

uint16_t ClientConnection::get_client_port() const {
    if (!socket_ || !socket_->is_connected()) {
        return 0;
    }
    return socket_->get_remote_port();
}

void ClientConnection::set_data_handler(std::function<void(Id, const std::vector<uint8_t>&)> handler) {
    data_handler_ = std::move(handler);
}

void ClientConnection::set_disconnection_handler(std::function<void(Id)> handler) {
    disconnection_handler_ = std::move(handler);
}