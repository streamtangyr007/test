// 极简 ECS（实体-组件-系统）—— 对应《游戏引擎架构》第16章（运行时游戏性基础系统）
// 中"以属性为中心(property-centric)"的游戏对象模型。
//
// - Entity：32 位 = 20 位索引 + 12 位代数(generation)，销毁后旧句柄自动失效
// - 组件存储：每种组件类型一个稀疏集(sparse set)，数据在 dense 数组里紧密排列，缓存友好
// - 系统：就是普通函数，通过 world.each<A, B>(...) 遍历
//
// 这是学习用实现，刻意保持简单；以后可以替换为 EnTT / flecs 或自己的 archetype 方案。
#pragma once

#include "engine/core/assert.h"
#include "engine/core/types.h"

#include <memory>
#include <tuple>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace engine::ecs {

struct Entity {
    u32 id = 0xFFFFFFFFu;

    static constexpr u32 kIndexBits = 20;
    static constexpr u32 kIndexMask = (1u << kIndexBits) - 1;
    static constexpr u32 kGenerationMask = 0xFFFu;

    u32 index() const { return id & kIndexMask; }
    u32 generation() const { return (id >> kIndexBits) & kGenerationMask; }
    bool isNull() const { return id == 0xFFFFFFFFu; }

    static Entity make(u32 index, u32 generation) {
        return Entity{(index & kIndexMask) | ((generation & kGenerationMask) << kIndexBits)};
    }

    friend bool operator==(Entity a, Entity b) { return a.id == b.id; }
    friend bool operator!=(Entity a, Entity b) { return a.id != b.id; }
};

inline constexpr Entity kNullEntity{};

namespace detail {

struct IStorage {
    virtual ~IStorage() = default;
    virtual void remove(u32 entityIndex) = 0;
    virtual bool has(u32 entityIndex) const = 0;
    virtual usize size() const = 0;
};

template <typename T> class SparseSet final : public IStorage {
public:
    static constexpr u32 kEmpty = 0xFFFFFFFFu;

    template <typename... Args> T& emplace(u32 index, Args&&... args) {
        if (index >= m_sparse.size()) {
            m_sparse.resize(index + 1, kEmpty);
        }
        if (m_sparse[index] != kEmpty) {
            T& existing = m_data[m_sparse[index]];
            existing = T{std::forward<Args>(args)...};
            return existing;
        }
        m_sparse[index] = static_cast<u32>(m_dense.size());
        m_dense.push_back(index);
        m_data.push_back(T{std::forward<Args>(args)...});
        return m_data.back();
    }

    void remove(u32 index) override {
        if (!has(index)) {
            return;
        }
        // swap-and-pop：把最后一个元素挪到被删位置，保持 dense 数组紧密。
        const u32 slot = m_sparse[index];
        const u32 lastEntity = m_dense.back();
        m_dense[slot] = lastEntity;
        m_data[slot] = std::move(m_data.back());
        m_sparse[lastEntity] = slot;
        m_dense.pop_back();
        m_data.pop_back();
        m_sparse[index] = kEmpty;
    }

    bool has(u32 index) const override {
        return index < m_sparse.size() && m_sparse[index] != kEmpty;
    }

    usize size() const override { return m_dense.size(); }

    T* get(u32 index) { return has(index) ? &m_data[m_sparse[index]] : nullptr; }

    const std::vector<u32>& entities() const { return m_dense; }

private:
    std::vector<u32> m_sparse; // entity index -> dense slot
    std::vector<u32> m_dense;  // dense slot -> entity index
    std::vector<T> m_data;     // dense slot -> component
};

} // namespace detail

class World {
public:
    Entity create() {
        u32 index;
        if (!m_freeIndices.empty()) {
            index = m_freeIndices.back();
            m_freeIndices.pop_back();
        } else {
            index = static_cast<u32>(m_generations.size());
            ENGINE_ASSERT(index <= Entity::kIndexMask, "too many entities");
            m_generations.push_back(0);
        }
        ++m_alive;
        return Entity::make(index, m_generations[index]);
    }

    void destroy(Entity e) {
        if (!isAlive(e)) {
            return;
        }
        for (auto& [type, storage] : m_storages) {
            storage->remove(e.index());
        }
        m_generations[e.index()] = (m_generations[e.index()] + 1) & Entity::kGenerationMask;
        m_freeIndices.push_back(e.index());
        --m_alive;
    }

    bool isAlive(Entity e) const {
        return !e.isNull() && e.index() < m_generations.size() &&
               m_generations[e.index()] == e.generation();
    }

    usize aliveCount() const { return m_alive; }

    template <typename T, typename... Args> T& add(Entity e, Args&&... args) {
        ENGINE_ASSERT(isAlive(e), "add() on dead entity");
        return storage<T>().emplace(e.index(), std::forward<Args>(args)...);
    }

    template <typename T> void remove(Entity e) {
        if (isAlive(e)) {
            storage<T>().remove(e.index());
        }
    }

    template <typename T> T* get(Entity e) {
        return isAlive(e) ? storage<T>().get(e.index()) : nullptr;
    }

    template <typename T> bool has(Entity e) { return get<T>(e) != nullptr; }

    // 遍历同时拥有 First, Rest... 所有组件的实体：fn(Entity, First&, Rest&...)
    template <typename First, typename... Rest, typename Fn> void each(Fn&& fn) {
        auto& primary = storage<First>();
        // 复制一份实体列表，允许在回调里增删组件（学习阶段优先正确性而非极致性能）。
        const std::vector<u32> indices = primary.entities();
        for (u32 index : indices) {
            if (!(storage<Rest>().has(index) && ...)) {
                continue;
            }
            First* a = primary.get(index);
            if (!a) {
                continue;
            }
            fn(Entity::make(index, m_generations[index]), *a, *storage<Rest>().get(index)...);
        }
    }

private:
    template <typename T> detail::SparseSet<T>& storage() {
        auto key = std::type_index(typeid(T));
        auto it = m_storages.find(key);
        if (it == m_storages.end()) {
            it = m_storages.emplace(key, std::make_unique<detail::SparseSet<T>>()).first;
        }
        return static_cast<detail::SparseSet<T>&>(*it->second);
    }

    std::vector<u32> m_generations;
    std::vector<u32> m_freeIndices;
    std::unordered_map<std::type_index, std::unique_ptr<detail::IStorage>> m_storages;
    usize m_alive = 0;
};

} // namespace engine::ecs
