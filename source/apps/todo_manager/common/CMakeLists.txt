# Common library with shared code between server and client
add_library(common STATIC
    src/message.cpp
    src/network_utils.cpp
    include/task.h
    include/user.h
    include/message.h
    include/network_utils.h
)

target_include_directories(common PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/third_party/nlohmann_json/include>
)

target_link_libraries(common PUBLIC
    SQLite::SQLite3
)