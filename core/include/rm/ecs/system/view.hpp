#pragma once

#include "rm/ecs/group/group.hpp"
#include "rm/ecs/world.hpp"

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
            const std::tuple<remove_all_qualifiers_t<Components>*...> owned;                   ///< Pointers to owned components, if any.
            const std::tuple<Component::Pool<remove_all_qualifiers_t<Components>>*...> pools;  ///< Pointers to component pools.
            const Entity* entities;                                                            ///< Pointer to the entities.
            const u64 max;                                                                     ///< Maximum number of entities in the view.
            u64 index;                                                                         ///< Current index in the view.

            /**
             * @brief Fetches the component for a given entity. Decides whether to use the owned pointer or the pool based on availability.
             * @tparam T The component type to fetch.
             * @param owned Pointer to the owned component, if any.
             * @param pool Pointer to the component pool, if not owned.
             * @param e The entity to fetch the component for.
             * @return A reference to the component.
             */
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

        /**
         * @brief A view over a set of components for a specific group of entities.
         * @tparam ...Components The component types to include in the view.
         */
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

            std::tuple<remove_all_qualifiers_t<Components>*...> owned;                   ///< Pointers to owned components, if any.
            std::tuple<Component::Pool<remove_all_qualifiers_t<Components>>*...> pools;  ///< Pointers to component pools.
            const Entity* entities;                                                      ///< Pointer to the entities.
            const u64 count;                                                             ///< Number of entities in the view.
        };
    }  // namespace System
}  // namespace rome::core
