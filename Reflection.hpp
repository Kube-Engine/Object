/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Reflection interface for KUBE
 */

#pragma once

#include <Kube/Meta/Meta.hpp>

#include "Make.hpp"
#include "Register.hpp"

/** @brief Register an object derivd type to the reflection system */
#define K_OBJECT(ClassType, ...) \
private: \
    using _MetaType = ClassType; \
protected: \
    [[nodiscard]] virtual kF::Var getTypeHandle(void) noexcept { return kF::Var::Assign(*this); } \
    [[nodiscard]] virtual kF::Var getTypeHandle(void) const noexcept { return kF::Var::Assign(*this); } \
public: \
    [[nodiscard]] virtual kF::Meta::Type getMetaType(void) const noexcept { return kF::Meta::Factory<_MetaType>::Resolve(); } \
    static void RegisterMetadata(void) noexcept { KUBE_REGISTER_TYPE(ClassType) ADD_PREFIX_EACH(KUBE_REGISTER_, __VA_ARGS__) ; } \
    ADD_PREFIX_EACH(KUBE_MAKE_, __VA_ARGS__) \
private:

/** @brief Use this macro to register a base to a meta type
 *  @param BaseType is the type of the base
 */
#define K_BASE(BaseType) BASE(BaseType)

/** @brief Use this macro to register a custom constructor to a meta type
 *  @param ... all variadic are constructor's parameters types
 */
#define K_CONSTRUCTOR(...)

/** @brief Use these macros to generate a new property and register it to a meta type
 *  The property can have several tags used to generate an optimal getter and setter.
 *      - COPY: the property setter will take a constant reference
 *      - MOVE: the property setter will take a rvalue reference
 *      - REF: the property also has a non-const getter that will not trigger the property signal on change
 *
 *  @param PropertyType is the property type
 *  @param name is the name of the property
 *  @param ... all variadic arguments are used to initialize the property
 */
#define K_PROPERTY_COPY(PropertyType, name, ...)     PROPERTY_COPY(PropertyType, name, __VA_ARGS__)
#define K_PROPERTY_COPY_REF(PropertyType, name, ...) PROPERTY_COPY_REF(PropertyType, name, __VA_ARGS__)
#define K_PROPERTY_MOVE(PropertyType, name, ...)     PROPERTY_COPY_MOVE(PropertyType, name, __VA_ARGS__)
#define K_PROPERTY_MOVE_REF(PropertyType, name, ...) PROPERTY_COPY_MOVE_REF(PropertyType, name, __VA_ARGS__)

/** @brief Use this macro to register a meta property using custom getter and setter
 *  This macro will NOT generate NOR register a SIGNAL, the user is in charge of doing it manually
 *
 *  @param PropertyType is the property type
 *  @param name is the property name
 *  @param getter is the property getter pointer
 *  @param setter is the property setter pointer
 */
#define K_PROPERTY_CUSTOM(PropertyType, name, getter, setter) PROPERTY_CUSTOM(PropertyType, name, getter, setter)

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

namespace kF
{
    using TypeHandle = Var;

    /** @brief The named used to register Item instantiation helper */
    constexpr HashedName MakeInstanceFunctionName = kF::Hash("__makePtr__");

    /** @brief Registers all base types */
    void RegisterMetadata(void);
}