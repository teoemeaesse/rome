#include <gtest/gtest.h>

#include "ecs/component/registry.hpp"
#include "ecs/entity/entity.hpp"
#include "reflection/traits/field.hpp"

using namespace iodine::core;

// A simple test‐only component: must be reflectable and copy‐constructible
struct Position {
    float x, y;

    IO_REFLECT;
};
IO_REFLECT_IMPL(Position, "Position", Fields().with("x", &Position::x).with("y", &Position::y));

TEST(ComponentRegistryTest, CreateDestroyReuse) {
    Entity::Registry entityRegistry;
    // Create a dummy entity with index 0
    Entity entity = entityRegistry.create();

    Component::Registry componentRegistry;

    // 1) Register the component type
    componentRegistry.enter<Position>();

    // 2) Create (insert) a Position on the entity
    Position initPos{1.0f, 2.0f};
    componentRegistry.create<Position>(entity, initPos);

    // 3) Retrieve and verify its values
    {
        auto& p = componentRegistry.get<Position>(entity);
        EXPECT_FLOAT_EQ(p.x, 1.0f);
        EXPECT_FLOAT_EQ(p.y, 2.0f);
    }

    // 4) Remove it
    componentRegistry.remove<Position>(entity);

    // 5) Re‐create (reuse the slot) with new values
    Position newPos{3.0f, 4.0f};
    componentRegistry.create<Position>(entity, newPos);

    // 6) Retrieve again and verify the new values
    {
        auto& p2 = componentRegistry.get<Position>(entity);
        EXPECT_FLOAT_EQ(p2.x, 3.0f);
        EXPECT_FLOAT_EQ(p2.y, 4.0f);
    }
}
