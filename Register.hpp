/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Registeres generation helpers
 */

#pragma once

#include "MacroUtils.hpp"

/** @brief Register a new type (already used by 'KUBE_REGISTER' and 'KUBE_REGISTER_INSTANTIABLE') */
#define KUBE_REGISTER_TYPE(ClassType, ...) \
    kF::Meta::Factory<ClassType>(kF::Hash(#ClassType))

/** @brief Register a base type */
#define KUBE_REGISTER_BASE(BaseType) \
    .base<BaseType>()

/** @brief Register a custom constructor (not default / copy / move) */
#define KUBE_REGISTER_CONSTRUCTOR(...) \
    .constructor<__VA_ARGS__>();

/** @brief Register a copy only property */
#define KUBE_REGISTER_PROPERTY_COPY(PropertyType, name, ...) \
    .data< \
        static_cast<const PropertyType &(_MetaType::*)(void) const noexcept>(&_MetaType::name), \
        static_cast<bool(_MetaType::*)(const PropertyType &)>(&_MetaType::name) \
    >(kF::Hash(#name)) \
    KUBE_REGISTER_SIGNAL(name##Changed)
#define KUBE_REGISTER_PROPERTY_COPY_REF(PropertyType, name, ...) KUBE_REGISTER_PROPERTY_COPY(PropertyType, name, __VA_ARGS__)

/** @brief Register a move only property */
#define KUBE_REGISTER_PROPERTY_MOVE(PropertyType, name, ...) \
    .data< \
        static_cast<const PropertyType &(_MetaType::*)(void) const noexcept>(&_MetaType::name), \
        static_cast<bool(_MetaType::*)(PropertyType &&)>(&_MetaType::name) \
    >(kF::Hash(#name)) \
    KUBE_REGISTER_SIGNAL(name##Changed)
#define KUBE_REGISTER_PROPERTY_MOVE_REF(PropertyType, name, ...) KUBE_REGISTER_PROPERTY_MOVE(PropertyType, name, __VA_ARGS__)

/** @brief Register a custom property using getter and setter */
#define KUBE_REGISTER_PROPERTY_CUSTOM(PropertyType, name, getter, setter) \
    .data<getter, setter>(kF::Hash(#name))

/** @brief Register a function */
#define KUBE_REGISTER_FUNCTION(name) \
    .function<&_MetaType::name>(kF::Hash(#name))

/** @brief Register an overloaded function */
#define KUBE_REGISTER_FUNCTION_OVERLOAD(name, FuncType) \
    .function<static_cast<FuncType>(&_MetaType::name)>(kF::Hash(#name))

#define KUBE_REGISTER_SIGNAL(name, ...) \
    .signal<&_MetaType::name>(kF::Hash(#name))
