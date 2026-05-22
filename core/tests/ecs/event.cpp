#include <gtest/gtest.h>

#include "rm/ecs/component/registry.hpp"
#include "rm/ecs/entity/registry.hpp"
#include "rm/ecs/event/bus.hpp"
#include "rm/ecs/event/registry.hpp"
#include "rm/ecs/system/registry.hpp"
#include "rm/ecs/world.hpp"
#include "rm/reflection/traits/field.hpp"

using namespace rome;
using namespace rome::core;

struct TestEvent {
    i32 value;

    RM_REFLECT;
};
RM_REFLECT_IMPL(TestEvent, "TestEvent", Fields().with("value", &TestEvent::value));

struct OtherTestEvent {
    i32 value;

    RM_REFLECT;
};
RM_REFLECT_IMPL(OtherTestEvent, "OtherTestEvent", Fields().with("value", &OtherTestEvent::value));

TEST(EventRegistry, Enter_DuplicateEvent_OK) {
    Event::Registry registry;

    Event::ID id0 = registry.enter("TestEvent");
    Event::ID id1 = registry.enter("TestEvent");

    EXPECT_NE(id0, Event::INVALID_ID);
    EXPECT_EQ(id0, id1);
    EXPECT_TRUE(registry.contains("TestEvent"));
}

TEST(EventRegistry, Enter_DifferentEvents_DifferentIDs) {
    Event::Registry registry;

    Event::ID first = registry.enter("TestEvent");
    Event::ID second = registry.enter("OtherTestEvent");

    EXPECT_NE(first, Event::INVALID_ID);
    EXPECT_NE(second, Event::INVALID_ID);
    EXPECT_NE(first, second);
}

TEST(EventRegistry, Get_OutOfBounds_ReturnsNullID) {
    Event::Registry registry;

    EXPECT_EQ(registry.get("MissingEvent"), Event::INVALID_ID);
}

TEST(EventStorage, PushSwapRead_Always_OK) {
    Event::Storage<TestEvent> storage;

    EXPECT_TRUE(storage.empty());

    storage.push(TestEvent{1});
    EXPECT_FALSE(storage.empty());
    EXPECT_TRUE(storage.read().empty());

    storage.swap();

    ASSERT_EQ(storage.read().size(), 1);
    EXPECT_EQ(storage.read()[0].value, 1);

    storage.emplace(2);
    storage.swap();

    ASSERT_EQ(storage.read().size(), 1);
    EXPECT_EQ(storage.read()[0].value, 2);
}

TEST(EventBus, Queue_Always_OK) {
    System::Registry systems;
    Component::Registry components;
    Entity::Registry entities;
    Event::Registry events;
    World world{systems, components, entities, events};
    events.enter(Reflect::reflect<TestEvent>().getType().getName());
    Event::Bus bus(world);

    bus.enter<TestEvent>();

    bus.queue<TestEvent>().push(TestEvent{42});
    EXPECT_TRUE(bus.queue<TestEvent>().read().empty());

    bus.swap();

    ASSERT_EQ(bus.queue<TestEvent>().read().size(), 1);
    EXPECT_EQ(bus.queue<TestEvent>().read()[0].value, 42);
}

TEST(EventBus, Enter_Duplicate_Throws) {
    System::Registry systems;
    Component::Registry components;
    Entity::Registry entities;
    Event::Registry events;
    World world{systems, components, entities, events};
    events.enter(Reflect::reflect<TestEvent>().getType().getName());
    Event::Bus bus(world);

    bus.enter<TestEvent>();

    EXPECT_THROW(bus.enter<TestEvent>(), Exception);
}

TEST(EventBus, Queue_MissingQueue_Throws) {
    System::Registry systems;
    Component::Registry components;
    Entity::Registry entities;
    Event::Registry events;
    World world{systems, components, entities, events};
    events.enter(Reflect::reflect<TestEvent>().getType().getName());
    Event::Bus bus(world);

    EXPECT_THROW(bus.queue<TestEvent>(), Exception);
}
