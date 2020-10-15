/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Reflection interface for KUBE
 */

#include "Object.hpp"
// #include "String.hpp"
// #include "Vector.hpp"
// #include "Item.hpp"

void kF::RegisterMetadata(void)
{
    Meta::RegisterMetadata();
    Object::RegisterMetadata();
    // String::RegisterMetadata();
    // Vector::RegisterMetadata();
    // Item::RegisterMetadata();
}
