---
name: "cpp-concurrency"
description: "C++ 并发编程训练，涵盖线程、互斥锁、原子操作、条件变量、任务队列、无锁编程。Invoke when user requests concurrency training, multithreading exercises, or parallel programming practice."
---

# C++ 并发编程训练

## 概述
并发编程是 C++ 专家级必备能力。本 Skill 系统训练线程管理、同步原语、原子操作、并发设计模式。

## 训练模块

### Module 1: 线程基础（第 1-3 天）
- std::thread 创建与管理
- 传递参数
- join vs detach
- jthread (C++20)

### Module 2: 互斥与锁（第 4-6 天）
- std::mutex
- std::lock_guard
- std::unique_lock
- std::scoped_lock (C++17)
- 死锁预防

### Module 3: 条件变量（第 7-8 天）
- std::condition_variable
- 生产者 - 消费者模型
- 虚假唤醒处理

### Module 4: 原子操作（第 9-11 天）
- std::atomic
- memory_order 语义
- acquire-release 模型
- 顺序一致性

### Module 5: 高级并发（第 12-14 天）
- 线程池设计
- 任务队列
- 并发数据结构
- 无锁编程基础

## 每日练习
详见 training/concurrency/ 目录

## 项目实战
- 线程池实现
- 并发缓存
- 日志系统
- 任务调度器

## 达标标准
- 能设计线程安全的组件
- 理解内存模型
- 能避免常见并发 bug
- 掌握并发设计模式
