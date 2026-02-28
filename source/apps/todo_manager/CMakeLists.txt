project(todo_manager)

# 查找SQLite3库
find_package(SQLite3 REQUIRED)

# 添加子目录
add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(client)

# 设置全局包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}/libs/third_party/nlohmann_json/include
)