// 池分配器 —— 对应《游戏引擎架构》第6.2节（内存管理）。
// 所有块大小相同；空闲块通过嵌入式链表(free list)串起来，分配/释放都是 O(1)。
// 典型用途：粒子、子弹、组件等大量同尺寸对象。
#pragma once

#include "engine/core/assert.h"
#include "engine/core/types.h"

#include <algorithm>
#include <memory>

namespace engine {

class PoolAllocator {
public:
    PoolAllocator(usize blockSize, usize blockCount, usize alignment = alignof(std::max_align_t))
        : m_blockCount(blockCount) {
        ENGINE_ASSERT((alignment & (alignment - 1)) == 0, "alignment must be a power of two");
        // 每个块至少能放下一个指针（用于空闲链表），并且大小是对齐值的整数倍。
        const usize minSize = std::max(blockSize, sizeof(void*));
        m_alignment = std::max(alignment, alignof(void*));
        m_blockSize = (minSize + m_alignment - 1) & ~(m_alignment - 1);
        m_buffer = std::make_unique<std::byte[]>(m_blockSize * blockCount + m_alignment);

        auto raw = reinterpret_cast<std::uintptr_t>(m_buffer.get());
        m_start = reinterpret_cast<std::byte*>((raw + m_alignment - 1) & ~(m_alignment - 1));
        reset();
    }

    PoolAllocator(const PoolAllocator&) = delete;
    PoolAllocator& operator=(const PoolAllocator&) = delete;

    void* allocate() {
        if (!m_freeList) {
            return nullptr;
        }
        FreeNode* node = m_freeList;
        m_freeList = node->next;
        ++m_used;
        return node;
    }

    void free(void* ptr) {
        if (!ptr) {
            return;
        }
        ENGINE_ASSERT(owns(ptr), "pointer does not belong to this pool");
        auto* node = static_cast<FreeNode*>(ptr);
        node->next = m_freeList;
        m_freeList = node;
        --m_used;
    }

    // 把所有块重新放回空闲链表（不调用析构）。
    void reset() {
        m_freeList = nullptr;
        for (usize i = m_blockCount; i > 0; --i) {
            auto* node = reinterpret_cast<FreeNode*>(m_start + (i - 1) * m_blockSize);
            node->next = m_freeList;
            m_freeList = node;
        }
        m_used = 0;
    }

    bool owns(const void* ptr) const {
        auto p = reinterpret_cast<std::uintptr_t>(ptr);
        auto begin = reinterpret_cast<std::uintptr_t>(m_start);
        auto end = begin + m_blockSize * m_blockCount;
        return p >= begin && p < end && (p - begin) % m_blockSize == 0;
    }

    usize blockSize() const { return m_blockSize; }
    usize blockCount() const { return m_blockCount; }
    usize used() const { return m_used; }

private:
    struct FreeNode {
        FreeNode* next;
    };

    std::unique_ptr<std::byte[]> m_buffer;
    std::byte* m_start = nullptr;
    FreeNode* m_freeList = nullptr;
    usize m_blockSize = 0;
    usize m_blockCount = 0;
    usize m_alignment = 0;
    usize m_used = 0;
};

} // namespace engine
