# CLAUDE.md — 给 Claude 的项目说明

这是一个**以学习为目的、按《游戏引擎架构》(Jason Gregory) 逐章搭建**的 C++ 游戏引擎。
仓库主人正在入门引擎开发：请用**中文**交流，改动时顺带解释背后的概念，并注明对应书中章节。

## 构建与测试

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
./build/tests/engine_tests            # 全部测试
./build/tests/engine_tests ecs        # 只跑名称含 "ecs" 的测试
./build/sandbox/sandbox               # 无头运行示例
./build/sandbox/sandbox --console     # 交互式开发者控制台
```

每次改动后都必须：编译无警告 + 全部测试通过。

## 目录结构

| 路径 | 内容 |
|---|---|
| `engine/include/engine/<module>/` | 公共头文件（按模块分目录） |
| `engine/src/<module>/` | 实现文件 |
| `sandbox/` | 演示/试验用可执行程序 |
| `tests/` | 单元测试（零依赖迷你框架 `test_framework.h`） |
| `docs/` | 学习路线、架构说明、AI 协作指南、书单 |

## 架构规则（重要）

- 分层依赖只能**自上而下**：`game → gameplay(ecs) → resources/render/physics/audio → core/memory → platform`。
  下层模块绝不 `#include` 上层模块。详见 `docs/02-architecture.md`。
- 引擎必须能**无头(headless)运行**：逻辑不依赖窗口/GPU，保证测试和 AI 自动化可以跑。
- 逻辑/物理在 `GameLoop` 的**固定步长** `fixedUpdate` 中推进，保持确定性。
- 新的可操作功能尽量同时注册为**控制台命令**（`tools::CommandRegistry`），返回结构化 `CommandResult`——
  这是未来接入 AI 代理（MCP 等）的统一入口。
- 引入第三方库前先询问仓库主人；优先头文件库或 CMake FetchContent。

## 代码约定

- C++20，命名空间 `engine::<module>`；类型 `PascalCase`，函数/变量 `camelCase`，成员 `m_` 前缀，常量 `kName`。
- 基础类型用 `engine/core/types.h`（`u32`, `f32`…）；检查用 `ENGINE_ASSERT`；日志用 `engine::log`。
- 每个头文件顶部一行注释说明用途，并标注对应《游戏引擎架构》章节。
- 格式遵循 `.clang-format`。
- 新模块 = 头文件 + 实现 + `tests/test_<module>.cpp` + 在 `docs/01-roadmap.md` 勾选进度。

## 工作方式

- 当前进度与下一步见 `docs/01-roadmap.md`；开始任务前先读它。
- 一次只做一个里程碑中的一小步；先写/改测试描述期望行为，再实现。
- 解释设计取舍时，给出"书里怎么说 / 我们为什么这样简化 / 以后怎么升级"。
