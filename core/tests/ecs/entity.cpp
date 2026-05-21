#include <gtest/gtest.h>

#include "rm/ecs/entity/registry.hpp"

using namespace rome;
using namespace rome::core;

TEST(Entity, Comparison_SameHandle_IsEqual) {
    Entity::Registry registry;
    Entity e = registry.create();
    Entity copy = e;

    EXPECT_TRUE(e == copy);
    EXPECT_FALSE(e != copy);
}

TEST(Entity, Comparison_DifferentHandle_NotEqual) {
    Entity::Registry registry;
    Entity e0 = registry.create();
    Entity e1 = registry.create();

    EXPECT_TRUE(e0 != e1);
    EXPECT_FALSE(e0 == e1);
}

TEST(EntityRegistry, Create_InBounds_ReturnsAliveEntity) {
    Entity::Registry registry;

    Entity e0 = registry.create();
    Entity e1 = registry.create();
    Entity e2 = registry.create();

    EXPECT_TRUE(registry.isAlive(e0));
    EXPECT_TRUE(registry.isAlive(e1));
    EXPECT_TRUE(registry.isAlive(e2));
}

TEST(EntityRegistry, Destroy_InBounds_KillsEntity) {
    Entity::Registry registry;
    Entity e = registry.create();

    ASSERT_TRUE(registry.isAlive(e));

    registry.destroy(e);

    EXPECT_FALSE(registry.isAlive(e));
}

TEST(EntityRegistry, Destroy_ThenCreate_RecyclesEntity) {
    Entity::Registry registry;
    Entity e0 = registry.create();

    registry.destroy(e0);
    Entity e1 = registry.create();

    EXPECT_EQ(e1.getIndex(), e0.getIndex());
    EXPECT_EQ(e1.getVersion(), e0.getVersion() + 1);
    EXPECT_NE(e1, e0);

    EXPECT_FALSE(registry.isAlive(e0));
    EXPECT_TRUE(registry.isAlive(e1));
}

TEST(EntityRegistry, Destroy_Repeated_IncrementsVersion) {
    Entity::Registry registry;
    Entity e = registry.create();

    for (u64 expectedVersion = 1; expectedVersion <= 128; ++expectedVersion) {
        registry.destroy(e);
        EXPECT_FALSE(registry.isAlive(e));

        e = registry.create();
        EXPECT_EQ(e.getIndex(), 0);
        EXPECT_EQ(e.getVersion(), expectedVersion);
        EXPECT_TRUE(registry.isAlive(e));
    }
}

TEST(EntityRegistry, Destroy_Repeated_KillsEntities) {
    Entity::Registry registry;

    std::vector<Entity> entities;
    for (u64 i = 0; i < 128; ++i) entities.push_back(registry.create());

    for (Entity e : entities) registry.destroy(e);
    for (Entity e : entities) EXPECT_FALSE(registry.isAlive(e));
}

TEST(EntityRegistry, Destroy_Duplicate_OK) {
    Entity::Registry registry;
    Entity e = registry.create();

    registry.destroy(e);
    registry.destroy(e);
    registry.destroy(e);
    registry.destroy(e);

    EXPECT_FALSE(registry.isAlive(e));
}

TEST(EntityRegistry, Destroy_StaleHandle_NoEffect) {
    Entity::Registry registry;
    Entity first = registry.create();

    registry.destroy(first);
    Entity second = registry.create();

    ASSERT_EQ(first.getIndex(), second.getIndex());
    ASSERT_NE(first.getVersion(), second.getVersion());

    registry.destroy(first);

    EXPECT_TRUE(registry.isAlive(second));
    EXPECT_FALSE(registry.isAlive(first));
    ASSERT_EQ(first.getIndex(), second.getIndex());
    ASSERT_NE(first.getVersion(), second.getVersion());
}

TEST(EntityRegistry, IsAlive_WrongRegistry_OK) {
    Entity::Registry registry0;
    Entity::Registry registry1;

    Entity created = registry0.create();

    EXPECT_FALSE(registry1.isAlive(created));
}
