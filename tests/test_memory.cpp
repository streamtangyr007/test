#include "test_framework.h"

#include "engine/memory/pool_allocator.h"
#include "engine/memory/stack_allocator.h"

#include <cstdint>
#include <set>

using namespace engine;

TEST_CASE("stack allocator: aligned allocation and markers") {
    StackAllocator stack(1024);
    void* a = stack.allocate(3, 1);
    CHECK(a != nullptr);
    auto marker = stack.marker();

    void* b = stack.allocate(16, 16);
    CHECK(b != nullptr);
    CHECK(reinterpret_cast<std::uintptr_t>(b) % 16 == 0);
    CHECK(stack.used() > marker);

    stack.freeToMarker(marker);
    CHECK(stack.used() == marker);

    stack.clear();
    CHECK(stack.used() == 0);
}

TEST_CASE("stack allocator: returns nullptr when full") {
    StackAllocator stack(64);
    CHECK(stack.allocate(64, 1) != nullptr);
    CHECK(stack.allocate(1, 1) == nullptr);
}

TEST_CASE("pool allocator: allocate all, free, reuse") {
    PoolAllocator pool(sizeof(double) * 3, 4);
    std::set<void*> seen;
    for (int i = 0; i < 4; ++i) {
        void* p = pool.allocate();
        CHECK(p != nullptr);
        CHECK(pool.owns(p));
        seen.insert(p);
    }
    CHECK(seen.size() == 4);
    CHECK(pool.used() == 4);
    CHECK(pool.allocate() == nullptr);

    void* first = *seen.begin();
    pool.free(first);
    CHECK(pool.used() == 3);
    CHECK(pool.allocate() == first);
}

TEST_CASE("pool allocator: blocks respect alignment") {
    PoolAllocator pool(5, 8, 32);
    CHECK(pool.blockSize() % 32 == 0);
    for (int i = 0; i < 8; ++i) {
        CHECK(reinterpret_cast<std::uintptr_t>(pool.allocate()) % 32 == 0);
    }
}
