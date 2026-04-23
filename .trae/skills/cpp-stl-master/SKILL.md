---
name: "cpp-stl-master"
description: "C++ STL 标准库专项训练，覆盖容器、算法、迭代器。Invoke when user requests STL training, container usage, algorithm practice, or iterator exercises."
---

# C++ STL 标准库专项训练

## 概述

STL 是 C++ 编程的核心工具集。本 Skill 通过系统训练让学习者掌握容器、算法、迭代器的正确使用方法，理解底层原理和性能特征。

## 训练模块

### Module 3.1: 序列式容器（第 1-2 周）

#### vector - 动态数组
- 初始化方式
- 增删改查操作
- capacity vs size
- 迭代器失效规则
- 性能特征

**练习题目：**
1. 实现动态数组基本操作
2. 遍历与查找元素
3. 插入与删除性能测试
4. 迭代器失效场景分析

#### list - 双向链表
- 与 vector 的对比
- splice、merge、sort 操作
- 适用场景分析

#### deque - 双端队列
- 与 vector 的区别
- 队列和栈的实现

**每日任务：**
- 手写 5 个容器操作示例
- 完成 8 道容器应用题
- 对比不同容器的性能差异

### Module 3.2: 关联式容器（第 3 周）

#### map / unordered_map
- 键值对存储
- 查找效率对比
- 自定义键类型
- 迭代器使用

#### set / unordered_set
- 唯一元素集合
- 集合运算
- 适用场景

**练习题目：**
1. 词频统计工具
2. 学生成绩管理系统
3. 去重与查找程序
4. 自定义比较器

### Module 3.3: 算法库（第 4 周）

#### 常用算法
- sort, stable_sort
- find, find_if
- for_each, transform
- copy, move
- remove, erase

#### lambda 表达式
- 基本语法
- 捕获列表
- 与算法配合使用

**练习题目：**
1. 自定义排序规则
2. 数据过滤与转换
3. 复合算法应用
4. 性能优化实践

### Module 3.4: 迭代器（第 5 周）

- 迭代器分类
- begin/end, rbegin/rend
- 迭代器适配器
- 迭代器失效场景

## 训练方法

### 1. 容器对比分析
对每个操作，对比不同容器：
- 时间复杂度
- 空间复杂度
- 迭代器失效规则
- 适用场景

### 2. 性能测试
- 使用 chrono 测量执行时间
- 大数据量测试
- 内存使用分析

### 3. AI 反馈提示词

```
"请检查这段 STL 代码：
1. 迭代器是否失效
2. 容器选择是否合理
3. 算法复杂度是否最优
4. 是否有更现代的写法"
```

```
"请对比 vector 和 list 在这个场景下的性能差异"
```

## 达标标准

- 能根据需求选择合适容器
- 理解迭代器失效规则
- 熟练使用常用算法
- 能解释容器底层实现

## 练习模板

```
training/phase3_stl/
  ├── CMakeLists.txt
  ├── exercise_vector/
  │   ├── src/main.cpp
  │   └── CMakeLists.txt
  ├── exercise_map/
  │   ├── src/main.cpp
  │   └── CMakeLists.txt
  └── ...
```
