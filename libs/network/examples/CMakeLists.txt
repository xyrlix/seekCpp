# CMake 最低版本要求
cmake_minimum_required(VERSION 3.16)

# 查找网络库
find_package(seekcpp_network REQUIRED)

# 添加服务器示例
add_executable(server_example server_example.cpp)
# 链接网络库
target_link_libraries(server_example PRIVATE seekcpp_network)

# 添加客户端示例
add_executable(client_example client_example.cpp)
# 链接网络库
target_link_libraries(client_example PRIVATE seekcpp_network)

# 设置编译选项
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(server_example PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(client_example PRIVATE -Wall -Wextra -Wpedantic)
endif()

# 设置 C++ 标准为 C++20
target_compile_features(server_example PRIVATE cxx_std_20)
target_compile_features(client_example PRIVATE cxx_std_20)

# 安装示例
sinstall(TARGETS server_example client_example
    RUNTIME DESTINATION bin/examples/network
)