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
        K_PROPERTY_COPY_READONLY(Item *, parent, nullptr),
        K_PROPERTY_CUSTOM_READONLY(childrenCount, childrenCount),
        K_SIGNAL(childrenCountChanged)
    )

public:
    [[nodiscard]] std::size_t childrenCount(void) const noexcept { return _children.size(); }

private:
    Core::FlatVector<Item *> _children {};
};