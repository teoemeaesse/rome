#include "ecs/entity/entity.hpp"

#include <gtest/gtest.h>

using namespace iodine::core;

/**
 * @brief Tests basic creation, destruction, and reuse of Entity slots.
 */
TEST(EntityRegistryTest, CreateDestroyReuse) {
    Entity::Registry registry;

    Entity e1 = registry.create();
    Entity e2 = registry.create();

    // Entities should be different right after creation
    EXPECT_NE(e1, e2);

    registry.destroy(e2);

    // Create a third entity, which should reuse the e2 slot but with an incremented version
    Entity e3 = registry.create();

    // The new entity should not be equal to the old e2 because its version has changed
    EXPECT_NE(e2, e3);
    EXPECT_NE(e1, e3);
}

/**
 * @brief Tests that multiple creates without destruction yield distinct entities.
 */
TEST(EntityRegistryTest, MultipleCreateDistinct) {
    Entity::Registry registry;

    // Create several entities in a row
    Entity e1 = registry.create();
    Entity e2 = registry.create();
    Entity e3 = registry.create();

    // All should differ
    EXPECT_NE(e1, e2);
    EXPECT_NE(e2, e3);
    EXPECT_NE(e1, e3);
}

/**
 * @brief Tests destruction of multiple entities in random order.
 */
TEST(EntityRegistryTest, DestroyInDifferentOrder) {
    Entity::Registry registry;

    Entity e1 = registry.create();
    Entity e2 = registry.create();
    Entity e3 = registry.create();

    registry.destroy(e2);

    // Create a new entity (should reuse the slot from e2)
    Entity e4 = registry.create();
    EXPECT_NE(e2, e4);

    // Now destroy the first one
    registry.destroy(e1);

    // Create another entity (should reuse the slot from e1)
    Entity e5 = registry.create();
    EXPECT_NE(e1, e5);

    // e3 is still alive and should be distinct from newly created entities
    EXPECT_NE(e3, e4);
    EXPECT_NE(e3, e5);
}