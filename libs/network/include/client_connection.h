#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <functional>
#include "socket.h"

namespace seekCpp {
namespace network {

// class Socket; // 不需要前向声明，直接包含头文件

class ClientConnection {
public:
    using Id = uint64_t;
    using Data = std::vector<uint8_t>;
    using DisconnectCallback = std::function<void(Id)>;
    using DataCallback = std::function<void(Id, const Data&)>;
    using ErrorCallback = std::function<void(Id, const std::string&)>;

    ClientConnection(Id id, std::unique_ptr<::Socket> socket);
    ClientConnection(const ClientConnection&) = delete;
    ClientConnection& operator=(const ClientConnection&) = delete;
    
    ClientConnection(ClientConnection&& other) noexcept;
    ClientConnection& operator=(ClientConnection&& other) noexcept;
    
    ~ClientConnection();

    Id get_id() const;
    std::string get_address() const;
    uint16_t get_port() const;
    bool is_connected() const;

    bool send(const Data& data);
    bool disconnect();

    void set_on_data_callback(DataCallback callback);
    void set_on_disconnect_callback(DisconnectCallback callback);
    void set_on_error_callback(ErrorCallback callback);

    void process_data();
    void handle_disconnect();
    void handle_error(const std::string& error);

private:
    Id id_;
    std::unique_ptr<::Socket> socket_;
    bool connected_;
    
    DataCallback on_data_callback_;
    DisconnectCallback on_disconnect_callback_;
    ErrorCallback on_error_callback_;
};

} // namespace network
} // namespace seekCpp