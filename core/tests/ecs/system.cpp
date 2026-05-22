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

struct SystemPosition {
    f32 x, y;

    RM_REFLECT;
};
RM_REFLECT_IMPL(SystemPosition, "SystemPosition", Fields().with("x", &SystemPosition::x).with("y", &SystemPosition::y));

struct SystemVelocity {
    f32 dx, dy;

    RM_REFLECT;
};
RM_REFLECT_IMPL(SystemVelocity, "SystemVelocity", Fields().with("dx", &SystemVelocity::dx).with("dy", &SystemVelocity::dy));

static World makeWorld(System::Registry& systems, Component::Registry& components, Entity::Registry& entities, Event::Registry& events) {
    return World{systems, components, entities, events};
}

TEST(SystemBuilder, Build_WithMasks_OK) {
    System::Registry systems;
    Component::Registry components;
    Entity::Registry entities;
    Event::Registry events;
    World world = makeWorld(systems, components, entities, events);

    Event::ID emitted = events.enter("Emitted");
    Event::ID listened = events.enter("Listened");

    System::Descriptor descriptor = System::Builder("movement", world)
                                        .reads<SystemPosition>()
                                        .writes<SystemVelocity>()
                                        .emits({emitted})
                                        .listens({listened})
                                        .requireFull(true)
                                        .allowPartial(false)
                                        .build([](System::Context&) {});

    EXPECT_EQ(descriptor.name, "movement");
    EXPECT_TRUE(descriptor.reads.test(components.submit<SystemPosition>()));
    EXPECT_TRUE(descriptor.writes.test(components.submit<SystemVelocity>()));
    EXPECT_TRUE(descriptor.emits.test(emitted));
    EXPECT_TRUE(descriptor.listens.test(listened));
    EXPECT_TRUE(descriptor.requireFull);
    EXPECT_FALSE(descriptor.allowPartial);
    EXPECT_TRUE(descriptor.active);
}

TEST(SystemRegistry, Enter_ValidDescriptor_OK) {
    System::Registry systems;
    Component::Registry components;
    Entity::Registry entities;
    Event::Registry events;
    World world = makeWorld(systems, components, entities, events);

    System::ID id = systems.enter(System::Builder("movement", world).writes<SystemPosition>().build([](System::Context&) {}));

    EXPECT_NE(id, System::INVALID_ID);
    EXPECT_TRUE(systems.contains(id));
    EXPECT_EQ(systems.get(id).name, "movement");
}

TEST(SystemRegistry, Enter_DuplicateName_ReturnsNullID) {
    System::Registry systems;
    Component::Registry components;
    Entity::Registry entities;
    Event::Registry events;
    World world = makeWorld(systems, components, entities, events);

    systems.enter(System::Builder("movement", world).writes<SystemPosition>().build([](System::Context&) {}));

    EXPECT_EQ(systems.enter(System::Builder("movement", world).writes<SystemVelocity>().build([](System::Context&) {})), System::INVALID_ID);
}

TEST(SystemRegistry, Erase_InBounds_RemovesSystem) {
    System::Registry systems;
    Component::Registry components;
    Entity::Registry entities;
    Event::Registry events;
    World world = makeWorld(systems, components, entities, events);

    System::ID id = systems.enter(System::Builder("movement", world).writes<SystemPosition>().build([](System::Context&) {}));

    systems.erase(id);

    EXPECT_FALSE(systems.contains(id));
    EXPECT_THROW(systems.get(id), Exception);
}
