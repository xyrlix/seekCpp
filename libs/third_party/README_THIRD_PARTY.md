# 第三方库管理指南

## 概述

本项目依赖的第三方库 spdlog 和 nlohmann_json 已配置为可编译为独立静态库。通过使用提供的脚本，您可以将这些库编译并安装到系统中，从而从项目中移除它们的源代码，避免每次构建项目时都重新编译这些库。

## 构建第三方静态库

### 使用脚本构建

项目根目录下提供了一个 `build_third_party.sh` 脚本，用于自动构建和安装这两个第三方库：

```bash
# 运行构建脚本
./build_third_party.sh
```

### 脚本功能说明

该脚本会：

1. 在每个第三方库目录中创建单独的构建目录
2. 使用 CMake 配置构建选项（设置为 Release 模式、静态库）
3. 并行编译库文件
4. 将库文件和头文件安装到 `/usr/local` 目录

## 集成方式

成功安装第三方库后，您需要在项目的 CMakeLists.txt 文件中进行相应修改，以链接到已安装的静态库，而不是使用项目内的源代码。

### 修改 CMakeLists.txt

1. 首先，移除或注释掉直接包含第三方库源代码的部分：

```cmake
# 移除或注释这一行
# add_subdirectory(libs/third_party/spdlog)
```

2. 然后，添加查找和链接已安装库的代码：

```cmake
# 查找已安装的库
find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)

# 将库链接到您的目标
# 请将 your_target 替换为您实际的目标名称
# target_link_libraries(your_target PRIVATE spdlog::spdlog nlohmann_json::nlohmann_json)
```

## 验证安装

您可以通过以下方式验证库是否已正确安装：

1. 检查库文件是否存在：
   ```bash
   ls -la /usr/local/lib/libspdlog.a
   ls -la /usr/local/lib/libnlohmann_json.a
   ```

2. 检查头文件是否存在：
   ```bash
   ls -la /usr/local/include/spdlog
   ls -la /usr/local/include/nlohmann
   ```

## 自定义安装路径

如果您想修改库的安装路径，可以编辑 `build_third_party.sh` 脚本中的 `INSTALL_PREFIX` 变量：

```bash
# 修改为您希望的安装路径
INSTALL_PREFIX="/path/to/your/custom/location"
```

修改后，您需要确保在 CMake 中正确指定这些库的位置：

```cmake
# 指定自定义安装路径
list(APPEND CMAKE_PREFIX_PATH "/path/to/your/custom/location")

find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)
```

## 注意事项

1. 脚本需要 sudo 权限才能将库安装到系统目录
2. 构建完成后，您可以安全地从项目中删除第三方库的源代码目录
3. 如果您在构建或链接过程中遇到问题，请确保 CMake 版本兼容（spdlog 要求 CMake 3.10+，nlohmann_json 要求 CMake 3.13+）