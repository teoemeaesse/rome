#pragma once

#include "reflection/reflect.hpp"

namespace rome::core {
    namespace Component {
        using ID = u32;

        template <typename T>
        concept Component = std::copy_constructible<T> && requires { Reflect::reflect<T>(); };
    }  // namespace Component
}  // namespace rome::core
