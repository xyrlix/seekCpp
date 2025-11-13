# Client executable

add_executable(client
    src/main.cpp
    src/client.cpp
    src/socket.cpp
)

target_include_directories(client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/include
    ${CMAKE_SOURCE_DIR}/libs/network/include
    ${CMAKE_SOURCE_DIR}/libs/third_party/spdlog/include
)

target_link_libraries(client PRIVATE
    common
    seekcpp_network
    spdlog
)