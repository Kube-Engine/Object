/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Code generation helpers
 */

#pragma once

#include <functional>

#include "MacroUtils.hpp"

/** @brief Dummy generators, not used */
#define KUBE_MAKE_BASE(...)
#define KUBE_MAKE_CONSTRUCTOR(...)
#define KUBE_MAKE_FUNCTION(...)
#define KUBE_MAKE_FUNCTION_OVERLOAD(...)
#define KUBE_MAKE_PROPERTY_CUSTOM(...)

/** @brief Generate a property with given parameters (used internally) */
#define KUBE_MAKE_PROPERTY_IMPL(GETTER_SCOPE, GETTER_TYPE, SETTER_SCOPE, SETTER_TYPE, SIGNAL_SCOPE, Type, name, ...) \
    KUBE_MAKE_SIGNAL_IMPL(SIGNAL_SCOPE, name##Changed) \
GETTER_SCOPE: \
    KUBE_MAKE_##GETTER_TYPE(Type, name) \
SETTER_SCOPE: \
    KUBE_MAKE_##SETTER_TYPE(Type, name) \
private: \
    KUBE_MAKE_VARIABLE(Type, name, __VA_ARGS__) \

/** @brief Generate a property with a move setter */
#define KUBE_MAKE_PROPERTY_COPY(Type, name, ...) \
    KUBE_MAKE_PROPERTY_IMPL(public, GETTER, public, SETTER_COPY, public, Type, name, __VA_ARGS__)

/** @brief Generate a property with a volatile reference getter and a move setter */
#define KUBE_MAKE_PROPERTY_COPY_REF(Type, name, ...) \
    KUBE_MAKE_PROPERTY_IMPL(public, GETTER_REF, public, SETTER_COPY, public, Type, name, __VA_ARGS__)

/** @brief Generate a readonly property with a copy setter */
#define KUBE_MAKE_PROPERTY_COPY_READONLY(Type, name, ...) \
    KUBE_MAKE_PROPERTY_IMPL(public, GETTER, private, SETTER_COPY, public, Type, name, __VA_ARGS__)

/** @brief Generate a property with a move setter */
#define KUBE_MAKE_PROPERTY_MOVE(Type, name, ...) \
    KUBE_MAKE_PROPERTY_IMPL(public, GETTER, public, SETTER_MOVE, public, Type, name, __VA_ARGS__)

/** @brief Generate a property with a volatile reference getter and a move setter */
#define KUBE_MAKE_PROPERTY_MOVE_REF(Type, name, ...) \
    KUBE_MAKE_PROPERTY_IMPL(public, GETTER_REF, public, SETTER_MOVE, public, Type, name, __VA_ARGS__)

/** @brief Generate a readonly property with a move setter */
#define KUBE_MAKE_PROPERTY_MOVE_READONLY(Type, name, ...) \
    KUBE_MAKE_PROPERTY_IMPL(public, GETTER, private, SETTER_MOVE, public, Type, name, __VA_ARGS__)

/** @brief Const reference getter for any type */
#define KUBE_MAKE_GETTER(Type, name) \
    [[nodiscard]] const Type &name(void) const noexcept { return _##name; }

/** @brief Either const and volatiele getter for any type */
#define KUBE_MAKE_GETTER_REF(Type, name) \
    KUBE_MAKE_GETTER(Type, name) \
    [[nodiscard]] Type &name(void) noexcept { return _##name; }

/** @brief Copy setter for any type */
#define KUBE_MAKE_SETTER_COPY(Type, name) \
    bool name(const Type &value) { \
        if (_##name == value) \
            return false; \
        _##name = value; \
        emit<&_MetaType::name##Changed>(); \
        return true; \
    }

/** @brief Move setter for any type */
#define KUBE_MAKE_SETTER_MOVE(Type, name) \
    bool name(Type &&value) { \
        if (_##name == value) \
            return false; \
        _##name = std::move(value); \
        emit<&_MetaType::name##Changed>(); \
        return true; \
    }

/** @brief Declare a signal (used internal) */
#define KUBE_MAKE_SIGNAL_IMPL(SCOPE, name, ...) \
SCOPE: \
    void name(NAME_EACH(__VA_ARGS__)) { emit<&_MetaType::name>(FORWARD_NAME_EACH(__VA_ARGS__)); }

/** @brief Declare a public signal */
#define KUBE_MAKE_SIGNAL(name, ...) KUBE_MAKE_SIGNAL_IMPL(public, name, __VA_ARGS__)

/** @brief Defines a variable and initialize it with variadic arguments */
#define KUBE_MAKE_VARIABLE(Type, name, ...) Type _##name { __VA_ARGS__ };
