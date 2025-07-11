#include <gtest/gtest.h>

#include "reflection/external/primitives.hpp"
#include "reflection/external/string.hpp"
#include "reflection/traits/field.hpp"

using namespace rome;
using namespace rome::core;

struct SimpleStruct {
    i32 integer = 3;
    f32 floating = 3.14f;
    std::string name = "Hello, world!";

    RM_REFLECT;
};
RM_REFLECT_IMPL(SimpleStruct, "SimpleStruct",
                Fields().with("integer", &SimpleStruct::integer).with("floating", &SimpleStruct::floating).with("name", &SimpleStruct::name));

class StandardLayoutClass {
    public:
    StandardLayoutClass() = default;
    // virtual ~StandardLayoutClass(); big nono
    ~StandardLayoutClass() = default;

    private:
    static i32 abc();

    byte randomByte = 'Z';

    RM_REFLECT;
};
RM_REFLECT_IMPL(StandardLayoutClass, "StandardLayoutClass", Fields().with("randomByte", &StandardLayoutClass::randomByte));

TEST(TraitReflectionTest, SimpleStructFields) {
    // Obtain reflection info
    const Type& type = Reflect::reflect<SimpleStruct>();

    // Check that the name is as declared
    EXPECT_EQ(type.getName(), "SimpleStruct");

    // Check that the UUID in the type instance matches its static UUID
    EXPECT_EQ(type.getUUID(), Type::getUUID<SimpleStruct>());

    SimpleStruct instance;

    // Check that the field UUID's, names, and values are correct
    for (const auto& field : type.getTrait<Fields>()) {
        if (std::string(field.getName()) == "integer") {
            EXPECT_EQ(field.getType().getUUID(), Type::getUUID<i32>());
            EXPECT_EQ(*field.getValue<i32>(&instance), 3);
        } else if (std::string(field.getName()) == "floating") {
            EXPECT_EQ(field.getType().getUUID(), Type::getUUID<f32>());
            EXPECT_FLOAT_EQ(*field.getValue<f32>(&instance), 3.14f);
        } else if (std::string(field.getName()) == "name") {
            EXPECT_EQ(field.getType().getUUID(), Type::getUUID<std::string>());
            EXPECT_EQ(*field.getValue<std::string>(&instance), "Hello, world!");
        } else {
            FAIL() << "Unexpected field: " << field.getName();
        }
    }

    Type& typeMut = Reflect::reflect<SimpleStruct>();

    // Check that the field can be found by name
    EXPECT_EQ(typeMut.getTrait<Fields>().find("integer")->getType().getUUID(), Type::getUUID<i32>());

    // Modify it and check that the value is updated
    *(typeMut.getTrait<Fields>().find("integer")->getValue<i32>(&instance)) = 42;
    EXPECT_EQ(instance.integer, 42);
}

TEST(TraitReflectionTest, StandardLayoutClassFields) {
    // Obtain reflection info
    const Type& type = Reflect::reflect<StandardLayoutClass>();

    // Check that the name is as declared
    EXPECT_EQ(type.getName(), "StandardLayoutClass");

    // Check that the UUID in the type instance matches its static UUID
    EXPECT_EQ(type.getUUID(), Type::getUUID<StandardLayoutClass>());

    StandardLayoutClass instance;

    // Check that the field UUID's, names, and values are correct
    for (const auto& field : type.getTrait<Fields>()) {
        if (std::string(field.getName()) == "randomByte") {
            EXPECT_EQ(field.getType().getUUID(), Type::getUUID<byte>());
            EXPECT_EQ(*field.getValue<byte>(&instance), 'Z');
        } else {
            FAIL() << "Unexpected field: " << field.getName();
        }
    }

    Type& typeMut = Reflect::reflect<StandardLayoutClass>();

    // Check that the field can be found by name
    EXPECT_EQ(typeMut.getTrait<Fields>().find("randomByte")->getType().getUUID(), Type::getUUID<byte>());
}