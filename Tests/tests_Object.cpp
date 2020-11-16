/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Unit tests of Object
 */

#include <gtest/gtest.h>

// #include <Kube/Object/Object.hpp>

// using namespace kF;
// using namespace kF::Literal;

// class BasicFoo : public Object
// {
//     K_OBJECT(BasicFoo,
//         K_PROPERTY_COPY(int, data, 0),
//         K_SIGNAL(signal, int),
//         K_SIGNAL(signalIncrement, int &),
//         K_SIGNAL(signalSet, int &, int)
//     )

// public:
//     int incrementValue(int &value) { return ++value; }
//     int setValue(int &value, int tmp) { return value = tmp; }
// };

// TEST(Object, FunctorSlots)
// {
//     Meta::Resolver::Clear();
//     RegisterMetadata();
//     BasicFoo::RegisterMetadata();

//     BasicFoo foo, foo2;
//     int x = 0;
//     foo.connect("dataChanged"_hash, [&x] {
//         ++x;
//     });
//     foo2.connect<&BasicFoo::signal>([&foo](int x) {
//         foo.data(x);
//     });
//     foo2.emit<&BasicFoo::signal>(42);
//     ASSERT_EQ(foo.data(), 42);
//     ASSERT_EQ(x, 1);
//     foo2.emit("signal"_hash, 24);
//     ASSERT_EQ(foo.data(), 24);
//     ASSERT_EQ(x, 2);
//     foo2.emit<&BasicFoo::signal>(24);
//     ASSERT_EQ(x, 2);
// }

// TEST(Object, MemberSlots)
// {
//     Meta::Resolver::Clear();
//     RegisterMetadata();
//     BasicFoo::RegisterMetadata();

//     BasicFoo foo, foo2;
//     int x = 0;
//     foo.connect<&BasicFoo::signalIncrement>(foo, &BasicFoo::incrementValue);
//     foo.connect<&BasicFoo::signalSet>(foo2, &BasicFoo::setValue);
//     foo.emit("signalIncrement"_hash, x);
//     ASSERT_EQ(x, 1);
//     foo.emit("signalSet"_hash, x, 42);
//     ASSERT_EQ(x, 42);
//     foo2.emit<&BasicFoo::signalSet>(x, 24); // Using foo2 instead of foo
//     ASSERT_EQ(x, 42);
// }

// TEST(Object, Disconnection)
// {
//     Meta::Resolver::Clear();
//     RegisterMetadata();
//     BasicFoo::RegisterMetadata();

//     BasicFoo foo;
//     int x = 0;
//     auto conn = foo.connect<&BasicFoo::dataChanged>([&x]{ ++x; });
//     foo.emit<&BasicFoo::dataChanged>();
//     ASSERT_EQ(x, 1);
//     foo.disconnect<&BasicFoo::dataChanged>(conn);
//     foo.emit<&BasicFoo::dataChanged>();
//     ASSERT_EQ(x, 1);
// }

// TEST(Object, MemberDisconnection)
// {
//     Meta::Resolver::Clear();
//     RegisterMetadata();
//     BasicFoo::RegisterMetadata();

//     BasicFoo foo, foo2;
//     int x = 0;
//     auto conn = foo.connect<&BasicFoo::dataChanged>(foo2, &BasicFoo::dataChanged);
//     auto conn2 = foo2.connect<&BasicFoo::dataChanged>([&x]{ ++x; });
//     foo.emit<&BasicFoo::dataChanged>();
//     ASSERT_EQ(x, 1);
//     foo.disconnect<&BasicFoo::dataChanged>(conn);
//     foo.emit<&BasicFoo::dataChanged>();
//     ASSERT_EQ(x, 1);
//     foo2.emit<&BasicFoo::dataChanged>();
//     ASSERT_EQ(x, 2);
//     foo2.disconnect<&BasicFoo::dataChanged>(conn2);
//     foo2.emit<&BasicFoo::dataChanged>();
//     ASSERT_EQ(x, 2);
// }

// class AdvancedFoo : public BasicFoo
// {
//     K_OBJECT(AdvancedFoo,
//         K_BASE(BasicFoo),
//         K_PROPERTY_COPY_READONLY(int, readonlyCopyInt, 42),
//         K_SIGNAL(newReadonlyCopyInt, int),
//         K_PROPERTY_MOVE_REF(std::vector<int>, refVectorInt)
//     )

// public:
//     AdvancedFoo(void)
//     {
//         connect<&AdvancedFoo::newReadonlyCopyInt>(*this,
//             static_cast<bool(AdvancedFoo::*)(const int &)>(&AdvancedFoo::readonlyCopyInt)
//         );
//         connect<&AdvancedFoo::refVectorIntChanged>(*this, [this]() {
//             if (!refVectorInt().empty())
//                 readonlyCopyInt(refVectorInt().back());
//         });
//     }
// };

// TEST(Object, AdvancedFoo)
// {
//     AdvancedFoo::RegisterMetadata();
//     AdvancedFoo foo;
//     int trigger = 0;

//     // Base
//     foo.setVar("data"_hash, 123);
//     ASSERT_EQ(foo.data(), 123);

//     // Slot not matching signal
//     foo.connect<&AdvancedFoo::newReadonlyCopyInt>([&trigger] { ++trigger; });

//     // Avanced connection tricks
//     ASSERT_EQ(foo.readonlyCopyInt(), 42);
//     foo.newReadonlyCopyInt(24);
//     ASSERT_EQ(trigger, 1);
//     ASSERT_EQ(foo.readonlyCopyInt(), 24);
//     foo.refVectorInt(std::vector<int>(1, 21));
//     ASSERT_EQ(foo.readonlyCopyInt(), 21);
//     foo.refVectorInt().emplace_back(24);
//     ASSERT_EQ(foo.readonlyCopyInt(), 21);
//     foo.refVectorIntChanged();
//     foo.refVectorInt().clear();
//     ASSERT_EQ(foo.readonlyCopyInt(), 24);
//     foo.refVectorIntChanged();
//     ASSERT_EQ(foo.readonlyCopyInt(), 24);
//     ASSERT_EQ(trigger, 1);
// }