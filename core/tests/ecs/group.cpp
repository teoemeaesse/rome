#include "rm/ecs/group/group.hpp"

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

struct GroupPosition {
    f32 x, y;

    RM_REFLECT;
};
RM_REFLECT_IMPL(GroupPosition, "GroupPosition", Fields().with("x", &GroupPosition::x).with("y", &GroupPosition::y));

struct GroupVelocity {
    f32 dx, dy;

    RM_REFLECT;
};
RM_REFLECT_IMPL(GroupVelocity, "GroupVelocity", Fields().with("dx", &GroupVelocity::dx).with("dy", &GroupVelocity::dy));

static World makeGroupWorld(System::Registry& systems, Component::Registry& components, Entity::Registry& entities, Event::Registry& events) {
    return World{systems, components, entities, events};
}

TEST(Group, Constructors_FullOwning_OK) {
    System::Registry systems;
    Component::Registry components;
    Entity::Registry entities;
    Event::Registry events;
    World world = makeGroupWorld(systems, components, entities, events);

    System::Descriptor descriptor =
        System::Builder("movement", world).reads<GroupPosition>().writes<GroupVelocity>().requireFull(true).build([](System::Context&) {});
    Group group(descriptor);

    EXPECT_TRUE(group.owning.test(components.submit<GroupPosition>()));
    EXPECT_TRUE(group.owning.test(components.submit<GroupVelocity>()));
    EXPECT_TRUE(group.partial.none());
    EXPECT_TRUE(group.isEmpty());
}

TEST(Group, Constructors_Partial_OK) {
    System::Registry systems;
    Component::Registry components;
    Entity::Registry entities;
    Event::Registry events;
    World world = makeGroupWorld(systems, components, entities, events);

    System::Descriptor descriptor = System::Builder("movement", world)
                                        .reads<GroupPosition>()
                                        .writes<GroupVelocity>()
                                        .requireFull(false)
                                        .allowPartial(true)
                                        .build([](System::Context&) {});
    Group group(descriptor);

    EXPECT_TRUE(group.owning.test(components.submit<GroupVelocity>()));
    EXPECT_TRUE(group.partial.test(components.submit<GroupPosition>()));
}

TEST(Group, AddRemoveEntity_Always_OK) {
    System::Registry systems;
    Component::Registry components;
    Entity::Registry entities;
    Event::Registry events;
    World world = makeGroupWorld(systems, components, entities, events);
    Group group(System::Builder("movement", world).writes<GroupPosition>().build([](System::Context&) {}));
    Entity entity = entities.create();

    group.addEntity(entity);

    EXPECT_TRUE(group.contains(entity));
    ASSERT_EQ(group.getSize(), 1);
    EXPECT_EQ(group.getEntities()[0], entity);

    group.removeEntity(entity);

    EXPECT_FALSE(group.contains(entity));
    EXPECT_TRUE(group.isEmpty());
}

TEST(Group, AddRemoveEntity_WithMatchingArchetype_OK) {
    System::Registry systems;
    Component::Registry components;
    Entity::Registry entities;
    Event::Registry events;
    World world = makeGroupWorld(systems, components, entities, events);
    Group group(System::Builder("movement", world).writes<GroupPosition>().requireFull(true).build([](System::Context&) {}));
    Entity entity = entities.create();
    ASSERT_NE(components.emplace<GroupPosition>(entity, 1.0f, 2.0f), nullptr);

    group.addEntityOwned(entity);

    EXPECT_TRUE(group.contains(entity));
    EXPECT_EQ(group.getSize(), 1);

    group.removeEntityOwned(entity);

    EXPECT_FALSE(group.contains(entity));
    EXPECT_TRUE(group.isEmpty());
}

TEST(Group, ToString_WithRegisteredComponents_OK) {
    System::Registry systems;
    Component::Registry components;
    Entity::Registry entities;
    Event::Registry events;
    World world = makeGroupWorld(systems, components, entities, events);

    Group group(System::Builder("movement", world)
                    .reads<GroupPosition>()
                    .writes<GroupVelocity>()
                    .requireFull(false)
                    .allowPartial(true)
                    .build([](System::Context&) {}));

    EXPECT_NE(group.toString().find("+GroupVelocity"), std::string::npos);
    EXPECT_NE(group.toString().find("~GroupPosition"), std::string::npos);
}
