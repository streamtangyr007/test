# MiniEngine — 跟着《游戏引擎架构》用 Claude 搭一个自己的引擎

一个以学习为目的的 C++20 游戏引擎骨架，按 Jason Gregory《游戏引擎架构》(*Game Engine Architecture*)
的分层架构逐章实现，并从第一天起就为 **AI 辅助开发**（Claude Code）设计：
无头可运行、确定性模拟、测试驱动、命令化接口。

## 快速开始

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
./build/tests/engine_tests          # 运行单元测试
./build/sandbox/sandbox --console   # 打开开发者控制台，输入 help
```

需要：CMake ≥ 3.20，支持 C++20 的编译器（GCC 11+ / Clang 14+ / MSVC 2022）。当前无任何第三方依赖。

## 已实现（里程碑 M0–M1）

| 模块 | 文件 | 书中章节 |
|---|---|---|
| 基础类型 / 断言 / 日志 | `engine/core/` | 第3章、第10章 |
| 栈分配器 / 池分配器 | `engine/memory/` | 第6章 |
| 固定步长游戏循环 | `engine/core/game_loop.h` | 第8章 |
| 极简 ECS | `engine/ecs/world.h` | 第16章 |
| 命令注册表（控制台 / AI 接口） | `engine/tools/command_registry.h` | 第10章 |

## 文档

1. [学习路线图](docs/01-roadmap.md) —— 从这里开始，按里程碑推进
2. [引擎架构](docs/02-architecture.md) —— 分层结构与依赖规则
3. [用 Claude 开发的工作流](docs/03-ai-workflow.md) —— 提示词模板与 AI 友好设计
4. [书单与资源](docs/04-reading-list.md)
