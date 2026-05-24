#include "rm/ecs/ecs.hpp"

#include <gtest/gtest.h>

#include "rm/ecs/system/view.hpp"
#include "rm/reflection/traits/field.hpp"

using namespace rome;
using namespace rome::core;

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

TEST(ECS, RunSystem_GroupView_MatchesGroup) {
    ECS ecs;
    u32 iterations = 0;

    System::ID id =
        ecs.registerSystem(ecs.createSystem("movement").reads<Velocity>().writes<Position>().requireFull().build([&](System::Context& ctx) {
            System::View<Position, Velocity> view(ctx);
            for (auto&& [position, velocity] : view) {
                position.x += velocity.dx;
                position.y += velocity.dy;
                iterations++;
            }
        }));

    Entity moving = ecs.createEntity();
    ecs.addComponent<Position>(moving, 1.0f, 2.0f);
    ecs.addComponent<Velocity>(moving, 3.0f, 4.0f);

    Entity stationary = ecs.createEntity();
    ecs.addComponent<Position>(stationary, 10.0f, 20.0f);

    ecs.runSystem(id);

    EXPECT_EQ(iterations, 1);
    EXPECT_FLOAT_EQ(ecs.getComponent<Position>(moving).x, 4.0f);
    EXPECT_FLOAT_EQ(ecs.getComponent<Position>(moving).y, 6.0f);
    EXPECT_FLOAT_EQ(ecs.getComponent<Position>(stationary).x, 10.0f);
    EXPECT_FLOAT_EQ(ecs.getComponent<Position>(stationary).y, 20.0f);
}

TEST(ECS, RegisterSystem_ExistingEntity_MatchesGroup) {
    ECS ecs;
    u32 iterations = 0;

    Entity entity = ecs.createEntity();
    ecs.addComponent<Position>(entity, 1.0f, 2.0f);
    ecs.addComponent<Velocity>(entity, 3.0f, 4.0f);

    System::ID id =
        ecs.registerSystem(ecs.createSystem("movement").reads<Velocity>().writes<Position>().requireFull().build([&](System::Context& ctx) {
            System::View<Position, Velocity> view(ctx);
            for (auto&& [position, velocity] : view) {
                position.x += velocity.dx;
                position.y += velocity.dy;
                iterations++;
            }
        }));

    ecs.runSystem(id);

    EXPECT_EQ(iterations, 1);
    EXPECT_FLOAT_EQ(ecs.getComponent<Position>(entity).x, 4.0f);
    EXPECT_FLOAT_EQ(ecs.getComponent<Position>(entity).y, 6.0f);
}
