---
name: "cpp-diagnostic-assessor"
description: "C++ 能力诊断评估系统，定位知识盲区并生成个性化补洞计划。Invoke when user requests skill assessment, knowledge gap analysis, or before starting training to identify weak areas."
---

# C++ 能力诊断评估系统

## 用途
在开始训练前诊断学习者的真实水平，找出知识盲区，生成个性化的 12 周补洞计划。

## 诊断维度（8 大能力域）

### 1. 语言基础与现代语法
**诊断问题：**
- 能否解释值类别（lvalue, rvalue, xvalue, prvalue）？
- 能否说明拷贝/移动构造的触发时机？
- 能否写出异常安全的赋值操作符？
- 是否理解模板推导规则？
- 是否了解 C++20 concept 的用途？

### 2. 标准库与泛型能力
**诊断问题：**
- 何时选择 vector vs list vs deque？
- 迭代器失效规则是否清楚？
- 能否正确使用 optional/variant/expected？
- 是否了解 ranges 的优势？

### 3. 内存与性能
**诊断问题：**
- 能否解释对象内存布局？
- 是否理解缓存局部性的影响？
- 能否写出零拷贝的数据传递？
- 是否使用过 profiling 工具？

### 4. 并发与并行
**诊断问题：**
- atomic 的 memory_order 理解程度？
- 能否避免死锁和竞态条件？
- 是否理解无锁数据结构？
- jthread vs thread 的区别？

### 5. 系统工程
**诊断问题：**
- CMake 模块化构建经验？
- 链接模型理解（静态/动态）？
- ABI 兼容性问题处理？
- 跨平台编译经验？

### 6. 架构设计
**诊断问题：**
- 接口边界如何划分？
- 依赖注入实践经验？
- 错误处理策略选择？
- 组件化设计能力？

### 7. 领域落地
**诊断问题：**
- 网络编程经验？
- 低延迟系统经验？
- 存储引擎理解？
- 音视频/嵌入式/AI推理？

### 8. 规范与判断
**诊断问题：**
- C++ Core Guidelines 熟悉度？
- 代码审查能力？
- 复杂度控制意识？
- 可维护性判断力？

## 诊断流程

### Step 1: 问卷评估（30 分钟）
向学习者提出 20 个问题，每个能力域 2-3 个：

```
请回答以下问题（简要即可）：
1. 解释 std::move 的作用，它真的"移动"了吗？
2. vector 的 capacity 和 size 有什么区别？
3. 什么是 RAII？举一个你使用过的例子
4. atomic<int> 和 int 加锁有什么区别？
...
```

### Step 2: 代码审查诊断
提供 3 段有问题的代码，让学习者找出问题：

```cpp
// 诊断代码 1：内存管理问题
void process() {
    int* data = new int[100];
    if (condition) return;
    delete[] data;
}

// 诊断代码 2：并发问题
std::vector<int> shared_data;
void writer() { shared_data.push_back(1); }
void reader() { auto x = shared_data[0]; }

// 诊断代码 3：异常安全问题
void transfer(Account& from, Account& to, double amount) {
    from.withdraw(amount);
    if (fail_condition) throw std::runtime_error("fail");
    to.deposit(amount);
}
```

### Step 3: 生成能力地图
根据回答生成雷达图式的能力评估：

```
能力评估报告：
- 语言基础：■■■■□ (4/5) - 建议补洞：值类别、模板推导
- 标准库：   ■■■□□ (3/5) - 建议补洞：ranges、optional
- 内存性能： ■■□□□ (2/5) - 建议补洞：对象布局、profiling
...
```

### Step 4: 生成个性化 12 周计划
根据弱点调整训练重点：

```
个性化计划：
Week 1-2: 重点补洞 - 值类别、对象模型
Week 3-4: 重点补洞 - 内存布局、RAII 深入
Week 5-6: 强化 - 并发编程
Week 7-8: 强化 - 标准库现代化
...
```

## 输出格式

诊断完成后输出：
1. **能力雷达图**（8 个维度评分）
2. **知识缺口清单**（按优先级排序）
3. **12 周补洞计划**（个性化调整）
4. **推荐学习资源**（书籍、视频、文章）
5. **首周每日任务**（立即可以开始）

## 使用示例

用户输入：
```
"请对我进行 C++ 能力诊断"
```

AI 执行：
1. 发送 20 个诊断问题
2. 等待用户回答
3. 提供 3 段诊断代码
4. 分析回答生成报告
5. 输出个性化计划
