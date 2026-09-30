// Sandbox：把各个引擎模块串起来的最小示例。
// 默认无头运行固定帧数；加 --console 进入交互式命令行（开发者控制台）。
#include "engine/core/game_loop.h"
#include "engine/core/log.h"
#include "engine/ecs/world.h"
#include "engine/tools/command_registry.h"

#include <iostream>
#include <string>

using namespace engine;

namespace {

struct Transform {
    f32 x = 0, y = 0;
};
struct Velocity {
    f32 dx = 0, dy = 0;
};

// 系统 = 普通函数
void movementSystem(ecs::World& world, f64 dt) {
    world.each<Transform, Velocity>([dt](ecs::Entity, Transform& t, Velocity& v) {
        t.x += v.dx * static_cast<f32>(dt);
        t.y += v.dy * static_cast<f32>(dt);
    });
}

} // namespace

int main(int argc, char** argv) {
    const bool interactive = argc > 1 && std::string(argv[1]) == "--console";

    ecs::World world;
    for (int i = 0; i < 3; ++i) {
        ecs::Entity e = world.create();
        world.add<Transform>(e);
        world.add<Velocity>(e, 1.0f * (i + 1), 0.5f);
    }

    GameLoop loop({1.0 / 60.0, 8}, {
        [&](f64 dt) { movementSystem(world, dt); },
        nullptr,
        nullptr,
    });

    tools::CommandRegistry commands;
    commands.add("step", "推进 N 帧（默认 1），例如：step 60", [&](const tools::CommandArgs& args) {
        const u64 frames = args.empty() ? 1 : std::stoull(args[0]);
        loop.runHeadless(frames, 1.0 / 60.0);
        return tools::CommandResult::success("t=" + std::to_string(loop.simulationTime()));
    });
    commands.add("entities", "列出所有实体的位置", [&](const tools::CommandArgs&) {
        std::string out;
        world.each<Transform>([&](ecs::Entity e, Transform& t) {
            out += "entity " + std::to_string(e.index()) + ": (" + std::to_string(t.x) + ", " +
                   std::to_string(t.y) + ")\n";
        });
        return tools::CommandResult::success(out);
    });
    commands.add("spawn", "生成实体：spawn <dx> <dy>", [&](const tools::CommandArgs& args) {
        if (args.size() != 2) {
            return tools::CommandResult::failure("usage: spawn <dx> <dy>");
        }
        ecs::Entity e = world.create();
        world.add<Transform>(e);
        world.add<Velocity>(e, std::stof(args[0]), std::stof(args[1]));
        return tools::CommandResult::success("spawned entity " + std::to_string(e.index()));
    });

    if (!interactive) {
        log::info("sandbox", "running 120 frames headless...");
        std::cout << commands.execute("step 120").output << "\n";
        std::cout << commands.execute("entities").output;
        return 0;
    }

    log::info("sandbox", "developer console — type 'help', 'quit' to exit");
    std::string line;
    while (std::cout << "> " && std::getline(std::cin, line)) {
        if (line == "quit" || line == "exit") {
            break;
        }
        try {
            auto result = commands.execute(line);
            std::cout << (result.ok ? "" : "error: ") << result.output << "\n";
        } catch (const std::exception& ex) {
            std::cout << "error: " << ex.what() << "\n";
        }
    }
    return 0;
}
