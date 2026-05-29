#include <gtest/gtest.h>

#include "rm/ecs/component/registry.hpp"
#include "rm/ecs/entity/registry.hpp"
#include "rm/ecs/event/registry.hpp"
#include "rm/ecs/system/descriptor.hpp"
#include "rm/ecs/system/registry.hpp"
#include "rm/ecs/world.hpp"
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

static World makeWorld(System::Registry& systems, Component::Registry& components, Entity::Registry& entities, Event::Registry& events) {
    return World{systems, components, entities, events};
}

TEST(SystemBuilder, Build_WithMasks_OK) {
    System::Registry systems;
    Component::Registry components;
    Entity::Registry entities;
    Event::Registry events;
    World world = makeWorld(systems, components, entities, events);

    EXPECT_TRUE(events.submit("Emitted"));
    EXPECT_TRUE(events.submit("Listened"));
    Event::ID emitted = events.getID("Emitted");
    Event::ID listened = events.getID("Listened");

    System::Descriptor descriptor = System::Builder("movement", world)
                                        .reads<Position>()
                                        .writes<Velocity>()
                                        .emits({emitted})
                                        .listens({listened})
                                        .requireFull()
                                        .build([](System::Context&) {});

    EXPECT_EQ(descriptor.name, "movement");
    EXPECT_TRUE(descriptor.reads.test(components.getID<Position>()));
    EXPECT_TRUE(descriptor.writes.test(components.getID<Velocity>()));
    EXPECT_TRUE(descriptor.emits.test(emitted));
    EXPECT_TRUE(descriptor.listens.test(listened));
    EXPECT_TRUE(descriptor.requireFull);
    EXPECT_FALSE(descriptor.allowPartial);
    EXPECT_TRUE(descriptor.active);
}

TEST(SystemRegistry, Submit_ValidDescriptor_OK) {
    System::Registry systems;
    Component::Registry components;
    Entity::Registry entities;
    Event::Registry events;
    World world = makeWorld(systems, components, entities, events);

    EXPECT_TRUE(systems.submit(System::Builder("movement", world).writes<Position>().build([](System::Context&) {})));
    System::ID id = systems.getID("movement");

    EXPECT_NE(id, System::INVALID_ID);
    EXPECT_TRUE(systems.check(id));
    EXPECT_EQ(systems.get(id).name, "movement");
    EXPECT_TRUE(systems.getGroup(id).isEmpty());
}

TEST(SystemRegistry, Submit_DuplicateName_ReturnsFalse) {
    System::Registry systems;
    Component::Registry components;
    Entity::Registry entities;
    Event::Registry events;
    World world = makeWorld(systems, components, entities, events);

    EXPECT_TRUE(systems.submit(System::Builder("movement", world).writes<Position>().build([](System::Context&) {})));

    EXPECT_FALSE(systems.submit(System::Builder("movement", world).writes<Velocity>().build([](System::Context&) {})));
}

TEST(SystemRegistry, Revoke_InBounds_RevokesSystem) {
    System::Registry systems;
    Component::Registry components;
    Entity::Registry entities;
    Event::Registry events;
    World world = makeWorld(systems, components, entities, events);

    EXPECT_TRUE(systems.submit(System::Builder("movement", world).writes<Position>().build([](System::Context&) {})));
    System::ID id = systems.getID("movement");

    EXPECT_TRUE(systems.revoke(id));

    EXPECT_FALSE(systems.check(id));
    EXPECT_THROW(systems.get(id), Exception);
    EXPECT_THROW(systems.getGroup(id), Exception);
}

TEST(SystemRegistry, Revoke_MissingID_ReturnsFalse) {
    System::Registry systems;

    EXPECT_FALSE(systems.revoke(System::INVALID_ID));
    EXPECT_FALSE(systems.revoke(42));
}

TEST(SystemRegistry, UpdateEntity_FullMatch_TracksEntity) {
    System::Registry systems;
    Component::Registry components;
    Entity::Registry entities;
    Event::Registry events;
    World world = makeWorld(systems, components, entities, events);

    EXPECT_TRUE(
        systems.submit(System::Builder("movement", world).reads<Velocity>().writes<Position>().requireFull().build([](System::Context&) {})));
    System::ID id = systems.getID("movement");
    Entity entity = entities.create();

    ASSERT_NE(components.emplace<Position>(entity, 1.0f, 2.0f), nullptr);
    systems.updateEntity(entity);
    EXPECT_FALSE(systems.getGroup(id).contains(entity));

    ASSERT_NE(components.emplace<Velocity>(entity, 3.0f, 4.0f), nullptr);
    systems.updateEntity(entity);
    EXPECT_TRUE(systems.getGroup(id).contains(entity));

    components.remove<Velocity>(entity);
    systems.updateEntity(entity);
    EXPECT_FALSE(systems.getGroup(id).contains(entity));
}

TEST(SystemRegistry, UpdateEntity_PartialMatch_TracksEntity) {
    System::Registry systems;
    Component::Registry components;
    Entity::Registry entities;
    Event::Registry events;
    World world = makeWorld(systems, components, entities, events);

    EXPECT_TRUE(
        systems.submit(System::Builder("movement", world).reads<Velocity>().writes<Position>().allowPartial().build([](System::Context&) {})));
    System::ID id = systems.getID("movement");
    Entity entity = entities.create();

    ASSERT_NE(components.emplace<Position>(entity, 1.0f, 2.0f), nullptr);
    systems.updateEntity(entity);
    EXPECT_FALSE(systems.getGroup(id).contains(entity));

    ASSERT_NE(components.emplace<Velocity>(entity, 3.0f, 4.0f), nullptr);
    systems.updateEntity(entity);
    EXPECT_TRUE(systems.getGroup(id).contains(entity));
}
