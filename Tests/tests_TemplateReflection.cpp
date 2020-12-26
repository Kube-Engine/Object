/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Unit tests of Object
 */

#include <iostream>

#include <gtest/gtest.h>

#include <Kube/Object/Object.hpp>

using namespace kF;
using namespace kF::Literal;

template<typename Type>
struct TemplateFoo
{
    K_ABSTRACT(TemplateFoo,
        K_PROPERTY_SIGLESS(Type, value)
    )
};

// Instantiate the class
template class TemplateFoo<int>;

TEST(Template, Basics)
{
    Meta::Resolver::Clear();
    RegisterMetadata();

    auto name = "TemplateFoo"_hash;
    auto tname = "int"_hash;
    auto sname = name + tname;

    auto type = Meta::Resolver::FindTemplateSpecialization(
        name, sname
    );
    ASSERT_TRUE(type);
    auto foo = type.defaultConstruct();

    auto data = type.findData("value"_hash);
    ASSERT_TRUE(data);
    auto value = data.get(foo);
    ASSERT_TRUE(value);
    ASSERT_EQ(value.cast<int>(), 0);
    auto res = data.set(foo, 42);
    ASSERT_TRUE(res);
    ASSERT_TRUE(res.isVoid());
    value = data.get(foo);
    ASSERT_TRUE(value);
    ASSERT_EQ(value.cast<int>(), 42);
}