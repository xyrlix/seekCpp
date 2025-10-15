#include "../include/server.h"
#include <iostream>
#include <thread>
#include <string>
#include <cstdint>
#include <vector>

namespace seekCpp {
namespace network {
    class Server;
    class ClientConnection;
} // namespace network
} // namespace seekCpp

int main() {
    // 创建服务器实例
    seekCpp::network::Server server;
    
    // 设置连接回调
    server.set_on_client_connected([](seekCpp::network::ClientConnection::Id client_id) {
        std::cout << "客户端已连接，ID: " << client_id << std::endl;
    });
    
    // 设置数据接收回调
    server.set_on_data_received([&server](seekCpp::network::ClientConnection::Id client_id, const std::vector<uint8_t>& data) {
        std::cout << "收到来自客户端 " << client_id << " 的数据: ";
        for (const auto& byte : data) {
            std::cout << static_cast<char>(byte);
        }
        std::cout << std::endl;
        
        // 回显数据
        server.send_to_client(client_id, data);
    });
    
    // 设置断开连接回调
    server.set_on_client_disconnected([](seekCpp::network::ClientConnection::Id client_id) {
        std::cout << "客户端已断开，ID: " << client_id << std::endl;
    });
    
    // 设置错误回调
    server.set_on_error([](const std::string& error) {
        std::cerr << "服务器错误: " << error << std::endl;
    });
    
    // 启动服务器，监听8080端口，最大连接数100
    if (!server.start(8080, 100)) {
        std::cerr << "启动服务器失败: " << server.get_last_error() << std::endl;
        return 1;
    }
    
    std::cout << "服务器已启动，监听端口: 8080" << std::endl;
    std::cout << "按Enter键停止服务器..." << std::endl;
    
    // 等待用户输入
    std::cin.get();
    
    // 停止服务器
    server.stop();
    std::cout << "服务器已停止" << std::endl;
    
    return 0;
}