/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Unit tests of Object
 */

#include <iostream>

#include <gtest/gtest.h>

#include <Kube/Object/Object.hpp>

using namespace kF;
using namespace kF::Literal;
using namespace kF::ObjectUtils;

TEST(Object, BasicTree)
{
    Tree tree;
    Object root;

    root.parent(tree, Tree::RootIndex, "RootItem"_hash);
    ASSERT_EQ(root.hasParent(), false);
    ASSERT_EQ(root.parent(), nullptr);
    ASSERT_EQ(root.parentUnsafe(), nullptr);
    ASSERT_EQ(root.id(), "RootItem"_hash);
    root.id("RootItem2"_hash);
    ASSERT_EQ(root.id(), "RootItem2"_hash);
    ASSERT_EQ(root.find("NotExisting"_hash), nullptr);
    ASSERT_EQ(root.findGlobal("NotExisting"_hash), nullptr);
    ASSERT_EQ(root.find("RootItem2"_hash), &root);
    ASSERT_EQ(root.findGlobal("RootItem2"_hash), &root);
}