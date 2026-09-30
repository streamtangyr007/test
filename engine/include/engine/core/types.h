// 基础类型别名 —— 对应《游戏引擎架构》第3章：固定宽度的整数/浮点类型。
#pragma once

#include <cstddef>
#include <cstdint>

namespace engine {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
using f32 = float;
using f64 = double;
using usize = std::size_t;

} // namespace engine
