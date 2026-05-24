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

struct GroupA {
    u32 value;

    RM_REFLECT;
};
RM_REFLECT_IMPL(GroupA, "GroupA", Fields().with("value", &GroupA::value));

struct GroupB {
    u32 value;

    RM_REFLECT;
};
RM_REFLECT_IMPL(GroupB, "GroupB", Fields().with("value", &GroupB::value));

struct GroupC {
    u32 value;

    RM_REFLECT;
};
RM_REFLECT_IMPL(GroupC, "GroupC", Fields().with("value", &GroupC::value));

static World makeGroupWorld(System::Registry& systems, Component::Registry& components, Entity::Registry& entities, Event::Registry& events) {
    return World{systems, components, entities, events};
}

static void expectContains(const Group& group, std::initializer_list<Entity> present, std::initializer_list<Entity> absent) {
    EXPECT_EQ(group.getSize(), present.size());
    for (const Entity& entity : present) EXPECT_TRUE(group.contains(entity));
    for (const Entity& entity : absent) EXPECT_FALSE(group.contains(entity));
}

template <Component::Component T, typename... Args>
static void addGroupComponent(Component::Registry& components, const Entity& entity, Args&&... args) {
    ASSERT_NE(components.emplace<T>(entity, std::forward<Args>(args)...), nullptr);
}

TEST(Group, Constructors_FullOwning_OK) {
    System::Registry systems;
    Component::Registry components;
    Entity::Registry entities;
    Event::Registry events;
    World world = makeGroupWorld(systems, components, entities, events);

    System::Descriptor descriptor =
        System::Builder("movement", world).reads<Position>().writes<Velocity>().requireFull().build([](System::Context&) {});
    Group group(descriptor);

    EXPECT_TRUE(group.owning.test(components.submit<Position>()));
    EXPECT_TRUE(group.owning.test(components.submit<Velocity>()));
    EXPECT_TRUE(group.partial.none());
    EXPECT_TRUE(group.isEmpty());
}

TEST(Group, Constructors_Partial_OK) {
    System::Registry systems;
    Component::Registry components;
    Entity::Registry entities;
    Event::Registry events;
    World world = makeGroupWorld(systems, components, entities, events);

    System::Descriptor descriptor =
        System::Builder("movement", world).reads<Position>().writes<Velocity>().allowPartial().build([](System::Context&) {});
    Group group(descriptor);

    EXPECT_TRUE(group.owning.test(components.submit<Velocity>()));
    EXPECT_TRUE(group.partial.test(components.submit<Position>()));
}

TEST(Group, Constructors_NonOwning_OK) {
    System::Registry systems;
    Component::Registry components;
    Entity::Registry entities;
    Event::Registry events;
    World world = makeGroupWorld(systems, components, entities, events);

    System::Descriptor descriptor =
        System::Builder("watch", world).reads<Position, Velocity>().allowPartial().build([](System::Context&) {});
    Group group(descriptor);

    EXPECT_TRUE(group.owning.none());
    EXPECT_TRUE(group.partial.test(components.submit<Position>()));
    EXPECT_TRUE(group.partial.test(components.submit<Velocity>()));
}

TEST(Group, AddRemoveEntity_Always_OK) {
    System::Registry systems;
    Component::Registry components;
    Entity::Registry entities;
    Event::Registry events;
    World world = makeGroupWorld(systems, components, entities, events);
    Group group(System::Builder("movement", world).writes<Position>().build([](System::Context&) {}));
    Entity entity = entities.create();

    group.addEntity(entity);

    EXPECT_TRUE(group.contains(entity));
    ASSERT_EQ(group.getSize(), 1);
    EXPECT_EQ(*group.getEntities()[entity.getIndex()], entity);

    group.removeEntity(entity);

    EXPECT_FALSE(group.contains(entity));
    EXPECT_TRUE(group.isEmpty());
}

TEST(Group, RemoveEntity_Middle_KeepsMoved) {
    System::Registry systems;
    Component::Registry components;
    Entity::Registry entities;
    Event::Registry events;
    World world = makeGroupWorld(systems, components, entities, events);
    Group group(System::Builder("movement", world).writes<Position>().build([](System::Context&) {}));
    Entity first = entities.create();
    Entity second = entities.create();
    Entity third = entities.create();

    group.addEntity(first);
    group.addEntity(second);
    group.addEntity(third);
    group.removeEntity(second);

    EXPECT_TRUE(group.contains(first));
    EXPECT_FALSE(group.contains(second));
    EXPECT_TRUE(group.contains(third));
    ASSERT_EQ(group.getSize(), 2);

    group.removeEntity(third);

    EXPECT_TRUE(group.contains(first));
    EXPECT_FALSE(group.contains(third));
    EXPECT_EQ(group.getSize(), 1);
}

TEST(Group, AddRemoveEntity_WithMatchingArchetype_OK) {
    System::Registry systems;
    Component::Registry components;
    Entity::Registry entities;
    Event::Registry events;
    World world = makeGroupWorld(systems, components, entities, events);
    Group group(System::Builder("movement", world).writes<Position>().requireFull().build([](System::Context&) {}));
    Entity entity = entities.create();
    ASSERT_NE(components.emplace<Position>(entity, 1.0f, 2.0f), nullptr);

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

    Group group(System::Builder("movement", world).reads<Position>().writes<Velocity>().allowPartial().build([](System::Context&) {}));

    EXPECT_NE(group.toString().find("+Velocity"), std::string::npos);
    EXPECT_NE(group.toString().find("~Position"), std::string::npos);
}

TEST(Group, MixedGroups_AllArchetypes_MatchExpected) {
    System::Registry systems;
    Component::Registry components;
    Entity::Registry entities;
    Event::Registry events;
    World world = makeGroupWorld(systems, components, entities, events);

    Entity a = entities.create();
    addGroupComponent<GroupA>(components, a, 1);

    Entity b = entities.create();
    addGroupComponent<GroupB>(components, b, 2);

    Entity c = entities.create();
    addGroupComponent<GroupC>(components, c, 3);

    Entity ab = entities.create();
    addGroupComponent<GroupA>(components, ab, 4);
    addGroupComponent<GroupB>(components, ab, 5);

    Entity ac = entities.create();
    addGroupComponent<GroupA>(components, ac, 6);
    addGroupComponent<GroupC>(components, ac, 7);

    Entity bc = entities.create();
    addGroupComponent<GroupB>(components, bc, 8);
    addGroupComponent<GroupC>(components, bc, 9);

    Entity abc = entities.create();
    addGroupComponent<GroupA>(components, abc, 10);
    addGroupComponent<GroupB>(components, abc, 11);
    addGroupComponent<GroupC>(components, abc, 12);

    System::ID fullAB =
        systems.enter(System::Builder("fullAB", world).reads<GroupA>().writes<GroupB>().requireFull().build([](System::Context&) {}));
    System::ID partialAC = systems.enter(
        System::Builder("partialAC", world).reads<GroupC>().writes<GroupA>().allowPartial().build([](System::Context&) {}));
    System::ID nonOwningBC =
        systems.enter(System::Builder("nonOwningBC", world).reads<GroupB, GroupC>().allowPartial().build([](System::Context&) {}));

    expectContains(systems.getGroup(fullAB), {ab, abc}, {a, b, c, ac, bc});
    expectContains(systems.getGroup(partialAC), {ac, abc}, {a, b, c, ab, bc});
    expectContains(systems.getGroup(nonOwningBC), {bc, abc}, {a, b, c, ab, ac});
}

TEST(Group, MixedGroups_ComponentChanges_UpdateExpected) {
    System::Registry systems;
    Component::Registry components;
    Entity::Registry entities;
    Event::Registry events;
    World world = makeGroupWorld(systems, components, entities, events);

    System::ID fullAB =
        systems.enter(System::Builder("fullAB", world).reads<GroupA>().writes<GroupB>().requireFull().build([](System::Context&) {}));
    System::ID partialAC = systems.enter(
        System::Builder("partialAC", world).reads<GroupC>().writes<GroupA>().allowPartial().build([](System::Context&) {}));
    System::ID nonOwningBC =
        systems.enter(System::Builder("nonOwningBC", world).reads<GroupB, GroupC>().allowPartial().build([](System::Context&) {}));

    Entity entity = entities.create();

    addGroupComponent<GroupA>(components, entity, 1);
    systems.updateEntity(entity);
    expectContains(systems.getGroup(fullAB), {}, {entity});
    expectContains(systems.getGroup(partialAC), {}, {entity});
    expectContains(systems.getGroup(nonOwningBC), {}, {entity});

    addGroupComponent<GroupB>(components, entity, 2);
    systems.updateEntity(entity);
    expectContains(systems.getGroup(fullAB), {entity}, {});
    expectContains(systems.getGroup(partialAC), {}, {entity});
    expectContains(systems.getGroup(nonOwningBC), {}, {entity});

    addGroupComponent<GroupC>(components, entity, 3);
    systems.updateEntity(entity);
    expectContains(systems.getGroup(fullAB), {entity}, {});
    expectContains(systems.getGroup(partialAC), {entity}, {});
    expectContains(systems.getGroup(nonOwningBC), {entity}, {});

    components.remove<GroupA>(entity);
    systems.updateEntity(entity);
    expectContains(systems.getGroup(fullAB), {}, {entity});
    expectContains(systems.getGroup(partialAC), {}, {entity});
    expectContains(systems.getGroup(nonOwningBC), {entity}, {});

    components.remove<GroupC>(entity);
    systems.updateEntity(entity);
    expectContains(systems.getGroup(fullAB), {}, {entity});
    expectContains(systems.getGroup(partialAC), {}, {entity});
    expectContains(systems.getGroup(nonOwningBC), {}, {entity});
}
