#include <gtest/gtest.h>

#include "rm/ecs/component/registry.hpp"
#include "rm/ecs/entity/entity.hpp"
#include "rm/ecs/entity/registry.hpp"
#include "rm/reflection/traits/field.hpp"

using namespace rome;
using namespace rome::core;

// A simple test‐only component: must be reflectable and copy‐constructible
struct Position {
    f32 x, y;

    RM_REFLECT;
};
RM_REFLECT_IMPL(Position, "Position", Fields().with("x", &Position::x).with("y", &Position::y));

struct Velocity {
    f32 dx, dy;

    RM_REFLECT;
};
RM_REFLECT_IMPL(Velocity, "Velocity", Fields().with("dx", &Velocity::dx).with("dy", &Velocity::dy));

Entity::Registry er;
Entity e1 = er.create(), e2 = er.create();

TEST(ComponentPool, Constructors_DefaultMove_OK) {
    Component::Pool<Position> pool;
    Component::Pool<Position> moved(std::move(pool));
}

TEST(ComponentPool, Assignment_DefaultMove_OK) {
    Component::Pool<Position> pool;
    Component::Pool<Position> moved = Component::Pool<Position>(std::move(pool));
}

TEST(ComponentPool, Get_InBounds_ReturnsComponent) {
    Component::Pool<Position> pool;
    e1 = er.create();
    e2 = er.create();
    pool.emplace(e1, 1.0f, -2.0f);
    pool.emplace(e2, 3.0f, 1.0f);

    Position* pos0 = pool.get(e1);
    EXPECT_FLOAT_EQ(pos0->x, 1.0f);
    EXPECT_FLOAT_EQ(pos0->y, -2.0f);
    const Position* pos1 = pool.get(e2);
    EXPECT_FLOAT_EQ(pos1->x, 3.0f);
    EXPECT_FLOAT_EQ(pos1->y, 1.0f);
}

TEST(ComponentPool, Get_OutOfBounds_ReturnsNullptr) {
    Component::Pool<Position> pool;
    e1 = er.create();
    e2 = er.create();
    pool.emplace(e1, 1.0f, -2.0f);

    const Position* pos = pool.get(e2);
    EXPECT_EQ(pos, nullptr);
}

TEST(ComponentPool, Insert_Always_OK) {
    Component::Pool<Position> pool;
    e1 = er.create();
    e2 = er.create();

    pool.insert(e1, Position(1.0f, -1.0f));
    Position* pos0 = pool.get(e1);
    EXPECT_FLOAT_EQ(pos0->x, 1.0f);
    EXPECT_FLOAT_EQ(pos0->y, -1.0f);
    pool.insert(e1, Position(2.0f, -3.0f));
    EXPECT_FLOAT_EQ(pos0->x, 1.0f);
    EXPECT_FLOAT_EQ(pos0->y, -1.0f);

    pool.insert(e2, Position(0.0f, 0.0f));
    Position* pos1 = pool.get(e2);
    pos0 = pool.get(e1);
    EXPECT_FLOAT_EQ(pos1->x, 0.0f);
    EXPECT_FLOAT_EQ(pos1->y, 0.0f);
    EXPECT_FLOAT_EQ(pos0->x, 1.0f);
    EXPECT_FLOAT_EQ(pos0->y, -1.0f);
}

TEST(ComponentPool, Emplace_Always_OK) {
    Component::Pool<Position> pool;
    e1 = er.create();
    e2 = er.create();

    pool.emplace(e1, 1.0f, -1.0f);
    Position* pos0 = pool.get(e1);
    EXPECT_FLOAT_EQ(pos0->x, 1.0f);
    EXPECT_FLOAT_EQ(pos0->y, -1.0f);
    pool.emplace(e1, 2.0f, -3.0f);
    EXPECT_FLOAT_EQ(pos0->x, 1.0f);
    EXPECT_FLOAT_EQ(pos0->y, -1.0f);

    pool.emplace(e2, 0.0f, 0.0f);
    Position* pos1 = pool.get(e2);
    pos0 = pool.get(e1);
    EXPECT_FLOAT_EQ(pos1->x, 0.0f);
    EXPECT_FLOAT_EQ(pos1->y, 0.0f);
    EXPECT_FLOAT_EQ(pos0->x, 1.0f);
    EXPECT_FLOAT_EQ(pos0->y, -1.0f);
}

TEST(ComponentPool, Remove_Always_OK) {
    Component::Pool<Position> pool;
    e1 = er.create();

    EXPECT_FALSE(pool.contains(e1));
    pool.remove(e1);

    pool.emplace(e1, 1.0f, -1.0f);
    EXPECT_TRUE(pool.contains(e1));

    Position* pos0 = pool.get(e1);
    EXPECT_FLOAT_EQ(pos0->x, 1.0f);
    EXPECT_FLOAT_EQ(pos0->y, -1.0f);
    pool.emplace(e1, 2.0f, -3.0f);
    EXPECT_FLOAT_EQ(pos0->x, 1.0f);
    EXPECT_FLOAT_EQ(pos0->y, -1.0f);

    pool.remove(e1);
    EXPECT_FALSE(pool.contains(e1));
    EXPECT_EQ(pool.get(e1), nullptr);
    pool.remove(e1);
}

TEST(ComponentPool, GetData_Always_OK) {
    Component::Pool<Position> pool;
    e1 = er.create();

    pool.emplace(e1, 1.0f, 127.0f);

    auto pair = pool.getData();
    Position* pos = pair.first;
    EXPECT_FLOAT_EQ(pos->x, 1.0f);
    EXPECT_FLOAT_EQ(pos->y, 127.0f);
    EXPECT_EQ(pair.second, 1);

    pos->x = -5.0f;
    const Position* updatedPos = pool.get(e1);
    EXPECT_FLOAT_EQ(pos->x, -5.0f);
    EXPECT_FLOAT_EQ(pos->y, 127.0f);
}

TEST(ComponentPool, Iterator_Always_OK) {
    Component::Pool<Position> pool;

    for (i32 i = 0; i < 150; i++) {
        e1 = er.create();
        pool.emplace(e1, (f32)i, 0);
    }

    i32 i = 0;
    for (Position p : pool) {
        EXPECT_FLOAT_EQ(p.x, (f32)i++);
    }
}

TEST(ComponentRegistry, Submit_DuplicateComponent_OK) {
    Component::Registry registry;

    EXPECT_TRUE(registry.submit<Position>());
    Component::ID id0 = registry.getID<Position>();
    EXPECT_FALSE(registry.submit<Position>());
    Component::ID id1 = registry.getID<Position>();

    EXPECT_NE(id0, Component::INVALID_ID);
    EXPECT_EQ(id0, id1);
    EXPECT_EQ(registry.getSize(), 1);
}

TEST(ComponentRegistry, Submit_DifferentComponents_DifferentIDs) {
    Component::Registry registry;

    EXPECT_TRUE(registry.submit<Position>());
    EXPECT_TRUE(registry.submit<Velocity>());
    Component::ID positionID = registry.getID<Position>();
    Component::ID velocityID = registry.getID<Velocity>();

    EXPECT_NE(positionID, Component::INVALID_ID);
    EXPECT_NE(velocityID, Component::INVALID_ID);
    EXPECT_NE(positionID, velocityID);
    EXPECT_EQ(registry.getSize(), 2);
}

TEST(ComponentRegistry, Check_SubmittedComponent_OK) {
    Component::Registry registry;

    EXPECT_FALSE(registry.check<Position>());
    EXPECT_TRUE(registry.submit<Position>());
    EXPECT_TRUE(registry.check<Position>());
    EXPECT_FALSE(registry.submit<Position>());
    registry.revoke<Position>();
    EXPECT_FALSE(registry.check<Position>());
    EXPECT_EQ(registry.getID<Position>(), Component::INVALID_ID);
}

TEST(ComponentRegistry, GetName_InBounds_OK) {
    Component::Registry registry;

    EXPECT_TRUE(registry.submit<Position>());
    Component::ID id = registry.getID<Position>();

    EXPECT_EQ(registry.getName(id), "Position");
}

TEST(ComponentRegistry, GetName_OutOfBounds_Empty) {
    Component::Registry registry;

    Component::ID id = Component::INVALID_ID;

    EXPECT_TRUE(registry.getName(id).empty());
}

TEST(ComponentRegistry, GetPool_InBounds_OK) {
    Component::Registry registry;

    EXPECT_TRUE(registry.submit<Position>());

    EXPECT_NE(registry.getPool<Position>(), nullptr);
}

TEST(ComponentRegistry, GetPool_OutOfBounds_ReturnsNullptr) {
    Component::Registry registry;

    EXPECT_EQ(registry.getPool<Position>(), nullptr);
}

TEST(ComponentRegistry, Insert_Always_OK) {
    Component::Registry registry;
    e1 = er.create();
    e2 = er.create();

    ASSERT_TRUE(registry.submit<Position>());

    Position* pos0 = registry.insert<Position>(e1, Position{1.0f, -2.0f});
    ASSERT_NE(pos0, nullptr);
    EXPECT_FLOAT_EQ(pos0->x, 1.0f);
    EXPECT_FLOAT_EQ(pos0->y, -2.0f);

    const Position* pos1 = registry.get<Position>(e1);
    ASSERT_NE(pos1, nullptr);
    EXPECT_FLOAT_EQ(pos1->x, 1.0f);
    EXPECT_FLOAT_EQ(pos1->y, -2.0f);

    EXPECT_EQ(registry.get<Position>(e2), nullptr);
}

TEST(ComponentRegistry, Emplace_Always_OK) {
    Component::Registry registry;
    e1 = er.create();
    e2 = er.create();

    ASSERT_TRUE(registry.submit<Position>());

    Position* pos0 = registry.emplace<Position>(e1, 3.0f, 4.0f);
    ASSERT_NE(pos0, nullptr);
    EXPECT_FLOAT_EQ(pos0->x, 3.0f);
    EXPECT_FLOAT_EQ(pos0->y, 4.0f);

    const Position* pos1 = registry.get<Position>(e1);
    ASSERT_NE(pos1, nullptr);
    EXPECT_FLOAT_EQ(pos1->x, 3.0f);
    EXPECT_FLOAT_EQ(pos1->y, 4.0f);

    EXPECT_EQ(registry.get<Position>(e2), nullptr);
}

TEST(ComponentRegistry, Get_Always_OK) {
    Component::Registry registry;
    e1 = er.create();
    e2 = er.create();

    const Position* p = registry.emplace<Position>(e1, 1.0f, 2.0f);
    EXPECT_NE(p, nullptr);
    EXPECT_FLOAT_EQ(p->x, 1.0f);
    EXPECT_FLOAT_EQ(p->y, 2.0f);

    EXPECT_EQ(registry.get<Position>(e2), nullptr);
}

TEST(ComponentRegistry, Has_Always_OK) {
    Component::Registry registry;
    e1 = er.create();
    e2 = er.create();

    const Position* p = registry.emplace<Position>(e1, 5.0f, 6.0f);
    EXPECT_NE(p, nullptr);

    EXPECT_TRUE(registry.has<Position>(e1));
    EXPECT_FALSE(registry.has<Position>(e2));
}

TEST(ComponentRegistry, Remove_Always_OK) {
    Component::Registry registry;
    e1 = er.create();

    EXPECT_FALSE(registry.has<Position>(e1));
    registry.remove<Position>(e1);

    const Position* p = registry.emplace<Position>(e1, 1.0f, -1.0f);
    EXPECT_NE(p, nullptr);
    EXPECT_TRUE(registry.has<Position>(e1));

    p = registry.get<Position>(e1);
    ASSERT_NE(p, nullptr);
    EXPECT_FLOAT_EQ(p->x, 1.0f);
    EXPECT_FLOAT_EQ(p->y, -1.0f);

    registry.remove<Position>(e1);
    EXPECT_FALSE(registry.has<Position>(e1));
    EXPECT_EQ(registry.get<Position>(e1), nullptr);

    registry.remove<Position>(e1);
}

TEST(ComponentRegistry, GetSize_Always_OK) {
    Component::Registry registry;

    EXPECT_EQ(registry.getSize(), 0);

    EXPECT_TRUE(registry.submit<Position>());
    EXPECT_EQ(registry.getSize(), 1);

    EXPECT_TRUE(registry.submit<Velocity>());
    EXPECT_EQ(registry.getSize(), 2);

    EXPECT_FALSE(registry.submit<Position>());
    EXPECT_EQ(registry.getSize(), 2);
}

TEST(ComponentRegistry, Revoke_InBounds_RemovesStorageAndArchetypes) {
    Component::Registry registry;
    e1 = er.create();

    EXPECT_TRUE(registry.submit<Position>());
    const Component::ID id = registry.getID<Position>();
    [[maybe_unused]] Position* position = registry.emplace<Position>(e1, 1.0f, 2.0f);
    EXPECT_TRUE(registry.has<Position>(e1));

    EXPECT_TRUE(registry.revoke<Position>());

    EXPECT_EQ(registry.getSize(), 0);
    EXPECT_TRUE(registry.getName(id).empty());
    EXPECT_EQ(registry.getPool<Position>(), nullptr);
    EXPECT_FALSE(registry.has<Position>(e1));
}

TEST(ComponentRegistry, Revoke_Missing_ReturnsFalse) {
    Component::Registry registry;

    EXPECT_FALSE(registry.revoke<Position>());
}
