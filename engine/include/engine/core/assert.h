// 断言 —— 对应《游戏引擎架构》第3章（错误捕获与处理）：调试期检查，发布版可编译为空。
#pragma once

#include <cstdio>
#include <cstdlib>

#if defined(NDEBUG) && !defined(ENGINE_FORCE_ASSERTS)
#define ENGINE_ASSERT(expr, msg) ((void)0)
#else
#define ENGINE_ASSERT(expr, msg)                                                                   \
    do {                                                                                           \
        if (!(expr)) {                                                                             \
            std::fprintf(stderr, "ASSERT FAILED: %s\n  %s\n  at %s:%d\n", #expr, msg, __FILE__,    \
                         __LINE__);                                                                \
            std::abort();                                                                          \
        }                                                                                          \
    } while (0)
#endif
