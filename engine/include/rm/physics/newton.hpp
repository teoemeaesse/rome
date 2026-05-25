#pragma once

#include "rm/prelude.hpp"
#include "rm/reflection/external/primitives.hpp"
#include "rm/reflection/reflect.hpp"
#include "rm/reflection/traits/field.hpp"

namespace rome::engine::physics {
    struct Position {
        f32 x = 0.0f;
        f32 y = 0.0f;

        RM_REFLECT;
    };

    struct Velocity {
        f32 x = 0.0f;
        f32 y = 0.0f;

        RM_REFLECT;
    };

    struct Acceleration {
        f32 x = 0.0f;
        f32 y = 0.0f;

        RM_REFLECT;
    };

    struct NewtonIntegrator {
        f32 dt = 1.0f / 60.0f;

        RM_REFLECT;
    };
}  // namespace rome::engine::physics

RM_REFLECT_IMPL(rome::engine::physics::Position, "rome::engine::physics::Position",
                rome::core::Fields().with("x", &rome::engine::physics::Position::x).with("y", &rome::engine::physics::Position::y));

RM_REFLECT_IMPL(rome::engine::physics::Velocity, "rome::engine::physics::Velocity",
                rome::core::Fields().with("x", &rome::engine::physics::Velocity::x).with("y", &rome::engine::physics::Velocity::y));

RM_REFLECT_IMPL(rome::engine::physics::Acceleration, "rome::engine::physics::Acceleration",
                rome::core::Fields().with("x", &rome::engine::physics::Acceleration::x)
                    .with("y", &rome::engine::physics::Acceleration::y));

RM_REFLECT_IMPL(rome::engine::physics::NewtonIntegrator, "rome::engine::physics::NewtonIntegrator",
                rome::core::Fields().with("dt", &rome::engine::physics::NewtonIntegrator::dt));
