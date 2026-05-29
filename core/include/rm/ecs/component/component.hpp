#pragma once

#include "rm/reflection/reflect.hpp"

namespace rome::core {
    namespace Component {
        using ID = u32;
        inline constexpr ID INVALID_ID = 0;
        inline constexpr std::string_view INVALID_NAME = "";

        template <typename T>
        concept Component = std::copy_constructible<T> && requires { Reflect::reflect<T>(); };
    }  // namespace Component
}  // namespace rome::core
