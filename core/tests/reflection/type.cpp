#include <gtest/gtest.h>

#include "reflection/primitives.hpp"
#include "reflection/reflect.hpp"
#include "reflection/string.hpp"

// Automate the test for all simple types
#define STRINGIFY(x) #x
#define TEST_REFLECTION(type, name)                           \
    {                                                         \
        const auto& typeInfo = reflect<type>();               \
        EXPECT_EQ(typeInfo.getName(), name);                  \
        EXPECT_EQ(typeInfo.getUUID(), Type::getUUID<type>()); \
    }
#define TEST_ALL_REFLECTIONS(type)                          \
    TEST_REFLECTION(type, STRINGIFY(type));                 \
    TEST_REFLECTION(type*, STRINGIFY(type*));               \
    TEST_REFLECTION(type**, STRINGIFY(type**));             \
    TEST_REFLECTION(type&, STRINGIFY(type&));               \
    TEST_REFLECTION(type&&, STRINGIFY(type&&));             \
    TEST_REFLECTION(type[], STRINGIFY(type[]));             \
    TEST_REFLECTION(const type, STRINGIFY(const type));     \
    TEST_REFLECTION(const type*, STRINGIFY(const type*));   \
    TEST_REFLECTION(const type**, STRINGIFY(const type**)); \
    TEST_REFLECTION(const type&, STRINGIFY(const type&));   \
    TEST_REFLECTION(const type&&, STRINGIFY(const type&&)); \
    TEST_REFLECTION(const type[], STRINGIFY(const type[]));

using namespace iodine;

struct SimpleStruct {
    i32 integer;
    f32 floating;
    std::string name;
};
IO_REFLECT(SimpleStruct, "SimpleStruct");

class SimpleClass {
    public:
    SimpleClass();
    virtual ~SimpleClass();

    private:
    static i32 abc();

    byte data[16];
};
IO_REFLECT(SimpleClass, "SimpleClass");

using namespace iodine::core;

TEST(ReflectionTest, StructHasCorrectReflection) {
    // Obtain reflection info
    const auto& type = reflect<SimpleStruct>();

    // Check that the name is as declared
    EXPECT_EQ(type.getName(), "SimpleStruct");

    // Check that the UUID in the type instance matches its static UUID
    EXPECT_EQ(type.getUUID(), Type::getUUID<SimpleStruct>());
}

TEST(ReflectionTest, ClassHasCorrectReflection) {
    // Obtain reflection info
    const auto& type = reflect<SimpleClass>();

    // Check that the name is as declared
    EXPECT_EQ(type.getName(), "SimpleClass");

    // Check that the UUID in the type instance matches its static UUID
    EXPECT_EQ(type.getUUID(), Type::getUUID<SimpleClass>());
}

TEST(ReflectionTest, PrimitivesHaveCorrectReflection) {
    TEST_ALL_REFLECTIONS(iodine::u8);
    TEST_ALL_REFLECTIONS(iodine::u16);
    TEST_ALL_REFLECTIONS(iodine::u32);
    TEST_ALL_REFLECTIONS(iodine::u64);
    TEST_ALL_REFLECTIONS(iodine::i8);
    TEST_ALL_REFLECTIONS(iodine::i16);
    TEST_ALL_REFLECTIONS(iodine::i32);
    TEST_ALL_REFLECTIONS(iodine::i64);
    TEST_ALL_REFLECTIONS(iodine::f32);
    TEST_ALL_REFLECTIONS(iodine::f64);
}

TEST(ReflectionTest, StringHasCorrectReflection) { TEST_ALL_REFLECTIONS(std::string); }