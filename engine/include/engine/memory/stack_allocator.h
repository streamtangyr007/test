// 栈分配器 —— 对应《游戏引擎架构》第6.2节（内存管理）。
// 从一整块预分配内存中线性分配；只能按"标记(marker)"整体回滚，不能单独释放。
// 典型用途：关卡加载、单帧临时数据。
#pragma once

#include "engine/core/assert.h"
#include "engine/core/types.h"

#include <memory>

namespace engine {

class StackAllocator {
public:
    using Marker = usize;

    explicit StackAllocator(usize capacityBytes)
        : m_buffer(std::make_unique<std::byte[]>(capacityBytes)), m_capacity(capacityBytes) {}

    StackAllocator(const StackAllocator&) = delete;
    StackAllocator& operator=(const StackAllocator&) = delete;

    // 分配 size 字节，按 alignment 对齐（必须是 2 的幂）。空间不足返回 nullptr。
    void* allocate(usize size, usize alignment = alignof(std::max_align_t)) {
        ENGINE_ASSERT((alignment & (alignment - 1)) == 0, "alignment must be a power of two");
        const auto base = reinterpret_cast<std::uintptr_t>(m_buffer.get());
        const std::uintptr_t current = base + m_top;
        const std::uintptr_t aligned = (current + alignment - 1) & ~(std::uintptr_t(alignment) - 1);
        const usize newTop = static_cast<usize>(aligned - base) + size;
        if (newTop > m_capacity) {
            return nullptr;
        }
        m_top = newTop;
        return reinterpret_cast<void*>(aligned);
    }

    template <typename T> T* allocateArray(usize count) {
        return static_cast<T*>(allocate(sizeof(T) * count, alignof(T)));
    }

    Marker marker() const { return m_top; }

    void freeToMarker(Marker marker) {
        ENGINE_ASSERT(marker <= m_top, "marker is above current top");
        m_top = marker;
    }

    void clear() { m_top = 0; }

    usize used() const { return m_top; }
    usize capacity() const { return m_capacity; }

private:
    std::unique_ptr<std::byte[]> m_buffer;
    usize m_capacity = 0;
    usize m_top = 0;
};

} // namespace engine
