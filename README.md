# C++ 专家级训练项目

## 项目简介

这是一个**系统化的 C++ 专家级训练项目**，通过 12 周高强度刻意练习，帮助有编程基础的开发者从"会写代码"进阶到"能设计系统的 C++ 专家"。

## 核心特点

- **12 个专业 Skills** - 覆盖 C++ 全能力域（语言、标准库、内存、并发、架构、性能等）
- **84 天详细计划** - 每天学什么、做什么、怎么用 AI，全部规划好
- **AI 辅助训练** - 5 个 AI 角色（诊断师、讲解员、审稿人、出题人、复盘器）
- **项目驱动** - 3 个递进项目，从工具箱到领域级系统
- **科学复盘** - 知识卡片 + 间隔重复算法，形成长期记忆

## 从何处开始

### 第一步：能力诊断（Day 1）

开始训练前，先诊断你的真实水平：

```
"请对我进行 C++ 能力诊断"
```

AI 会：
1. 提出 20 个诊断问题，覆盖 8 大能力域
2. 提供 3 段有 bug 的代码让你分析
3. 生成能力雷达图和知识缺口清单
4. 输出个性化的 12 周补洞计划

### 第二步：开始每日训练（Day 2-84）

按照 `docs/DAILY_PLAN.md` 的安排，每天完成：
- **早上 45min** - 学习概念
- **中午 30min** - AI 问答
- **晚上 90min** - 代码实现

### 第三步：周复盘（每周日）

使用 `docs/WEEKLY_REVIEW_TEMPLATE.md` 进行周复盘：
- 整理错误到知识卡片
- 制定间隔复习计划
- 总结本周成长

## 训练流程

```
开始
  ↓
[诊断评估] ← 使用 cpp-diagnostic-assessor Skill
  ↓
[12 周训练] ← 按 DAILY_PLAN.md 每天训练
  ↓
[每周产出] ← 练习代码、项目、报告
  ↓
[阶段评估] ← 对比初始能力地图
  ↓
[领域项目] ← Week 11-12 完成专家级项目
  ↓
专家级 ✓
```

## 项目结构

```
seekCpp/
├── .trae/
│   ├── rules/              # 代码规范
│   └── skills/             # 12 个训练 Skills
│       ├── cpp-diagnostic-assessor/   # 能力诊断
│       ├── cpp-basics-trainer/        # 基础语法
│       ├── cpp-stl-master/            # STL 训练
│       ├── cpp-memory-raii/           # 内存管理
│       ├── cpp-template-meta/         # 模板元编程
│       ├── cpp-concurrency/           # 并发编程
│       ├── cpp-performance/           # 性能优化
│       ├── cpp-modern-features/       # 现代特性
│       ├── cpp-system-engineering/    # 系统工程
│       ├── cpp-architecture/          # 架构设计
│       ├── cpp-network-programming/   # 网络编程
│       └── cpp-code-reviewer/         # 代码审查
├── training/               # 练习代码目录
│   ├── week01_variables/   # Week 1 练习
│   ├── week01_control_flow/
│   ├── week01_functions/
│   └── ...                 # 后续周练习
├── docs/                   # 训练文档
│   ├── MASTER_GUIDE.md            # 全景指南
│   ├── DAILY_PLAN.md              # 84 天详细计划
│   ├── PROMPT_LIBRARY.md          # AI 提示词库
│   ├── KNOWLEDGE_CARD_TEMPLATE.md # 知识卡片模板
│   ├── ERROR_LOG_TEMPLATE.md      # 错误日志模板
│   └── WEEKLY_REVIEW_TEMPLATE.md  # 周复盘模板
├── CMakeLists.txt          # 顶层构建配置
├── .clang-format           # 代码格式化配置
└── README.md               # 本文件
```

## 文档导航

| 文档 | 用途 | 何时使用 |
|------|------|----------|
| [MASTER_GUIDE.md](docs/MASTER_GUIDE.md) | 训练全景指南 | 开始前阅读，了解全貌 |
| [DAILY_PLAN.md](docs/DAILY_PLAN.md) | 84 天详细计划 | 每天对照执行 |
| [PROMPT_LIBRARY.md](docs/PROMPT_LIBRARY.md) | AI 提示词库 | 需要 AI 辅助时查阅 |
| [KNOWLEDGE_CARD_TEMPLATE.md](docs/KNOWLEDGE_CARD_TEMPLATE.md) | 知识卡片模板 | 遇到错误/学到重点时填写 |
| [ERROR_LOG_TEMPLATE.md](docs/ERROR_LOG_TEMPLATE.md) | 错误日志模板 | 每天记录错误 |
| [WEEKLY_REVIEW_TEMPLATE.md](docs/WEEKLY_REVIEW_TEMPLATE.md) | 周复盘模板 | 每周日复盘使用 |

## 12 周训练概览

| 周期 | 重点 | 产出 |
|------|------|------|
| Week 1-2 | 诊断补洞：值类别、对象模型、异常安全 | 能力地图 + 50 个练习 |
| Week 3-4 | 标准库现代化：ranges、optional、variant | 2 个重写组件 |
| Week 5-6 | 内存与性能：RAII、benchmark、缓存优化 | 性能优化报告 |
| Week 7-8 | 并发编程：thread、atomic、任务队列 | 并发服务原型 |
| Week 9-10 | 架构设计：接口、组件、测试体系 | 中型项目骨架 |
| Week 11-12 | 领域项目：完整系统交付 | 专家级项目 |

## AI 使用方式

### 5 个 AI 角色

| 角色 | 功能 | 示例 |
|------|------|------|
| **诊断师** | 定位知识盲区 | "问我 20 个问题找出我的 C++ 盲区" |
| **讲解员** | 四版本讲解概念 | "用直觉/正式/源码/反例讲 [概念]" |
| **审稿人** | 代码审查打分 | "从 5 个维度审查我的代码" |
| **出题人** | 生成递进练习 | "给我从易到难的 7 天训练" |
| **复盘器** | 知识卡整理 | "把这周错误整理成知识卡片" |

完整的提示词模板见：[docs/PROMPT_LIBRARY.md](docs/PROMPT_LIBRARY.md)

## 如何编译练习代码

```bash
# 创建构建目录
mkdir build && cd build

# 配置项目
cmake ..

# 编译
cmake --build .

# 运行练习
./bin/exercise_name
```

## 达标标准

完成 12 周训练后，你应该能：

- [ ] 解释一个特性的设计动机和代价
- [ ] 不查资料写出安全、可维护、可测试的实现
- [ ] 在性能、抽象、可读性之间做出合理权衡
- [ ] 通过代码审查指出别人方案里真正会出问题的地方
- [ ] 在一个新领域里快速建立知识图谱并落地一个可用系统

## 开始训练

准备好后，输入以下指令开始：

```
"请对我进行 C++ 能力诊断"
```

或者直接从 Day 1 开始：

```
"我想开始 Week 1 Day 1 的训练"
```
