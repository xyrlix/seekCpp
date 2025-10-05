#!/bin/bash

# 设置错误时退出
set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 打印带颜色的信息
print_info() {
    echo -e "${GREEN}[INFO] $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}[WARNING] $1${NC}"
}

print_error() {
    echo -e "${RED}[ERROR] $1${NC}"
}

# 检查命令是否存在
check_command() {
    if ! command -v $1 &> /dev/null; then
        print_error "$1 未安装，请先安装"
        exit 1
    fi
}

# 检查必要的命令
check_commands() {
    check_command "cmake"
    check_command "make"
    check_command "gcc"
    check_command "g++"
}

# 检查CMake版本
check_cmake_version() {
    local required_version="3.8"
    local current_version=$(cmake --version | head -n1 | cut -d" " -f3)
    
    if [ "$(printf '%s\n' "$required_version" "$current_version" | sort -V | head -n1)" != "$required_version" ]; then
        print_error "CMake版本过低，需要 $required_version 或更高版本"
        exit 1
    fi
}

# 显示帮助信息
show_help() {
    echo "用法: $0 [选项]"
    echo "选项:"
    echo "  -h, --help     显示帮助信息"
    echo "  -c, --clean    清理构建目录"
    echo "  -d, --debug    构建Debug版本"
    echo "  -r, --release  构建Release版本"
    echo "  -j, --jobs N   使用N个线程进行编译"
    echo "  -t, --test     启用测试"
    echo "  -s, --sample   启用示例"
    echo "  -i, --install  安装到系统"
}

# 默认参数
BUILD_TYPE="Release"
ENABLE_TEST="OFF"
ENABLE_SAMPLE="OFF"
INSTALL="OFF"
JOBS=$(nproc)

# 解析命令行参数
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        -c|--clean)
            print_info "清理构建目录..."
            rm -rf build output
            print_info "清理完成"
            exit 0
            ;;
        -d|--debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        -r|--release)
            BUILD_TYPE="Release"
            shift
            ;;
        -j|--jobs)
            JOBS="$2"
            shift 2
            ;;
        -t|--test)
            ENABLE_TEST="ON"
            shift
            ;;
        -s|--sample)
            ENABLE_SAMPLE="ON"
            shift
            ;;
        -i|--install)
            INSTALL="ON"
            shift
            ;;
        *)
            print_error "未知选项: $1"
            show_help
            exit 1
            ;;
    esac
done

# 主函数
main() {
    # 检查环境
    check_commands
    check_cmake_version

    # 创建构建目录
    if [ ! -d "build" ]; then
        mkdir build
    fi

    cd build

    # 根据操作系统设置平台选项
    if [ "$(uname)" == "Linux" ]; then
        PLATFORM_OPTION="-DBUILD_NATIVE=ON"
    else
        PLATFORM_OPTION=""
    fi

    # 配置CMake
    print_info "配置CMake..."
    cmake .. \
        -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
        -DBUILD_GTEST=$ENABLE_TEST \
        -DBUILD_SAMPLES=$ENABLE_SAMPLE \
        $PLATFORM_OPTION

    # 编译
    print_info "开始编译..."
    make -j$JOBS

    # 安装
    if [ "$INSTALL" == "ON" ]; then
        print_info "开始安装..."
        make install
    fi

    print_info "构建完成！"
}

# 执行主函数
main