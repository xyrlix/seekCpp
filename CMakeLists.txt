# 指定最低 CMake 版本
cmake_minimum_required(VERSION 3.16)

# 项目名称和版本
project(ProjectRoot VERSION 1.0.0 LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 添加全局包含目录
include_directories(${CMAKE_SOURCE_DIR}/include)

# 添加子目录
add_subdirectory(libs)
add_subdirectory(source)

# 启用测试
option(BUILD_TESTING "Build tests" OFF)
if(BUILD_TESTING)
    enable_testing()
    find_package(GTest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS})
endif()

# 自动发现所有子模块的测试 
file(GLOB_RECURSE TEST_SUBDIRS "*/test/CMakeLists.txt") 
foreach(test_subdir ${TEST_SUBDIRS})
    get_filename_component(parent_dir ${test_subdir} DIRECTORY)
    add_subdirectory(${parent_dir})
endforeach()