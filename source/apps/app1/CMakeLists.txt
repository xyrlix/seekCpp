# Add executable
add_executable(app1 
    source/main.cpp
    source/app1.cpp
)

# Set include directories
target_include_directories(app1
    PRIVATE
        ${CMAKE_SOURCE_DIR}/libs/module1/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_directories(app1 PRIVATE
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib/mymodule1
)

# Set dependencies
target_link_libraries(app1
    PRIVATE
        mymodule1)

# Set output directory
set_target_properties(app1 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/app1
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/app1
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/app1
)

# Add tests if testing is enabled
if(BUILD_TESTING)
    add_subdirectory(test)
endif()
