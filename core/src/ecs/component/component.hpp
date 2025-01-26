#pragma once

#include "reflection/reflect.hpp"

namespace iodine::core {
    /**
     * @brief A component is a data structure that holds data for an entity.
     *        Components must have implemented reflection to be used in the ECS.
     */
    template <typename T>
    static inline b8 isComponent() {
        return is_reflectable_v<T>;
    }
}  // namespace iodine::core
