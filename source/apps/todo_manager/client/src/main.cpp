#include "client.h"
#include <iostream>
#include "client.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

int main() {
    try {
        // 初始化日志器
        spdlog::stdout_color_mt("client");
        spdlog::set_level(spdlog::level::info);
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%L%$] [%t] %v");
        spdlog::info("Client starting...");
        
        // 创建并运行客户端
        Client c("127.0.0.1", "12345"); // 连接到本地的12345端口
        c.run();
        
        // 清理日志资源
        spdlog::shutdown();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}