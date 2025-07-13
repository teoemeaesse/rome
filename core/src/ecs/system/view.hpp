#pragma once

#include "ecs/system/group.hpp"
#include "ecs/world.hpp"

namespace rome::core {
    namespace System {
        // Helpers to find the index of a type in a parameter pack.
        template <typename T, typename... List>
        struct index_of;
        template <typename T, typename First, typename... Rest>
        struct index_of<T, First, Rest...> {
            static constexpr u64 value = std::is_same<T, First>::value ? 0 : 1 + index_of<T, Rest...>::value;
        };
        template <typename T>
        struct index_of<T> {
            STATIC_ASSERT(sizeof(T) == 0, "Type not found in index_of");
        };

        struct Group;
        struct RM_API Context {
            const Group& group;  ///< The group this system is operating on.
            World& world;        ///< Reference to the world instance.
        };

        template <Component::Component... Components>
        class RM_API ViewIterator final {
            public:
            ViewIterator(const std::tuple<remove_all_qualifiers_t<Components>*...>& owned,
                         const std::tuple<Component::Pool<remove_all_qualifiers_t<Components>>*...>& pools, const Entity* entities, u64 index,
                         u64 max)
                : owned(owned), pools(pools), entities(entities), index(index), max(max) {}

            bool operator!=(const ViewIterator& iter) const { return index != iter.index; }
            ViewIterator& operator++() {
                index++;
                return *this;
            }

            decltype(auto) operator*() const {
                return std::apply(
                    [this](auto*... owned) {
                        return std::apply(
                            [this, owned...](auto*... pool) { return std::forward_as_tuple(fetch<Components>(owned, pool, entities[index])...); },
                            pools);
                    },
                    owned);
            }

            private:
            const std::tuple<remove_all_qualifiers_t<Components>*...> owned;
            const std::tuple<Component::Pool<remove_all_qualifiers_t<Components>>*...> pools;
            const Entity* entities;
            const u64 max;
            u64 index;

            template <class T>
            static decltype(auto) fetch(auto* owned, auto* pool, Entity e) {
                if (owned) {
                    if constexpr (std::is_const_v<T>)
                        return static_cast<const remove_all_qualifiers_t<T>&>(*(owned));
                    else
                        return static_cast<remove_all_qualifiers_t<T>&>(*(owned));
                } else {
                    if constexpr (std::is_const_v<T>)
                        return static_cast<const remove_all_qualifiers_t<T>&>(pool->get(e));
                    else
                        return static_cast<remove_all_qualifiers_t<T>&>(pool->get(e));
                }
            }
        };

        template <Component::Component... Components>
        class RM_API View final {
            public:
            explicit View(Context& ctx) : count(ctx.group.getSize()) {
                auto entities = ctx.group.getEntities();
                this->entities = entities.data();

                (void)std::initializer_list<int>{(source<Components>(ctx), 0)...};
            }

            ViewIterator<Components...> begin() const { return ViewIterator<Components...>{owned, pools, entities, 0, count}; }
            ViewIterator<Components...> end() const { return ViewIterator<Components...>{owned, pools, entities, count, count}; }

            private:
            template <Component::Component T>
            void source(Context& ctx) {
                constexpr u64 index = index_of<remove_all_qualifiers_t<T>, remove_all_qualifiers_t<Components>...>::value;
                auto* pool = ctx.world.components.getPool<remove_all_qualifiers_t<T>>();
                auto [ptr, _] = pool->getData();
                if (ctx.group.owning.test(ctx.world.components.enter<remove_all_qualifiers_t<T>>())) {
                    std::get<index>(owned) = ptr;
                    std::get<index>(pools) = nullptr;
                } else {
                    std::get<index>(owned) = nullptr;
                    std::get<index>(pools) = pool;
                }
            }

            std::tuple<remove_all_qualifiers_t<Components>*...> owned;
            std::tuple<Component::Pool<remove_all_qualifiers_t<Components>>*...> pools;
            const Entity* entities;
            const u64 count;
        };
    }  // namespace System
}  // namespace rome::core