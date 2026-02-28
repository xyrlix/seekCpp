#include "../include/client_connection.h"
#include "../include/client_connection.h"
#include <iostream>
#include <vector>

namespace seekCpp {
namespace network {

// 构造函数
ClientConnection::ClientConnection(Id id, std::unique_ptr<::Socket> socket)
    : id_(id), socket_(std::move(socket)), connected_(true) {
    // 设置回调
    if (socket_) {
        socket_->set_data_handler([this](const Data& data) {
            if (on_data_callback_ && connected_) {
                on_data_callback_(id_, data);
            }
        });
        
        socket_->set_disconnection_handler([this]() {
            if (connected_) {
                connected_ = false;
                handle_disconnect();
            }
        });
        
        socket_->set_error_handler([this](Socket::Error /* error */, const std::string& error_msg) {
            if (connected_) {
                handle_error(error_msg);
            }
        });
    }
}

// 移动构造函数
ClientConnection::ClientConnection(ClientConnection&& other) noexcept
    : id_(other.id_), socket_(std::move(other.socket_)), connected_(other.connected_),
      on_data_callback_(std::move(other.on_data_callback_)),
      on_disconnect_callback_(std::move(other.on_disconnect_callback_)),
      on_error_callback_(std::move(other.on_error_callback_)) {
    other.id_ = 0;
    other.connected_ = false;
}

// 移动赋值运算符
ClientConnection& ClientConnection::operator=(ClientConnection&& other) noexcept {
    if (this != &other) {
        id_ = other.id_;
        socket_ = std::move(other.socket_);
        connected_ = other.connected_;
        on_data_callback_ = std::move(other.on_data_callback_);
        on_disconnect_callback_ = std::move(other.on_disconnect_callback_);
        on_error_callback_ = std::move(other.on_error_callback_);
        
        other.id_ = 0;
        other.connected_ = false;
    }
    return *this;
}

// 析构函数
ClientConnection::~ClientConnection() {
    disconnect();
}

// 获取ID
ClientConnection::Id ClientConnection::get_id() const {
    return id_;
}

// 获取地址
std::string ClientConnection::get_address() const {
    if (!socket_ || !connected_) {
        return "";
    }
    return socket_->get_remote_address();
}

// 获取端口
uint16_t ClientConnection::get_port() const {
    if (!socket_ || !connected_) {
        return 0;
    }
    return socket_->get_remote_port();
}

// 检查是否连接
bool ClientConnection::is_connected() const {
    return connected_ && socket_ && socket_->get_state() == Socket::State::Connected;
}

// 发送数据
bool ClientConnection::send(const Data& data) {
    if (!socket_ || !connected_ || socket_->get_state() != Socket::State::Connected) {
        return false;
    }
    return socket_->send(data.data(), data.size());
}

// 断开连接
bool ClientConnection::disconnect() {
    if (socket_ && connected_) {
        connected_ = false;
        socket_->disconnect();
        handle_disconnect();
        return true;
    }
    return false;
}

// 处理数据
void ClientConnection::process_data() {
    // 这个方法可能需要实现，但头文件中没有具体定义
}

// 处理断开连接
void ClientConnection::handle_disconnect() {
    if (on_disconnect_callback_) {
        on_disconnect_callback_(id_);
    }
}

// 处理错误
void ClientConnection::handle_error(const std::string& error) {
    if (on_error_callback_) {
        on_error_callback_(id_, error);
    }
}

// 设置数据回调
void ClientConnection::set_on_data_callback(DataCallback callback) {
    on_data_callback_ = callback;
}

// 设置断开连接回调
void ClientConnection::set_on_disconnect_callback(DisconnectCallback callback) {
    on_disconnect_callback_ = callback;
}

// 设置错误回调
void ClientConnection::set_on_error_callback(ErrorCallback callback) {
    on_error_callback_ = callback;
}

} // namespace network
} // namespace seekCpp