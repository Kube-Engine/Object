/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Unit tests of Object
 */

#include <gtest/gtest.h>

#include <Kube/Object/Object.hpp>

using namespace kF;
using namespace kF::Literal;

class BasicFoo : public Object
{
    KUBE_MAKE_PROPERTY_COPY(int, data, 0)
    KUBE_MAKE_SIGNAL(signal, int)
    KUBE_MAKE_SIGNAL(signalIncrement, int &)
    KUBE_MAKE_SIGNAL(signalSet, int &, int)

    int incrementValue(int &value) { return ++value; }
    int setValue(int &value, int tmp) { return value = tmp; }

    KUBE_REGISTER(BasicFoo,
        KUBE_REGISTER_PROPERTY_COPY(int, data)
        KUBE_REGISTER_SIGNAL(signal)
    )
};

TEST(Object, FunctorSlots)
{
    Meta::Resolver::Clear();
    RegisterMetadata();
    BasicFoo::RegisterMetadata();

    BasicFoo foo, foo2;
    int x = 0;
    foo.connect("data"_hash, [&x] {
        ++x;
    });
    foo2.connect<&BasicFoo::signal>([&foo](int x) {
        foo.data(x);
    });
    foo2.emit<&BasicFoo::signal>(42);
    ASSERT_EQ(foo.data(), 42);
    ASSERT_EQ(x, 1);
    foo2.emit("signal"_hash, 24);
    ASSERT_EQ(foo.data(), 24);
    ASSERT_EQ(x, 2);
    foo2.emit<&BasicFoo::signal>(24);
    ASSERT_EQ(x, 2);
}

TEST(Object, MemberSlots)
{
    Meta::Resolver::Clear();
    RegisterMetadata();
    BasicFoo::RegisterMetadata();

    BasicFoo foo, foo2;
    int x = 0;
    foo.connect<&BasicFoo::signalIncrement>(foo, &BasicFoo::incrementValue);
    foo.connect<&BasicFoo::signalSet>(foo2, &BasicFoo::setValue);
    foo.emit("signalIncrement"_hash, x);
    ASSERT_EQ(x, 1);
    foo.emit("signalSet"_hash, x, 42);
    ASSERT_EQ(x, 42);
    foo2.emit<&BasicFoo::signalSet>(x, 24); // Using foo2 instead of foo
    ASSERT_EQ(x, 42);
}