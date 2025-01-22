#include <gtest/gtest.h>

#include "reflection/external/primitives.hpp"
#include "reflection/external/string.hpp"

// Automate the test for all simple types
#define STRINGIFY(x) #x
#define TEST_REFLECTION(type, name)                                     \
    {                                                                   \
        auto typeInfo = Reflect::reflect<type>();                       \
        EXPECT_EQ(typeInfo.getType().getName(), name);                  \
        EXPECT_EQ(typeInfo.getType().getUUID(), Type::getUUID<type>()); \
    }
#define TEST_ALL_REFLECTIONS(type)                  \
    TEST_REFLECTION(type, STRINGIFY(type));         \
    TEST_REFLECTION(type*, STRINGIFY(type));        \
    TEST_REFLECTION(type**, STRINGIFY(type));       \
    TEST_REFLECTION(type&, STRINGIFY(type));        \
    TEST_REFLECTION(type&&, STRINGIFY(type));       \
    TEST_REFLECTION(type[], STRINGIFY(type));       \
    TEST_REFLECTION(const type, STRINGIFY(type));   \
    TEST_REFLECTION(const type*, STRINGIFY(type));  \
    TEST_REFLECTION(const type**, STRINGIFY(type)); \
    TEST_REFLECTION(const type&, STRINGIFY(type));  \
    TEST_REFLECTION(const type&&, STRINGIFY(type)); \
    TEST_REFLECTION(const type[], STRINGIFY(type));

using namespace iodine;

struct SimpleStruct {
    i32 integer;
    f32 floating;
    std::string name;

    IO_REFLECT;
};
IO_REFLECT_IMPL(SimpleStruct, "SimpleStruct");

class SimpleClass {
    public:
    SimpleClass();
    virtual ~SimpleClass();

    private:
    static i32 abc();

    byte data[16];

    IO_REFLECT;
};
IO_REFLECT_IMPL(SimpleClass, "SimpleClass");

using namespace iodine::core;

TEST(TypeReflectionTest, StructHasCorrectReflection) {
    // Obtain reflection info
    const Type& type = Reflect::reflect<SimpleStruct>();

    // Check that the name is as declared
    EXPECT_EQ(type.getName(), "SimpleStruct");

    // Check that the UUID in the type instance matches its static UUID
    EXPECT_EQ(type.getUUID(), Type::getUUID<SimpleStruct>());
}

TEST(TypeReflectionTest, ClassHasCorrectReflection) {
    // Obtain reflection info
    const Type& type = Reflect::reflect<SimpleClass>();

    // Check that the name is as declared
    EXPECT_EQ(type.getName(), "SimpleClass");

    // Check that the UUID in the type instance matches its static UUID
    EXPECT_EQ(type.getUUID(), Type::getUUID<SimpleClass>());
}

TEST(TypeReflectionTest, TypeQualifierStripping) {
    // Obtain reflection info for various different types and check that the qualifiers are stripped
    TypeInfo& simpleType = Reflect::reflect<iodine::u8>();

    // Adding more complex type variations with multiple qualifiers
    TypeInfo& complexType1 = Reflect::reflect<iodine::u8**>();
    TypeInfo& complexType2 = Reflect::reflect<const volatile iodine::u8* [42]>();
    TypeInfo& complexType3 = Reflect::reflect<iodine::u8* const volatile&>();
    TypeInfo& complexType4 = Reflect::reflect<iodine::u8* const* volatile*>();
    TypeInfo& complexType5 = Reflect::reflect<const iodine::u8* const volatile&>();
    TypeInfo& complexType6 = Reflect::reflect<iodine::u8&&>();
    TypeInfo& complexType7 = Reflect::reflect<const iodine::u8* const volatile*[]>();
    TypeInfo& complexType8 = Reflect::reflect<iodine::u8* const volatile* const&>();
    TypeInfo& complexType9 = Reflect::reflect<const iodine::u8* const volatile* const* volatile&>();
    TypeInfo& complexType10 = Reflect::reflect<iodine::u8* const* volatile* const* volatile&>();

    // Ensure that the base type is the same after stripping qualifiers
    EXPECT_EQ(simpleType.getType(), complexType1.getType());
    EXPECT_EQ(simpleType.getType(), complexType2.getType());
    EXPECT_EQ(simpleType.getType(), complexType3.getType());
    EXPECT_EQ(simpleType.getType(), complexType4.getType());
    EXPECT_EQ(simpleType.getType(), complexType5.getType());
    EXPECT_EQ(simpleType.getType(), complexType6.getType());
    EXPECT_EQ(simpleType.getType(), complexType7.getType());
    EXPECT_EQ(simpleType.getType(), complexType8.getType());
    EXPECT_EQ(simpleType.getType(), complexType9.getType());
    EXPECT_EQ(simpleType.getType(), complexType10.getType());
}

TEST(TypeReflectionTest, TypeInfoHasCorrectMetadata) {
    // Basic type has no special qualifiers
    TypeInfo& type1 = Reflect::reflect<iodine::u8>();

    EXPECT_FALSE(type1.isConstType());
    EXPECT_FALSE(type1.isVolatileType());
    EXPECT_FALSE(type1.isPointerType());
    EXPECT_FALSE(type1.isArrayType());
    EXPECT_FALSE(type1.isReferenceType());
    EXPECT_FALSE(type1.isRValueReferenceType());
    EXPECT_FALSE(type1.isLValueReferenceType());

    // Both const and the array qualifier apply to the pointee.
    TypeInfo& type2 = Reflect::reflect<const iodine::u8[127]>();

    EXPECT_TRUE(type2.isConstType());
    EXPECT_FALSE(type2.isVolatileType());
    EXPECT_FALSE(type2.isPointerType());
    EXPECT_TRUE(type2.isArrayType());
    EXPECT_FALSE(type2.isReferenceType());
    EXPECT_FALSE(type2.isRValueReferenceType());
    EXPECT_FALSE(type2.isLValueReferenceType());

    // Volatile applies to the pointer, not the pointee. Same for the pointer.
    TypeInfo& type3 = Reflect::reflect<volatile iodine::u8*&>();

    EXPECT_FALSE(type3.isConstType());
    EXPECT_FALSE(type3.isVolatileType());
    EXPECT_FALSE(type3.isPointerType());
    EXPECT_FALSE(type3.isArrayType());
    EXPECT_TRUE(type3.isReferenceType());
    EXPECT_FALSE(type3.isRValueReferenceType());
    EXPECT_TRUE(type3.isLValueReferenceType());
}

TEST(TypeReflectionTest, PrimitivesHaveCorrectReflection) {
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