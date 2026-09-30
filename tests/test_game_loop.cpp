#include "test_framework.h"

#include "engine/core/game_loop.h"

using namespace engine;

TEST_CASE("game loop: fixed steps accumulate correctly") {
    int steps = 0;
    int frames = 0;
    GameLoop loop({1.0 / 60.0, 8}, {
        [&](f64) { ++steps; },
        [&](f64) { ++frames; },
        nullptr,
    });
    // 30 FPS 渲染帧 → 每帧应执行 2 个 60Hz 固定步
    loop.runHeadless(30, 1.0 / 30.0);
    CHECK(frames == 30);
    CHECK(steps == 60 || steps == 59); // 浮点累积误差可能少一步
    CHECK_NEAR(loop.simulationTime(), 1.0, 1.0 / 60.0 + 1e-9);
}

TEST_CASE("game loop: spiral-of-death clamp") {
    int steps = 0;
    GameLoop loop({0.01, 4}, {[&](f64) { ++steps; }, nullptr, nullptr});
    CHECK(loop.tick(10.0) == 4); // 卡顿 10 秒也只追 4 步
    CHECK(loop.tick(0.0) == 0);  // 多余时间被丢弃
    CHECK(steps == 4);
}

TEST_CASE("game loop: render alpha in [0,1)") {
    double lastAlpha = -1.0;
    GameLoop loop({0.1, 8}, {nullptr, nullptr, [&](f64 a) { lastAlpha = a; }});
    loop.tick(0.25);
    CHECK_NEAR(lastAlpha, 0.5, 1e-9);
}

TEST_CASE("game loop: deterministic across frame rates") {
    auto simulate = [](f64 frameDt, u64 frames) {
        f64 position = 0.0;
        f64 velocity = 0.0;
        GameLoop loop({1.0 / 120.0, 16}, {
            [&](f64 dt) {
                velocity += 9.8 * dt;
                position += velocity * dt;
            },
            nullptr,
            nullptr,
        });
        loop.runHeadless(frames, frameDt);
        return loop.simulationStepCount() == 120 ? position : -1.0;
    };
    // 1 秒模拟：用 1/60 帧率 与 1/40 帧率跑，固定步数相同 → 结果逐位一致
    CHECK(simulate(1.0 / 60.0 + 1e-12, 60) == simulate(1.0 / 40.0 + 1e-12, 40));
}
