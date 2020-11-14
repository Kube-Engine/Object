/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Item
 */

#pragma once

#include "Object.hpp"

namespace kF
{
    class Item;
}

class kF::Item : public Object
{
    K_OBJECT(Item,
        K_PROPERTY_COPY_READONLY(HashedName, id),
        K_PROPERTY_COPY_READONLY(Item *, parent, nullptr),
        K_PROPERTY_CUSTOM_READONLY(childrenCount, childrenCount),
        K_FUNCTION_OVERLOAD(&Item::getChild, Item &(Item::*)(void))
    )

public:
    /** @brief Get the number of children */
    [[nodiscard]] std::size_t childrenCount(void) const noexcept { return _children.size(); }

    /** @brief Get a child */
    [[nodiscard]] Item &getChild(const std::size_t index) noexcept { return *_children[index]; }
    [[nodiscard]] const Item &getChild(const std::size_t index) const noexcept { return *_children[index]; }


private:
    Core::FlatVector<Item *> _children {};
};


// 1 - Make Getter Setter Notifier if necessary
// 2 - Register Getter Setter Notifier if necessary

// -> make 1 global maker and 1 global registerer

// On demand properties
K_PROPERTY(int, data, 42, K_GET(), K_COPY(), K_SIGNAL());
K_PROPERTY_MEMBER(int, data, _data, K_GET_REF(), K_MOVE(), K_SIGNAL());
K_PROPERTY_MEMBER(int, data, _data, K_GET(), K_READONLY(), K_SIGLESS());
K_PROPERTY_MEMBER(int, data, _data, K_GET(), K_READONLY(), K_SIGLESS());




// Generate a property from nothing
K_PROPERTY_READONLY(int, data, 42) // Read
K_PROPERTY_SET(int, data, 42, &Class::setData) // Read Custom setter
K_PROPERTY_COPY(int, data, 42) // Read Copy
K_PROPERTY_MOVE(int, data, 42) // Read Move
K_PROPERTY_READONLY_SIGNAL(int, data, 42) // Read Signal
K_PROPERTY_SET_SIGNAL(int, data, 42) // Read Copy Signal
K_PROPERTY_COPY_SIGNAL(int, data, 42) // Read Copy Signal
K_PROPERTY_MOVE_SIGNAL(int, data, 42) // Read Move Signal

// Generate a property from a member variable
K_PROPERTY_MEMBER_READONLY(int, data, _data) // Read
K_PROPERTY_MEMBER_SET(int, data, _data, &Class::setData) // Read Move
K_PROPERTY_MEMBER_COPY(int, data, _data) // Read Copy
K_PROPERTY_MEMBER_MOVE(int, data, _data) // Read Move
K_PROPERTY_MEMBER_READONLY_SIGNAL(int, data, _data) // Read Signal
K_PROPERTY_MEMBER_COPY_SIGNAL(int, data, _data) // Read Copy Signal
K_PROPERTY_MEMBER_MOVE_SIGNAL(int, data, _data) // Read Move Signal

// Generate a property from given custom
K_PROPERTY_CUSTOM(int, data, &Class::getData, &Class::setData, &Class::dataChanged)
K_PROPERTY_CUSTOM_READONLY(int, data, &Class::getData)
K_PROPERTY_MEMBER_CUSTOM_SETTER(int, data, &Class::getData, &Class::setData)