#include "engine/core/game_loop.h"

#include "engine/core/assert.h"

#include <chrono>

namespace engine {

GameLoop::GameLoop(GameLoopConfig config, GameLoopCallbacks callbacks)
    : m_config(config), m_callbacks(std::move(callbacks)) {
    ENGINE_ASSERT(m_config.fixedDeltaSeconds > 0.0, "fixedDeltaSeconds must be positive");
    ENGINE_ASSERT(m_config.maxStepsPerFrame > 0, "maxStepsPerFrame must be positive");
}

u32 GameLoop::tick(f64 realDeltaSeconds) {
    if (realDeltaSeconds < 0.0) {
        realDeltaSeconds = 0.0;
    }
    const f64 dt = m_config.fixedDeltaSeconds;
    m_accumulator += realDeltaSeconds;

    u32 steps = 0;
    while (m_accumulator >= dt && steps < m_config.maxStepsPerFrame) {
        if (m_callbacks.fixedUpdate) {
            m_callbacks.fixedUpdate(dt);
        }
        m_accumulator -= dt;
        ++steps;
        ++m_stepCount;
    }
    // 追不上时丢弃多余时间，避免死亡螺旋（模拟会"变慢"而不是卡死）。
    if (steps == m_config.maxStepsPerFrame && m_accumulator >= dt) {
        m_accumulator = 0.0;
    }

    if (m_callbacks.update) {
        m_callbacks.update(realDeltaSeconds);
    }
    if (m_callbacks.render) {
        m_callbacks.render(m_accumulator / dt);
    }
    ++m_frameCount;
    return steps;
}

void GameLoop::runRealtime(const std::function<bool()>& shouldQuit) {
    using Clock = std::chrono::steady_clock;
    auto previous = Clock::now();
    while (!shouldQuit()) {
        const auto now = Clock::now();
        const f64 delta = std::chrono::duration<f64>(now - previous).count();
        previous = now;
        tick(delta);
    }
}

void GameLoop::runHeadless(u64 frames, f64 frameDeltaSeconds) {
    for (u64 i = 0; i < frames; ++i) {
        tick(frameDeltaSeconds);
    }
}

} // namespace engine
