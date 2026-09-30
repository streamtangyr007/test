# 书单与资源

## 主线教材
- **《游戏引擎架构》Game Engine Architecture** — Jason Gregory（顽皮狗）。本项目的骨架。
  建议第一遍通读第1、3、6、8、15、16章，其他章节在对应阶段再精读。

## 配套书籍（按阶段）

| 阶段 | 书 | 为什么读 |
|---|---|---|
| 0–1 | **《游戏编程模式》Game Programming Patterns** — Robert Nystrom（[免费在线版](https://gameprogrammingpatterns.com/)） | 游戏循环、组件、事件队列、对象池等模式，短小易读，最适合入门 |
| 0–1 | *Game Engine Black Book: Wolfenstein 3D / DOOM* — Fabien Sanglard | 看经典引擎在极限硬件上怎么设计 |
| 2 | **《3D数学基础：图形和游戏开发》** 3D Math Primer — Dunn & Parberry（[免费在线版](https://gamemath.com/)） | 最友好的游戏数学入门 |
| 2, 4 | *Foundations of Game Engine Development* Vol.1 数学 / Vol.2 渲染 — Eric Lengyel | 简洁、面向引擎实现 |
| 4 | **LearnOpenGL**（[英文](https://learnopengl.com/) / [中文](https://learnopengl-cn.github.io/)） | 渲染入门的最佳实践教程 |
| 4 | *Real-Time Rendering* 第4版 — Akenine-Möller 等 | 渲染"百科全书"，当参考书查 |
| 4 | *Physically Based Rendering* — Pharr 等（[免费在线版](https://pbr-book.org/)） | 进阶：PBR 理论 |
| 6 | *Data-Oriented Design* — Richard Fabian（[免费在线版](https://www.dataorienteddesign.com/dodbook/)） | ECS 背后的思想 |
| 7 | *Real-Time Collision Detection* — Christer Ericson | 碰撞检测圣经 |
| 7 | *Game Physics Engine Development* — Ian Millington | 从零写物理引擎 |
| 11 | *C++ Concurrency in Action* — Anthony Williams | 多线程与任务系统 |

## 文章
- Glenn Fiedler, *Fix Your Timestep!* —— 本仓库游戏循环的直接依据
- Glenn Fiedler, *Integration Basics* —— 物理积分入门
- Mike Acton, *Data-Oriented Design and C++*（CppCon 2014 演讲）

## 视频 / 系列
- **TheCherno — Game Engine 系列**（Hazel 引擎）：从零写引擎的完整实录，与本路线高度契合
- **Handmade Hero** — Casey Muratori：不用任何库从零写游戏，理解底层
- 3Blue1Brown《线性代数的本质》：阶段 2 前看

## 值得阅读源码的开源引擎
| 引擎 | 看什么 |
|---|---|
| [Hazel](https://github.com/TheCherno/Hazel) | 结构清晰的教学型引擎 |
| [EnTT](https://github.com/skypjack/entt) | 工业级 sparse-set ECS（我们 ECS 的升级方向） |
| [flecs](https://github.com/SanderMertens/flecs) | archetype ECS，文档极好 |
| [Godot](https://github.com/godotengine/godot) | 完整商业级开源引擎，场景树模型 |
| [sokol](https://github.com/floooh/sokol) | 极简跨平台头文件库（图形/音频/窗口） |
| [bgfx](https://github.com/bkaradzic/bgfx) | 跨 API 渲染抽象层 |

## 推荐的第三方库（到对应阶段再引入）
SDL3（窗口/输入）· glad（OpenGL 加载）· stb_image（图片）· nlohmann/json（场景文件）·
Dear ImGui（调试 UI）· sol2 + Lua（脚本）· miniaudio（音频）· Box2D / Jolt（物理）· Tracy（性能剖析）
