/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Unit tests of ObjectTree
 */

#include <iostream>

#include <gtest/gtest.h>

#include <Kube/Object/Object.hpp>

using namespace kF;
using namespace kF::Literal;
using namespace kF::ObjectUtils;

TEST(Object, Root)
{
    Tree tree;
    Object root;

    root.parent(tree, Tree::RootIndex, "RootItem"_hash);
    ASSERT_EQ(root.isInTree(), true);
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
    root.removeFromTree();
    ASSERT_EQ(root.isInTree(), false);
    ASSERT_EQ(root.hasParent(), false);
    ASSERT_EQ(root.parent(), nullptr);
}

TEST(Object, BasicTree)
{
    Tree tree;
    Object root, child1, child2, subchild;
    root.parent(tree, Tree::RootIndex, "root"_hash);
    child1.parent(root, "child1"_hash);
    child2.parent(root, "child2"_hash);
    subchild.parent(child1, "subchild"_hash);
    ASSERT_TRUE(root.isInTree());
    ASSERT_TRUE(child1.isInTree());
    ASSERT_TRUE(child2.isInTree());
    ASSERT_TRUE(subchild.isInTree());
    ASSERT_FALSE(root.hasParent());
    ASSERT_TRUE(child1.hasParent());
    ASSERT_TRUE(child2.hasParent());
    ASSERT_TRUE(subchild.hasParent());
    ASSERT_EQ(root.parent(), nullptr);
    ASSERT_EQ(child1.parent(), &root);
    ASSERT_EQ(child2.parent(), &root);
    ASSERT_EQ(subchild.parent(), &child1);

    ASSERT_EQ(root.find("root"_hash), &root);
    ASSERT_EQ(root.find("child1"_hash), &child1);
    ASSERT_EQ(root.find("child2"_hash), &child2);
    ASSERT_EQ(root.find("subchild"_hash), nullptr);

    ASSERT_EQ(child1.find("root"_hash), &root);
    ASSERT_EQ(child1.find("child1"_hash), &child1);
    ASSERT_EQ(child1.find("child2"_hash), &child2);
    ASSERT_EQ(child1.find("subchild"_hash), &subchild);

    ASSERT_EQ(child2.find("root"_hash), &root);
    ASSERT_EQ(child2.find("child1"_hash), &child1);
    ASSERT_EQ(child2.find("child2"_hash), &child2);
    ASSERT_EQ(child2.find("subchild"_hash), nullptr);

    ASSERT_EQ(subchild.find("root"_hash), &root);
    ASSERT_EQ(subchild.find("child1"_hash), &child1);
    ASSERT_EQ(subchild.find("child2"_hash), &child2);
    ASSERT_EQ(subchild.find("subchild"_hash), &subchild);


    ASSERT_EQ(root.findGlobal("root"_hash), &root);
    ASSERT_EQ(root.findGlobal("child1"_hash), &child1);
    ASSERT_EQ(root.findGlobal("child2"_hash), &child2);
    ASSERT_EQ(root.findGlobal("subchild"_hash), &subchild);

    ASSERT_EQ(child1.findGlobal("root"_hash), &root);
    ASSERT_EQ(child1.findGlobal("child1"_hash), &child1);
    ASSERT_EQ(child1.findGlobal("child2"_hash), &child2);
    ASSERT_EQ(child1.findGlobal("subchild"_hash), &subchild);

    ASSERT_EQ(child2.findGlobal("root"_hash), &root);
    ASSERT_EQ(child2.findGlobal("child1"_hash), &child1);
    ASSERT_EQ(child2.findGlobal("child2"_hash), &child2);
    ASSERT_EQ(child2.findGlobal("subchild"_hash), &subchild);

    ASSERT_EQ(subchild.findGlobal("root"_hash), &root);
    ASSERT_EQ(subchild.findGlobal("child1"_hash), &child1);
    ASSERT_EQ(subchild.findGlobal("child2"_hash), &child2);
    ASSERT_EQ(subchild.findGlobal("subchild"_hash), &subchild);
}