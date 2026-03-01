#!/bin/bash

# 构建第三方库脚本
# 这个脚本将spdlog和nlohmann_json编译为静态库并安装到系统中

# 设置中文环境
export LANG="zh_CN.UTF-8"
export LC_ALL="zh_CN.UTF-8"

# 创建构建目录
BUILD_DIR="/tmp/third_party_build"
INSTALL_PREFIX="/usr/local"

# 清理之前的构建
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

# 获取当前脚本所在目录
script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# 函数：构建并安装spdlog
build_spdlog() {
    echo "开始构建spdlog..."
    cd "$script_dir/libs/third_party/spdlog"
    mkdir -p build
    cd build
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DSPDLOG_BUILD_SHARED=OFF \
        -DSPDLOG_INSTALL=ON \
        -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX"
    make -j$(nproc)
    sudo make install
    echo "spdlog构建和安装完成！"
}

# 函数：构建并安装nlohmann_json
build_nlohmann_json() {
    echo "开始构建nlohmann_json..."
    cd "$script_dir/libs/third_party/nlohmann_json"
    mkdir -p build
    cd build
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DJSON_BuildTests=OFF \
        -DJSON_Install=ON \
        -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX"
    make -j$(nproc)
    sudo make install
    echo "nlohmann_json构建和安装完成！"
}

# 执行构建
main() {
    echo "===== 开始构建第三方静态库 ====="
    build_spdlog
    build_nlohmann_json
    echo "===== 所有第三方库构建完成并已安装到$INSTALL_PREFIX ====="
    echo "\n你现在可以从你的项目中移除第三方库的源代码，直接链接到已安装的静态库。"
    echo "\n在你的CMakeLists.txt中，你可以使用以下配置来链接这些库："
    echo "find_package(spdlog REQUIRED)"
    echo "find_package(nlohmann_json REQUIRED)"
    echo "target_link_libraries(your_target PRIVATE spdlog::spdlog nlohmann_json::nlohmann_json)"
}

# 运行主函数
main