#pragma once

#include "container/sparse_set.hpp"
#include "debug/log.hpp"
#include "ecs/entity/entity.hpp"
#include "reflection/reflect.hpp"

namespace iodine::core {
    namespace Component {
        /**
         * @brief A component is a data structure that holds data for an entity.
         *        Components must have implemented reflection to be used in the ECS.
         */
        template <typename T>
        static inline b8 isComponent() {
            return is_reflectable_v<T> && std::is_copy_constructible_v<T>;
        }

        /**
         * @brief Acts as an interface for the storage of components.
         */
        class IO_API Dashboard {
            public:
            virtual ~Dashboard() = default;

            /**
             * @brief Gets the component for the given entity.
             * @tparam T The component type to get.
             * @param entity The entity to get the component for.
             * @return The component for the given entity.
             */
            template <typename T>
            T& get(const Entity& entity) {
                IO_ASSERT_MSG(isComponent<T>(), "Component must be reflectable and copy-constructible to be stored");
                IO_ASSERT_MSG(Reflect::reflect<T>().getType() == getType(), "Component type mismatch");
                return *static_cast<T*>(get(entity));
            }

            /**
             * @brief Gets the component for the given entity.
             * @tparam T The component type to get.
             * @param entity The entity to get the component for.
             * @return The component for the given entity.
             */
            template <typename T>
            const T& get(const Entity& entity) const {
                IO_ASSERT_MSG(isComponent<T>(), "Component must be reflectable and copy-constructible to be stored");
                IO_ASSERT_MSG(Reflect::reflect<T>().getType() == getType(), "Component type mismatch");
                return *static_cast<T*>(get(entity));
            }

            /**
             * @brief Inserts the component for the given entity.
             * @tparam T The component type to insert.
             * @param entity The entity to insert the component for.
             * @param component The component to insert.
             */
            template <typename T>
            void insert(const Entity& entity, T& component) {
                IO_ASSERT_MSG(isComponent<T>(), "Component must be reflectable and copy-constructible to be stored");
                IO_ASSERT_MSG(Reflect::reflect<T>().getType() == getType(), "Component type mismatch");
                insert(entity, &component);
            }

            /**
             * @brief Removes the component for the given entity.
             * @param entity The entity to remove the component for.
             */
            virtual void remove(const Entity& entity) = 0;

            /**
             * @brief Gets the reflected type for this dashboard's component type.
             * @return The reflected type for this dashboard's component type.
             */
            virtual Type& getType() = 0;

            private:
            /**
             * @brief Gets the component for the given entity.
             * @param entity The entity to get the component for.
             * @return The component for the given entity.
             */
            virtual void* get(const Entity& entity) = 0;

            /**
             * @brief Gets the component for the given entity.
             * @param entity The entity to get the component for.
             * @return The component for the given entity.
             */
            const void* get(const Entity& entity) const { return const_cast<Dashboard*>(this)->get(entity); }

            /**
             * @brief Inserts the component for the given entity.
             * @param entity The entity to insert the component for.
             * @param component The component to insert.
             */
            virtual void insert(const Entity& entity, void* component) = 0;
        };

        /**
         * @brief Manages the storage of a component type.
         * @tparam T The component type to manage.
         */
        template <typename T>
        class IO_API Storage : public Dashboard {
            IO_ASSERT_MSG(isComponent<T>(), "Component must be reflectable and copy-constructible to be stored");

            public:
            Storage() : type(Reflect::reflect<T>().getType()) {}
            ~Storage() = default;
            Storage(const Storage& other) = delete;
            Storage(Storage&& other) noexcept = default;
            Storage& operator=(const Storage& other) = delete;
            Storage& operator=(Storage&& other) noexcept = default;

            /**
             * @brief Removes the component for the given entity.
             * @param entity The entity to remove the component for.
             */
            void remove(const Entity& entity) override {
                if (!entities.contains(entity.getIndex())) {
                    IO_WARN("Entity does not have component of type: %s", getType().getName().c_str());
                    return;
                }
                entities.erase(entity.getIndex());
            }

            /**
             * @brief Gets the reflected type for this dashboard's component type.
             * @return The reflected type for this dashboard's component type.
             */
            Type& getType() override {
                static Type& type = Reflect::reflect<T>();
                return type;
            }

            /**
             * @brief Gets the component for the given entity.
             * @param entity The entity to get the component for.
             * @return The component for the given entity.
             */
            void* get(const Entity& entity) override {
                if (!entities.contains(entity.getIndex())) {
                    IO_WARN("Entity does not have component of type: %s", getType().getName().c_str());
                    return nullptr;
                }
                return &entities[entity.getIndex()];
            }

            /**
             * @brief Inserts the component for the given entity.
             * @param entity The entity to insert the component for.
             * @param component The component to insert.
             */
            void insert(const Entity& entity, void* component) override {
                if (entities.contains(entity.getIndex())) {
                    IO_WARN("Entity already has component of type: %s", getType().getName().c_str());
                    return;
                }
                entities.emplace(entity.getIndex(), *static_cast<T*>(component));
            }

            inline std::vector<T>::iterator begin() { return entities.begin(); }
            inline std::vector<T>::iterator end() { return entities.end(); }

            inline std::vector<T>::const_iterator begin() const { return entities.begin(); }
            inline std::vector<T>::const_iterator end() const { return entities.end(); }

            private:
            SparseSet<T> entities;  ///< The entities with this component.
            Type& type;             ///< The reflected type for this component.
        };

        /**
         * @brief Manages the registration, creation, and destruction of components.
         *        Components must be registered before they can be used in the ECS.
         *        Every component must implement reflection and a copy-constructor to be registered.
         */
        class IO_API Registry {
            public:
            Registry() = default;
            ~Registry() = default;

            /**
             * @brief Registers a component type with the registry.
             * @tparam T The component type to register.
             * @return The ID of the registered component.
             */
            template <typename T>
            const UUID& registerComponent() {
                IO_ASSERT_MSG(isComponent<T>(), "Component must be reflectable and copy-constructible to be registered");
                static UUID id = Reflect::reflect<T>().getType().getUUID();
                IO_ASSERT_MSG(store.find(id) == store.end(), "Duplicate component registration");
                store[id] = MakeUnique<Storage<T>>();
                return id;
            }

            /**
             * @brief Creates a new component for the given entity.
             * @tparam T The component type to create.
             * @param entity The entity to create the component for.
             * @return The created component.
             */
            template <typename T>
            T& createComponent(const Entity& entity, T& component) {
                IO_ASSERT_MSG(isComponent<T>(), "Component must be reflectable and copy-constructible to be created");
                UUID id = Reflect::reflect<T>().getType().getUUID();
                IO_ASSERT_MSG(store.find(id) != store.end(), "Component not registered");
                store[id]->insert(entity, component);
                return component;
            }

            /**
             * @brief Creates a new component for the given entity.
             * @tparam T The component type to create.
             * @tparam Args The types of the arguments to forward to the component constructor.
             * @param entity The entity to create the component for.
             * @param ...args The arguments to forward to the component constructor.
             * @return The created component.
             */
            template <typename T, typename... Args>
            T& createComponent(const Entity& entity, Args&&... args) {
                IO_ASSERT_MSG(isComponent<T>(), "Component must be reflectable and copy-constructible to be created");
                UUID id = Reflect::reflect<T>().getType().getUUID();
                IO_ASSERT_MSG(store.find(id) != store.end(), "Component not registered");
                T component(std::forward<Args>(args)...);
                store[id]->insert(entity, component);
                return component;
            }

            /**
             * @brief Removes a component from a given entity.
             * @tparam T The component type.
             * @param entity The entity to remove the component from.
             */
            template <typename T>
            void removeComponent(const Entity& entity) {
                IO_ASSERT_MSG(isComponent<T>(), "Component must be reflectable and copy-constructible to be removed");
                UUID id = Reflect::reflect<T>().getType().getUUID();
                IO_ASSERT_MSG(store.find(id) != store.end(), "Component not registered");
                store[id]->remove(entity);
            }

            /**
             * @brief Gets the component for the given entity.
             * @tparam T The component type to get.
             * @param entity The entity to get the component for.
             * @return The component for the given entity.
             */
            template <typename T>
            T& getComponent(const Entity& entity) {
                IO_ASSERT_MSG(isComponent<T>(), "Component must be reflectable and copy-constructible to be retrieved");
                UUID id = Reflect::reflect<T>().getType().getUUID();
                IO_ASSERT_MSG(store.find(id) != store.end(), "Component not registered");
                return store.at(id)->get<T>(entity);
            }

            /**
             * @brief Gets the component for the given entity.
             * @tparam T The component type to get.
             * @param entity The entity to get the component for.
             * @return The component for the given entity.
             */
            template <typename T>
            const T& getComponent(const Entity& entity) const {
                IO_ASSERT_MSG(isComponent<T>(), "Component must be reflectable and copy-constructible to be retrieved");
                UUID id = Reflect::reflect<T>().getType().getUUID();
                IO_ASSERT_MSG(store.find(id) != store.end(), "Component not registered");
                return store.at(id)->get<T>(entity);
            }

            private:
            std::unordered_map<UUID, Unique<Dashboard>> store;  ///< The storage for component types.
        };
    }  // namespace Component
}  // namespace iodine::core
