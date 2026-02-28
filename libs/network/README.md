# seekCpp Network Library

一个基于 C++17 的高性能、跨平台网络库。

## 特性

- 基于现代 C++17 标准
- 采用 Pimpl 模式实现跨平台支持
- 支持 TCP 服务器和客户端
- 非阻塞 I/O
- 事件驱动的回调机制
- 线程安全的设计
- 支持异步操作

## 架构

该网络库采用分层设计：

1. **公共接口层**：提供给用户的 API，包括 `Socket`、`Server` 和 `ClientConnection` 类
2. **实现抽象层**：定义抽象的实现接口，包括 `SocketImpl` 和 `ServerImpl` 类
3. **平台特定实现层**：针对不同平台的具体实现，目前支持 Linux

## 使用示例

### TCP 服务器示例

```cpp
#include "network/server.h"
#include <iostream>

int main() {
    using namespace seekCpp::network;
    
    // 创建服务器实例
    Server server;
    
    // 设置回调函数
    server.set_connection_handler([](const ClientConnection& client) {
        std::cout << "Client connected: " << client.get_remote_address() << ":" << client.get_remote_port() << std::endl;
        
        // 发送欢迎消息
        std::string welcome = "Welcome to seekCpp Network Server!\n";
        std::vector<uint8_t> data(welcome.begin(), welcome.end());
        server.send_to_client(client.get_id(), data);
    });
    
    server.set_data_handler([&server](const ClientConnection& client, const std::vector<uint8_t>& data) {
        std::cout << "Received data from client " << client.get_id() << ": ";
        std::cout.write(reinterpret_cast<const char*>(data.data()), data.size());
        std::cout << std::endl;
        
        // 回显数据
        server.send_to_client(client.get_id(), data);
    });
    
    server.set_disconnection_handler([](const ClientConnection& client) {
        std::cout << "Client disconnected: " << client.get_remote_address() << ":" << client.get_remote_port() << std::endl;
    });
    
    server.set_error_handler([](const std::string& error_msg) {
        std::cerr << "Server error: " << error_msg << std::endl;
    });
    
    // 启动服务器，监听 8080 端口，最大连接数为 100
    if (!server.start(8080, 100)) {
        std::cerr << "Failed to start server" << std::endl;
        return 1;
    }
    
    std::cout << "Server started on port 8080" << std::endl;
    std::cout << "Press Enter to stop the server..." << std::endl;
    
    // 等待用户输入
    std::cin.get();
    
    // 停止服务器
    server.stop();
    
    return 0;
}
```

### TCP 客户端示例

```cpp
#include "network/socket.h"
#include <iostream>
#include <thread>
#include <string>

int main() {
    using namespace seekCpp::network;
    
    // 创建套接字实例
    Socket socket;
    
    // 设置回调函数
    socket.set_connection_handler([]() {
        std::cout << "Connected to server" << std::endl;
    });
    
    socket.set_data_handler([](const std::vector<uint8_t>& data) {
        std::cout << "Received data: ";
        std::cout.write(reinterpret_cast<const char*>(data.data()), data.size());
        std::cout << std::endl;
    });
    
    socket.set_disconnection_handler([]() {
        std::cout << "Disconnected from server" << std::endl;
    });
    
    socket.set_error_handler([](Socket::Error error, const std::string& error_msg) {
        std::cerr << "Socket error (" << static_cast<int>(error) << "): " << error_msg << std::endl;
    });
    
    // 连接到服务器
    if (!socket.connect("127.0.0.1", 8080)) {
        std::cerr << "Failed to connect to server" << std::endl;
        std::cerr << "Error: " << socket.get_last_error().second << std::endl;
        return 1;
    }
    
    std::cout << "Connected to server" << std::endl;
    
    // 创建一个线程来接收用户输入
    std::thread input_thread([&socket]() {
        std::string line;
        while (std::getline(std::cin, line)) {
            // 发送数据到服务器
            std::vector<uint8_t> data(line.begin(), line.end());
            if (!socket.send(data)) {
                std::cerr << "Failed to send data" << std::endl;
                break;
            }
        }
    });
    
    // 等待用户输入线程结束
    input_thread.join();
    
    // 断开连接
    socket.disconnect();
    
    return 0;
}
```

## 编译和构建

### Linux 平台

1. 确保安装了 C++17 兼容的编译器（如 GCC 10+ 或 Clang 10+）
2. 创建 build 目录并进入
   ```bash
   mkdir build && cd build
   ```
3. 使用 CMake 配置项目
   ```bash
   cmake ..
   ```
4. 编译项目
   ```bash
   make
   ```

## 平台支持

- Linux（当前已实现）
- Windows（计划中）
- macOS（计划中）

## 注意事项

- 本库仍在开发中，API 可能会发生变化
- 目前只支持 IPv4，IPv6 支持计划中
- 请确保使用 C++17 兼容的编译器

## 许可证

MIT License