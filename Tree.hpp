/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: A tree of objects
 */

#pragma once

#include <Kube/Core/SmallVector.hpp>
#include <Kube/Core/Vector.hpp>
#include <Kube/Core/Hash.hpp>

namespace kF
{
    class Object;

    namespace ObjectUtils
    {
        class Tree;
    }
}

/** @brief A tree of objects which comes in form of a big array for fast search
 *
 *  This class provides stable indexing (nodes' indexes) but NOT stable addressing (nodes' pointers)
 *  This class is not thread safe at all
*/
class alignas_cacheline kF::ObjectUtils::Tree
{
public:
    enum class Flags : std::uint16_t {
        None = 0b0,
        MouseHandler = 0b1,
        KeyHandler = 0b10,
        DrawHandler = 0b100
    };

    /** @brief Index of a node in the tree */
    using Index = std::uint32_t;

    /** @brief The root index (which is a non-object) */
    static constexpr Index RootIndex = 0u;

    /** @brief The null index */
    static constexpr Index NullIndex = std::numeric_limits<Index>::max();

    /** @brief Number of nodes in the table on startup */
    static constexpr Index DefaultTableSize = 4096u;

    /** @brief Represent an object in a node tree */
    struct alignas_cacheline Node
    {
        using Children = Core::SmallVector<Index, 24u / sizeof(Index), Index>;

        Object *object { nullptr };
        HashedName id { 0u };
        Index parentIndex { NullIndex };
        bool enabled { true }; // False will prevent propagating event
        bool visible { true }; // False will prevent propagating rendering
        Flags flags { Flags::None };
        Children children {};
    };

    static_assert_fit_cacheline(Node);

    /** @brief Default constructor */
    Tree(void) noexcept;

    /** @brief Move constructor */
    Tree(Tree &&other) noexcept = default;

    /** @brief Destructor */
    ~Tree(void) noexcept = default;

    /** @brief Move assignment */
    Tree &operator=(Tree &&other) noexcept = default;


    /** @brief Get a node of the tree using its index */
    [[nodiscard]] Node &get(const Index index) noexcept
        { return _nodes[index]; }
    [[nodiscard]] const Node &get(const Index index) const noexcept
        { return _nodes[index]; }


    /** @brief Adds a node into the tree */
    [[nodiscard]] Index add(const Index parentIndex, Object * const object, const HashedName id, const Flags flags) noexcept;

    /** @brief Removes a node from the tree */
    void remove(const Index index) noexcept;


    /** @brief Replace index of a node for another existing one */
    void setParent(const Index index, const Index parentIndex) noexcept;


    /** @brief Find a node using its hashed name
     *  Be aware that id can collide and thus, the function will return the first match */
    [[nodiscard]] Index find(const HashedName id) const noexcept;

    /** @brief Find a node using its hashed name and a starting point
     *  This function will search in 'from' close children then in every parent and their close children
     *  This mean you can't access a sub-child or the sub-child of a parent
     *  Be aware that id can collide and thus, the function will return the first match */
    [[nodiscard]] Index findInScope(const HashedName id, const Index from) const noexcept;


    /** @brief Check if the tree has changed */
    [[nodiscard]] bool isTreeDirty(void) const noexcept { return _treeDirty; }

    /** @brief Set the tree dirty flag */
    void setTreeDirtyFlag(const bool value) noexcept { _treeDirty = value; }


    /** @brief Check if the tree event-ability has changed */
    [[nodiscard]] bool isEnabledDirty(void) const noexcept { return _enabledDirty; }

    /** @brief Set the tree event-ability dirty flag */
    void setEnabledDirtyFlag(const bool value) noexcept { _enabledDirty = value; }


    /** @brief Check if the tree visibility has changed */
    [[nodiscard]] bool isVisibleDirty(void) const noexcept { return _visibleDirty; }

    /** @brief Set the tree event-ability dirty flag */
    void setVisibleDirtyFlag(const bool value) noexcept { _visibleDirty = value; }

private:
    Core::Vector<Node, Index> _nodes {};
    Core::Vector<Index, Index> _freeList {};
    bool _treeDirty { false };
    bool _enabledDirty { false };
    bool _visibleDirty { false };
    std::byte _padding {};

    /** @brief Set every dirty flags to true */
    void setAllDirtyFlags(void) noexcept;
};

static_assert_fit_cacheline(kF::ObjectUtils::Tree);

#include "Tree.ipp"