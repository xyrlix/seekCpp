#include "../include/socket.h"
#include <iostream>
#include <thread>
#include <string>
#include <cstdint>
#include <vector>

namespace seekCpp {
namespace network {
    class Socket;
} // namespace network
} // namespace seekCpp

int main() {
    // 创建套接字实例
    seekCpp::network::Socket socket;
    
    // 设置连接回调
    socket.set_on_connected([]() {
        std::cout << "成功连接到服务器" << std::endl;
    });
    
    // 设置数据接收回调
    socket.set_on_data_received([](const std::vector<uint8_t>& data) {
        std::cout << "收到服务器数据: ";
        for (const auto& byte : data) {
            std::cout << static_cast<char>(byte);
        }
        std::cout << std::endl;
    });
    
    // 设置断开连接回调
    socket.set_on_disconnected([]() {
        std::cout << "与服务器断开连接" << std::endl;
    });
    
    // 设置错误回调
    socket.set_on_error([](const std::string& error) {
        std::cerr << "错误: " << error << std::endl;
    });
    
    // 连接到服务器
    if (!socket.connect("127.0.0.1", 8080)) {
        auto [error_code, error_msg] = socket.get_last_error();
        std::cerr << "连接服务器失败: " << error_msg << std::endl;
        return 1;
    }
    
    std::cout << "已连接到服务器" << std::endl;
    
    // 创建一个线程来处理用户输入
    std::thread input_thread([&socket]() {
        std::string message;
        while (socket.is_connected() && std::getline(std::cin, message)) {
            if (message == "exit") {
                break;
            }
            
            // 发送消息到服务器
            std::vector<uint8_t> data(message.begin(), message.end());
            socket.send(data.data(), data.size());
        }
    });
    
    // 等待输入线程完成
    if (input_thread.joinable()) {
        input_thread.join();
    }
    
    // 断开连接
    socket.disconnect();
    
    return 0;
}