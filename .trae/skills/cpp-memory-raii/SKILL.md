---
name: "cpp-memory-raii"
description: "C++ 内存管理与 RAII 训练，涵盖智能指针、资源管理、异常安全。Invoke when user requests RAII training, smart pointer practice, memory management exercises, or exception safety review."
---

# C++ 内存管理与 RAII 训练

## 概述

RAII（Resource Acquisition Is Initialization）是 C++ 资源管理的核心范式。本 Skill 通过系统训练让学习者掌握智能指针、值语义、异常安全等高级主题。

## 训练模块

### Module 4.1: 内存模型基础（第 1 周）

- 栈内存与堆内存
- new/delete 配对使用
- 内存泄漏检测
- 深拷贝与浅拷贝
- 拷贝构造函数

**练习题目：**
1. 手动管理动态数组
2. 实现深拷贝的类
3. 内存泄漏场景分析
4. 拷贝构造函数编写

### Module 4.2: RAII 范式（第 2 周）

- RAII 原理与应用
- 自定义资源管理类
- 文件句柄管理
- 锁的 RAII 封装

**练习题目：**
1. 实现 File 类（RAII 封装文件操作）
2. 实现 LockGuard 类
3. 数据库连接管理
4. 自定义智能指针

### Module 4.3: 智能指针（第 3 周）

#### unique_ptr
- 独占所有权
- move 语义
- 自定义删除器
- 数组支持

#### shared_ptr
- 共享所有权
- 引用计数
- weak_ptr 打破循环引用
- 性能开销分析

**练习题目：**
1. 使用 unique_ptr 重构裸指针代码
2. 实现对象工厂返回 unique_ptr
3. shared_ptr 管理共享资源
4. weak_ptr 观察对象生命周期

### Module 4.4: 异常安全（第 4 周）

- try-catch 基础
- 异常安全级别
  - 基本保证
  - 强保证
  - 无抛异常保证
- RAII 与异常安全
- noexcept 修饰符

**练习题目：**
1. 实现异常安全的赋值操作符
2. 事务性操作设计
3. 异常安全等级分析
4. 资源回滚机制

### Module 4.5: 值语义与移动语义（第 5 周）

- 左值与右值
- std::move
- 移动构造函数
- 完美转发
- std::forward

**练习题目：**
1. 实现可移动但不可拷贝的类
2. 性能对比：拷贝 vs 移动
3. 工厂函数返回优化
4. 模板完美转发

## 训练方法

### 1. 代码重构训练
- 提供有问题的旧代码
- 使用 RAII 重构
- 对比重构前后差异
- 分析资源安全性

### 2. 边界条件测试
- 构造异常场景
- 测试资源释放
- 验证无泄漏

### 3. AI 反馈提示词

```
"请审查这段代码的资源管理：
1. 是否有内存泄漏风险
2. 是否应该使用智能指针
3. 异常发生时资源是否安全
4. 拷贝/移动语义是否正确"
```

```
"请把这段代码重写成现代 C++ 风格，
使用 RAII 和智能指针"
```

## 达标标准

- 能独立编写资源安全的代码
- 理解智能指针选择策略
- 掌握异常安全设计
- 能解释移动语义原理

## 练习模板

```
training/phase4_memory/
  ├── CMakeLists.txt
  ├── exercise_raii/
  │   ├── src/main.cpp
  │   └── CMakeLists.txt
  ├── exercise_smart_ptr/
  │   ├── src/main.cpp
  │   └── CMakeLists.txt
  └── ...
```
