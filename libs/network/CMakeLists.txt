# CMake 最低版本要求
cmake_minimum_required(VERSION 3.16)

# 项目名称
project(seekCppNetwork VERSION 1.0.0 LANGUAGES CXX)

# 设置 C++ 标准为 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 定义库名称
set(LIBRARY_NAME "seekcpp_network")

# 设置编译选项
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    # 添加优化选项
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O3)
    endif()
endif()

# 添加包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 定义源文件列表
set(SOURCES
    src/client_connection.cpp
    src/server.cpp
    src/server_impl.cpp
    src/server_impl_linux.cpp
    src/socket.cpp
    src/socket_factory.cpp
    src/socket_impl.cpp
    src/socket_impl_linux.cpp
)

# 创建静态库
add_library(${LIBRARY_NAME} STATIC ${SOURCES})

# 添加系统依赖
if(UNIX AND NOT APPLE)
    # Linux 平台需要链接 pthread 库
    target_link_libraries(${LIBRARY_NAME} PRIVATE pthread)
endif()

# 安装规则
install(TARGETS ${LIBRARY_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# 安装头文件
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION include/seekCpp/network
    FILES_MATCHING PATTERN "*.h"
)