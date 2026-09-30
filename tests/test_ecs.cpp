#include "test_framework.h"

#include "engine/ecs/world.h"

using namespace engine;
using namespace engine::ecs;

namespace {
struct Position {
    float x = 0, y = 0;
};
struct Velocity {
    float dx = 0, dy = 0;
};
} // namespace

TEST_CASE("ecs: create, destroy, stale handles") {
    World world;
    Entity a = world.create();
    Entity b = world.create();
    CHECK(world.isAlive(a));
    CHECK(world.aliveCount() == 2);

    world.destroy(a);
    CHECK(!world.isAlive(a));
    CHECK(world.aliveCount() == 1);

    // 索引被复用，但代数不同，旧句柄依旧无效。
    Entity c = world.create();
    CHECK(c.index() == a.index());
    CHECK(c != a);
    CHECK(!world.isAlive(a));
    CHECK(world.isAlive(b));
}

TEST_CASE("ecs: add/get/remove components") {
    World world;
    Entity e = world.create();
    world.add<Position>(e, 1.0f, 2.0f);
    CHECK(world.has<Position>(e));
    CHECK(!world.has<Velocity>(e));
    CHECK(world.get<Position>(e)->y == 2.0f);

    world.remove<Position>(e);
    CHECK(!world.has<Position>(e));

    world.add<Position>(e, 3.0f, 4.0f);
    world.destroy(e);
    CHECK(world.get<Position>(e) == nullptr);
}

TEST_CASE("ecs: each iterates only matching entities") {
    World world;
    Entity moving = world.create();
    world.add<Position>(moving);
    world.add<Velocity>(moving, 1.0f, 0.5f);

    Entity still = world.create();
    world.add<Position>(still, 10.0f, 10.0f);

    int visited = 0;
    world.each<Position, Velocity>([&](Entity, Position& p, Velocity& v) {
        p.x += v.dx;
        p.y += v.dy;
        ++visited;
    });
    CHECK(visited == 1);
    CHECK(world.get<Position>(moving)->x == 1.0f);
    CHECK(world.get<Position>(still)->x == 10.0f);
}

TEST_CASE("ecs: swap-and-pop keeps remaining data intact") {
    World world;
    Entity e[3];
    for (int i = 0; i < 3; ++i) {
        e[i] = world.create();
        world.add<Position>(e[i], float(i), 0.0f);
    }
    world.destroy(e[0]);
    CHECK(world.get<Position>(e[1])->x == 1.0f);
    CHECK(world.get<Position>(e[2])->x == 2.0f);
}
