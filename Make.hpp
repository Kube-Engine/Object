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
#define KUBE_MAKE_PROPERTY_IMPL(SCOPE, GETTER, SETTER, Type, name, ...) \
SCOPE: \
    KUBE_MAKE_##GETTER(Type, name) \
    KUBE_MAKE_##SETTER(Type, name) \
    KUBE_MAKE_SIGNAL_IMPL(SCOPE, name##Changed) \
private: \
    KUBE_MAKE_VARIABLE(Type, name) \

/** @brief Generate a readonly property with given parameters (used internally) */
#define KUBE_MAKE_PROPERTY_READONLY_IMPL(SCOPE, GETTER, SETTER, Type, name, ...) \
protected: \
    KUBE_MAKE_##SETTER(Type, name) \
SCOPE: \
    KUBE_MAKE_##GETTER(Type, name) \
    KUBE_MAKE_SIGNAL_IMPL(SCOPE, name##Changed) \
private: \
    KUBE_MAKE_VARIABLE(Type, name) \

/** @brief Generate a property with a move setter */
#define KUBE_MAKE_PROPERTY_COPY(Type, name, ...) \
    KUBE_MAKE_PROPERTY_IMPL(public, GETTER, SETTER_COPY, Type, name, __VA_ARGS__)

/** @brief Generate a property with a volatile reference getter and a move setter */
#define KUBE_MAKE_PROPERTY_COPY_REF(Type, name, ...) \
    KUBE_MAKE_PROPERTY_IMPL(public, GETTER_REF, SETTER_COPY, Type, name, __VA_ARGS__)

/** @brief Generate a property with a move setter */
#define KUBE_MAKE_PROPERTY_MOVE(Type, name, ...) \
    KUBE_MAKE_PROPERTY_IMPL(public, GETTER, SETTER_MOVE, Type, name, __VA_ARGS__)

/** @brief Generate a property with a volatile reference getter and a move setter */
#define KUBE_MAKE_PROPERTY_MOVE_REF(Type, name, ...) \
    KUBE_MAKE_PROPERTY_IMPL(public, GETTER_REF, SETTER_MOVE, Type, name, __VA_ARGS__)

/** @brief Generate a readonly property with a move setter */
#define KUBE_MAKE_PROPERTY_MOVE_READONLY(Type, name, ...) \
    KUBE_MAKE_PROPERTY_READONLY_IMPL(public, GETTER_REF, SETTER_MOVE, Type, name, __VA_ARGS__)

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

/** @brief Declare a signal */
#define KUBE_MAKE_SIGNAL_IMPL(SCOPE, name, ...) \
SCOPE: \
    void name(NAME_EACH(__VA_ARGS__)) { emit<&_MetaType::name>(FORWARD_NAME_EACH(__VA_ARGS__)); }

/** @brief Declare a public signal */
#define KUBE_MAKE_SIGNAL(name, ...) KUBE_MAKE_SIGNAL_IMPL(public, name, __VA_ARGS__)

/** @brief Defines a variable and initialize it with variadic arguments */
#define KUBE_MAKE_VARIABLE(Type, name, ...) Type _##name { __VA_ARGS__ };
