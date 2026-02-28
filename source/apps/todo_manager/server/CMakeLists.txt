# Server executable
add_executable(server
    src/main.cpp
    src/server.cpp
    src/database.cpp
    include/server.h
    include/database.h
)

target_include_directories(server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/include
)

target_link_libraries(server PRIVATE
    common
    SQLite::SQLite3
)