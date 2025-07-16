#pragma once

#include "reflection/reflect.hpp"

namespace rome::core {
    namespace Event {
        using ID = u32;

        template <typename T>
        concept Event = std::copy_constructible<T> && requires { Reflect::reflect<T>(); };
    }  // namespace Event
}  // namespace rome::core