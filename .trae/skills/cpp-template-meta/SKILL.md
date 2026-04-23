---
name: "cpp-template-meta"
description: "C++ 模板与元编程训练，涵盖函数模板、类模板、模板特化、SFINAE、概念、CRTP。Invoke when user requests template programming training, metaprogramming exercises, or generic component design practice."
---

# C++ 模板与元编程训练

## 概述
模板是 C++ 泛型编程的核心，也是通向专家级的必经之路。本 Skill 通过渐进式训练让学习者掌握从基础模板到高级元编程的技术。

## 训练模块

### Module 1: 模板基础（第 1-2 天）
- 函数模板语法
- 模板推导
- 显式模板参数
- 非类型模板参数

### Module 2: 类模板（第 3-4 天）
- 类模板定义
- 模板成员函数
- 模板友元
- 模板分离编译

### Module 3: 模板特化（第 5-6 天）
- 全特化
- 偏特化
- 特化优先级
- 应用场景

### Module 4: SFINAE（第 7-8 天）
- Substitution Failure Is Not An Error
- std::enable_if
- 类型萃取
- 检测惯用法

### Module 5: C++20 Concept（第 9-10 天）
- Concept 定义
- requires 子句
- 约束表达
- 相比 SFINAE 的优势

### Module 6: CRTP（第 11-12 天）
- Curiously Recurring Template Pattern
- 静态多态
- 混入模式
- 性能优势

## 每日练习

### Day 1: 函数模板
```cpp
// 练习：实现泛型 swap 函数
// 练习：实现泛型 max 函数，支持自定义比较器
// 练习：实现泛型数组求和
```

### Day 2: 模板推导
```cpp
// 练习：分析以下代码推导结果
template<typename T>
void f(T, T*);

int x = 1;
f(x, &x);  // T = ?
```

### Day 3: 类模板
```cpp
// 练习：实现泛型 Stack<T>
// 练习：实现泛型 Pair<T1, T2>
```

### Day 4-12: 后续练习
详见 training 目录下对应练习文件。

## 项目实战
- 泛型容器（Vector, List, Map）
- 泛型算法库
- 类型安全枚举
- 编译期计算（Fibonacci, 阶乘）

## 达标标准
- 能独立编写泛型组件
- 理解 SFINAE 原理
- 能使用 Concept 约束模板
- 掌握 CRTP 应用场景
