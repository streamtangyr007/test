// 游戏循环 —— 对应《游戏引擎架构》第8章（游戏循环与实时模拟）。
// 采用"固定步长模拟 + 可变步长渲染"模型（见 Glenn Fiedler《Fix Your Timestep!》）：
//   - fixedUpdate 以恒定 dt 推进逻辑/物理 → 结果确定，便于测试、回放、AI 自动验证
//   - render 每帧调用一次，alpha 用于在两个模拟状态之间插值
// 时间来源被抽象出来，测试和无头(headless)模式可以注入假时间。
#pragma once

#include "engine/core/types.h"

#include <functional>

namespace engine {

struct GameLoopConfig {
    f64 fixedDeltaSeconds = 1.0 / 60.0;
    // 防止"死亡螺旋"：单帧最多追赶这么多个固定步。
    u32 maxStepsPerFrame = 8;
};

struct GameLoopCallbacks {
    std::function<void(f64 dt)> fixedUpdate;       // 逻辑/物理，dt 恒定
    std::function<void(f64 frameDt)> update;       // 每帧一次，可变 dt（输入、相机等）
    std::function<void(f64 alpha)> render;         // 每帧一次，alpha ∈ [0,1)
};

class GameLoop {
public:
    GameLoop(GameLoopConfig config, GameLoopCallbacks callbacks);

    // 推进一帧：realDeltaSeconds 是距离上一帧经过的真实时间。
    // 返回本帧执行的固定步数。
    u32 tick(f64 realDeltaSeconds);

    // 用实时时钟运行，直到 shouldQuit() 返回 true。
    void runRealtime(const std::function<bool()>& shouldQuit);

    // 无头模式：用恒定帧时间跑 frames 帧（不 sleep），用于测试/AI 自动化/服务器。
    void runHeadless(u64 frames, f64 frameDeltaSeconds);

    u64 frameCount() const { return m_frameCount; }
    u64 simulationStepCount() const { return m_stepCount; }
    f64 simulationTime() const { return static_cast<f64>(m_stepCount) * m_config.fixedDeltaSeconds; }

private:
    GameLoopConfig m_config;
    GameLoopCallbacks m_callbacks;
    f64 m_accumulator = 0.0;
    u64 m_frameCount = 0;
    u64 m_stepCount = 0;
};

} // namespace engine
