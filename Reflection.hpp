/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Reflection interface for KUBE
 */

#pragma once

#include <Kube/Meta/Registerer.hpp>

#include "Make.hpp"
#include "Register.hpp"

/** @brief Register a concrete type (no virtual functions) to the reflection system */
#define K_CONCRETE(ClassType, ...) \
    KUBE_REGISTER_LATER(ClassType __VA_OPT__(,) __VA_ARGS__) \
    KUBE_MAKE_META_TYPE_GETTER \
private:

/** @brief Register a virtual base type (no parent class) to the reflection system */
#define K_ABSTRACT(ClassType, ...) \
    KUBE_REGISTER_LATER(ClassType __VA_OPT__(,) __VA_ARGS__) \
    KUBE_MAKE_VIRTUAL_META_TYPE_GETTER \
private:

/** @brief Register a virtual derived type to the reflection system */
#define K_DERIVED(ClassType, BaseType, ...) \
    KUBE_REGISTER_LATER(ClassType, K_BASE(BaseType) __VA_OPT__(,) __VA_ARGS__) \
    KUBE_MAKE_VIRTUAL_META_TYPE_GETTER \
private:

/** @brief Use this macro to register a base to a meta type
 *  @param BaseType is the type of the base
 */
#define K_BASE(BaseType) BASE(BaseType)

/** @brief Use this macro to register a custom constructor to a meta type
 *  @param ... all variadic are constructor's parameters types
 */
#define K_CONSTRUCTOR(...) CONSTRUCTOR(__VA_ARGS__)

/** @brief Use these macros to generate a new property (getter / setter / signal) and register it
 *  The property can have several tags used to generate an optimal getter and setter.
 *      - VOLATILE: the property has a protected setter not registered to the meta type
 *      - SIGLESS: Will prevent generation of a signal
 *      - GETONLY: Will generate a private setter and not register it
 *
 *  @param PropertyType is the property type
 *  @param name is the name of the property
 *  @param ... all variadic arguments are used to initialize the property
 */
#define K_PROPERTY(PropertyType, name, ...)                             PROPERTY(PropertyType, name __VA_OPT__(,) __VA_ARGS__)
#define K_PROPERTY_SIGLESS(PropertyType, name, ...)                     PROPERTY_SIGLESS(PropertyType, name __VA_OPT__(,) __VA_ARGS__)
#define K_PROPERTY_VOLATILE(PropertyType, name, ...)                    PROPERTY_VOLATILE(PropertyType, name __VA_OPT__(,) __VA_ARGS__)
#define K_PROPERTY_VOLATILE_SIGLESS(PropertyType, name, ...)            PROPERTY_VOLATILE_SIGLESS(PropertyType, name __VA_OPT__(,) __VA_ARGS__)
#define K_PROPERTY_GETONLY(PropertyType, name, ...)                     PROPERTY_GETONLY(PropertyType, name __VA_OPT__(,) __VA_ARGS__)
#define K_PROPERTY_GETONLY_SIGLESS(PropertyType, name, ...)             PROPERTY_GETONLY_SIGLESS(PropertyType, name __VA_OPT__(,) __VA_ARGS__)
#define K_PROPERTY_VOLATILE_GETONLY(PropertyType, name, ...)            PROPERTY_VOLATILE_GETONLY(PropertyType, name __VA_OPT__(,) __VA_ARGS__)
#define K_PROPERTY_VOLATILE_GETONLY_SIGLESS(PropertyType, name, ...)    PROPERTY_VOLATILE_GETONLY_SIGLESS(PropertyType, name __VA_OPT__(,) __VA_ARGS__)

/** @brief Use these macros to register a custom property and generate a signal
 *  The property can have several tags used to generate an optimal getter and setter.
 *      - COPY: Will discard copy setter
 *      - MOVE: Will discard move setter
 *      - SIGLESS: Will prevent generation of a signal
 *      - GETONLY: Will not register setters
 *
 *  @param PropertyType is the property type
 *  @param name is the name of the property
 *  @param getter is the pointer to the getter function
 *  @param copySetter is the pointer to the copy setter function (nullptr for none)
 *  @param moveSetter is the pointer to the move setter function (nullptr for none)
 */
#define K_PROPERTY_CUSTOM(PropertyType, name, getter, copySetter, moveSetter)           PROPERTY_CUSTOM(PropertyType, name, getter, copySetter, moveSetter)
#define K_PROPERTY_CUSTOM_SIGLESS(PropertyType, name, getter, copySetter, moveSetter)   PROPERTY_CUSTOM_SIGLESS(PropertyType, name, getter, copySetter, moveSetter)
#define K_PROPERTY_CUSTOM_COPY(PropertyType, name, getter, copySetter)                  PROPERTY_CUSTOM(PropertyType, name, getter, copySetter, nullptr)
#define K_PROPERTY_CUSTOM_COPY_SIGLESS(PropertyType, name, getter, copySetter)          PROPERTY_CUSTOM_SIGLESS(PropertyType, name, getter, copySetter, nullptr)
#define K_PROPERTY_CUSTOM_MOVE(PropertyType, name, getter, moveSetter)                  PROPERTY_CUSTOM(PropertyType, name, getter, nullptr, moveSetter)
#define K_PROPERTY_CUSTOM_MOVE_SIGLESS(PropertyType, name, getter, moveSetter)          PROPERTY_CUSTOM_SIGLESS(PropertyType, name, getter, nullptr, moveSetter)
#define K_PROPERTY_CUSTOM_GETONLY(PropertyType, name, getter)                           PROPERTY_CUSTOM(PropertyType, name, getter, nullptr, nullptr)
#define K_PROPERTY_CUSTOM_GETONLY_SIGLESS(PropertyType, name, getter)                   PROPERTY_CUSTOM_SIGLESS(PropertyType, name, getter, nullptr, nullptr)

/** @brief Use this macro to generate a new signal and register it to a meta type
 *  @param name is the name of the signal function
 *  @param ... all variadic arguments are signal function's parameters types
 */
#define K_SIGNAL(name, ...) SIGNAL(name __VA_OPT__(, __VA_ARGS__))

/** @brief Use this macro to register a new function to a meta type
 *  @param name is the name of the function
 */
#define K_FUNCTION(name) FUNCTION(name)

/** @brief Use this macro to register a new function to a meta type
 *  @param name is the name of the function
 *  @param FunctionType is the type of the overloaded function
 */
#define K_FUNCTION_OVERLOAD(name, FunctionType) FUNCTION_OVERLOAD(name, FunctionType)

/** @brief The emit macro, totally useless but adds clarity to differentiate functions calls and signal emition */
#define emit

namespace kF
{
    /** @brief Registers all base types */
    void RegisterMetadata(void);
}