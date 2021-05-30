/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Code generation helpers
 */

#pragma once

// This header must no be directly included, include 'Reflection' instead

#include <functional>

#include <Kube/Core/MacroUtils.hpp>

/** @brief Meta type getter generator */
#define KUBE_MAKE_META_TYPE_GETTER \
public: \
    kF::Meta::Type getMetaType(void) const noexcept { return kF::Meta::Factory<_MetaType>::Resolve(); }

/** @brief Virtual meta type getter generator */
#define KUBE_MAKE_VIRTUAL_META_TYPE_GETTER \
public: \
    virtual kF::Meta::Type getMetaType(void) const noexcept { return kF::Meta::Factory<_MetaType>::Resolve(); }

/** @brief Dummy generators, not used */
#define KUBE_MAKE_BASE(...)
#define KUBE_MAKE_CONSTRUCTOR(...)
#define KUBE_MAKE_FUNCTION(...)
#define KUBE_MAKE_FUNCTION_OVERLOAD(...)
#define KUBE_MAKE_PROPERTY_CUSTOM_SIGLESS(...)
#define KUBE_MAKE_PROPERTY_CUSTOM_VOLATILE_SIGLESS(...)
#define KUBE_MAKE_PROPERTY_CUSTOM_VOLATILE_GETONLY_SIGLESS(...)

/** @brief Generate a property with given parameters (used internally) */
#define KUBE_MAKE_PROPERTY_IMPL(GETTER_SCOPE, GETTER_TYPE, SETTER_SCOPE, SETTER_TYPE, SIGNAL_SCOPE, PropertyType, name, ...) \
public: \
    KUBE_MAKE_SIGNAL_IMPL(SIGNAL_SCOPE, name##Changed) \
GETTER_SCOPE: \
    KUBE_MAKE_##GETTER_TYPE(PropertyType, name) \
SETTER_SCOPE: \
    KUBE_MAKE_##SETTER_TYPE(PropertyType, name) \
private: \
    KUBE_MAKE_VARIABLE(PropertyType, name, __VA_ARGS__) \


/** @brief Generate a property with given parameters (used internally) */
#define KUBE_MAKE_PROPERTY_SIGLESS_IMPL(GETTER_SCOPE, GETTER_TYPE, SETTER_SCOPE, SETTER_TYPE, SIGNAL_SCOPE, PropertyType, name, ...) \
GETTER_SCOPE: \
    KUBE_MAKE_##GETTER_TYPE(PropertyType, name) \
SETTER_SCOPE: \
    KUBE_MAKE_##SETTER_TYPE(PropertyType, name) \
private: \
    KUBE_MAKE_VARIABLE(PropertyType, name, __VA_ARGS__) \

/** @brief Generate a constant getter, copy / move (if supported) setter(s) and a signal */
#define KUBE_MAKE_PROPERTY(PropertyType, name, ...) \
    KUBE_MAKE_PROPERTY_IMPL(public, GETTER, public, SETTER, public, PropertyType, name, __VA_ARGS__)

/** @brief Generate a constant getter, copy / move (if supported) setter(s) */
#define KUBE_MAKE_PROPERTY_SIGLESS(PropertyType, name, ...) \
    KUBE_MAKE_PROPERTY_SIGLESS_IMPL(public, GETTER, public, SETTER_SIGLESS, public, PropertyType, name, __VA_ARGS__)

/** @brief Generate a volatile getter, copy / move (if supported) setter(s) and a signal */
#define KUBE_MAKE_PROPERTY_VOLATILE(PropertyType, name, ...) \
    KUBE_MAKE_PROPERTY_IMPL(public, GETTER_REF, public, SETTER, public, PropertyType, name, __VA_ARGS__)

/** @brief Generate a volatile getter, copy / move (if supported) setter(s) */
#define KUBE_MAKE_PROPERTY_VOLATILE_SIGLESS(PropertyType, name, ...) \
    KUBE_MAKE_PROPERTY_SIGLESS_IMPL(public, GETTER_REF, public, SETTER_SIGLESS, public, PropertyType, name, __VA_ARGS__)

/** @brief Generate a constant getter, private copy / move (if supported) setter(s) and a signal */
#define KUBE_MAKE_PROPERTY_GETONLY(PropertyType, name, ...) \
    KUBE_MAKE_PROPERTY_IMPL(public, GETTER, private, SETTER, public, PropertyType, name, __VA_ARGS__)

/** @brief Generate a constant getter, private copy / move (if supported) setter(s) */
#define KUBE_MAKE_PROPERTY_GETONLY_SIGLESS(PropertyType, name, ...) \
    KUBE_MAKE_PROPERTY_SIGLESS_IMPL(public, GETTER, private, SETTER_SIGLESS, public, PropertyType, name, __VA_ARGS__)

/** @brief Generate a volatile getter, private copy / move (if supported) setter(s) and a signal */
#define KUBE_MAKE_PROPERTY_VOLATILE_GETONLY(PropertyType, name, ...) \
    KUBE_MAKE_PROPERTY_IMPL(public, GETTER_REF, private, SETTER, public, PropertyType, name, __VA_ARGS__)

/** @brief Generate a volatile getter, private copy / move (if supported) setter(s) */
#define KUBE_MAKE_PROPERTY_VOLATILE_GETONLY_SIGLESS(PropertyType, name, ...) \
    KUBE_MAKE_PROPERTY_SIGLESS_IMPL(public, GETTER_REF, private, SETTER_SIGLESS, public, PropertyType, name, __VA_ARGS__)

/** @brief Generate the signal of a custom property */
#define KUBE_MAKE_PROPERTY_CUSTOM(PropertyType, name, ...) \
    KUBE_MAKE_SIGNAL(name##Changed)

/** @brief Generate the signal of a custom property */
#define KUBE_MAKE_PROPERTY_CUSTOM_VOLATILE(PropertyType, name, ...) \
    KUBE_MAKE_PROPERTY_CUSTOM(PropertyType, name, __VA_ARGS__)

/** @brief Generate the signal of a custom property */
#define KUBE_MAKE_PROPERTY_CUSTOM_VOLATILE_GETONLY(PropertyType, name, ...) \
    KUBE_MAKE_PROPERTY_CUSTOM(PropertyType, name, __VA_ARGS__)

/** @brief Const reference getter for any type */
#define KUBE_MAKE_GETTER(PropertyType, name) \
    [[nodiscard]] inline kF::Internal::ToConstReference<PropertyType> name(void) const noexcept { return _##name; }

/** @brief Either const and volatiele getter for any type */
#define KUBE_MAKE_GETTER_REF(PropertyType, name) \
    KUBE_MAKE_GETTER(PropertyType, name) \
    [[nodiscard]] inline PropertyType &name(void) noexcept { return _##name; }

/** @brief Copy setter for any type (with signal emition) */
#define KUBE_MAKE_SETTER(PropertyType, name) \
    template<typename _PropertyType> requires (std::is_same_v<std::remove_cvref_t<_PropertyType>, PropertyType>) \
    inline bool name(_PropertyType &&value) noexcept_forward_assignable(decltype(value)) \
    { \
        if (_##name == value) \
            return false; \
        _##name = std::forward<_PropertyType>(value); \
        name##Changed(); \
        return true; \
    }

/** @brief Copy / Move setter for any type (without signal emition) */
#define KUBE_MAKE_SETTER_SIGLESS(PropertyType, name) \
    template<typename _PropertyType> requires (std::is_same_v<std::remove_cvref_t<_PropertyType>, PropertyType>) \
    inline void name(_PropertyType &&value) noexcept_forward_assignable(decltype(value))  \
    { \
        _##name = std::forward<_PropertyType>(value); \
    }

/** @brief Declare a signal (used internal) */
#define KUBE_MAKE_SIGNAL_IMPL(SCOPE, name, ...) \
SCOPE: \
    inline void name(NAME_EACH(__VA_ARGS__)) \
    { \
        static kF::Meta::Signal Cache; \
        if (!Cache) [[unlikely]] \
            Cache = getMetaType().findSignal<&_MetaType::name>(); \
        emitSignal(Cache __VA_OPT__(, FORWARD_NAME_EACH(__VA_ARGS__))); \
    } \

/** @brief Declare a public signal */
#define KUBE_MAKE_SIGNAL(name, ...) KUBE_MAKE_SIGNAL_IMPL(public, name, __VA_ARGS__)

/** @brief Defines a variable and initialize it with variadic arguments */
#define KUBE_MAKE_VARIABLE(PropertyType, name, ...) PropertyType _##name { __VA_ARGS__ };
