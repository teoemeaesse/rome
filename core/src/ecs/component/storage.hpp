#pragma once

#include "reflection/reflect.hpp"

namespace rome::core {
    namespace Component {
        template <typename T>
        concept Component = std::copy_constructible<T> && requires { Reflect::reflect<T>(); };

        using ID = u32;

        /**
         * @brief Acts as an interface for the storage of components.
         */
        class RM_API Storage {
            public:
            virtual ~Storage() = default;
        };
    }  // namespace Component
}  // namespace rome::core