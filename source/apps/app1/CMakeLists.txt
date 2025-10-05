# set
set(THIS_APP_NAME "app1")

# Add executable
add_executable(${THIS_APP_NAME} 
    source/main.cpp
)

# Set include directories
target_include_directories(${THIS_APP_NAME} 
    PRIVATE
        ${CMAKE_SOURCE_DIR}/libs/module1/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_directories(${THIS_APP_NAME} PRIVATE
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib/mymodule1
)

# Set dependencies
target_link_libraries(${THIS_APP_NAME} 
    PRIVATE
        mymodule1)

# Set output directory
set_target_properties(${THIS_APP_NAME}  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${THIS_APP_NAME} 
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${THIS_APP_NAME} 
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${THIS_APP_NAME} 
)

# Add tests if testing is enabled
if(BUILD_TESTING)
    add_subdirectory(test)
endif()
