#include <gtest/gtest.h>

#include "rm/ecs/component/registry.hpp"
#include "rm/ecs/entity/registry.hpp"
#include "rm/reflection/traits/field.hpp"

using namespace rome::core;

// A simple test‐only component: must be reflectable and copy‐constructible
struct Position {
    float x, y;

    RM_REFLECT;
};
RM_REFLECT_IMPL(Position, "Position", Fields().with("x", &Position::x).with("y", &Position::y));

TEST(ComponentRegistryTest, CreateDestroyReuse) {}
