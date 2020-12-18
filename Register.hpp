/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Registeres generation helpers
 */

#pragma once

#include <iostream>

#include <Kube/Core/MacroUtils.hpp>

/** @brief Add static registerer for automatic registeration */
#define KUBE_REGISTER_LATER(ClassType, ...) \
private: \
    using _MetaType = ClassType; \
    ADD_PREFIX_EACH(KUBE_MAKE_, __VA_ARGS__) \
    static inline kF::Meta::RegisterLater _MetaRegisterer = kF::Meta::RegisterLater::Make<ClassType>( \
        [] { \
            std::cout << "Registering type " << #ClassType << std::endl; \
            KUBE_REGISTER_TYPE(ClassType) \
                ADD_PREFIX_EACH(KUBE_REGISTER_, __VA_ARGS__) ; \
        } \
    );

/** @brief Register a new type (already used by 'KUBE_REGISTER' and 'KUBE_REGISTER_INSTANTIABLE') */
#define KUBE_REGISTER_TYPE(ClassType) \
    kF::Meta::Factory<_MetaType>( \
        kF::Hash(#ClassType), \
        kF::Internal::GetTemplateSpecializedName<_MetaType>(kF::Hash(#ClassType)) \
    )

/** @brief Register a base type */
#define KUBE_REGISTER_BASE(BaseType) \
    .template base<BaseType>()

/** @brief Register a custom constructor (not default / copy / move) */
#define KUBE_REGISTER_CONSTRUCTOR(...) \
    .template constructor<__VA_ARGS__>();

/** @brief Register a constant readable, writable and signalable property */
#define KUBE_REGISTER_PROPERTY(PropertyType, name, ...) \
    KUBE_REGISTER_PROPERTY_CUSTOM(PropertyType, name, \
        static_cast<const PropertyType &(_MetaType::*)(void) const noexcept>(&_MetaType::name), \
        ConstexprTernary(std::is_copy_assignable_v<PropertyType>, &_MetaType::name<const PropertyType &>, nullptr), \
        ConstexprTernary(std::is_move_assignable_v<PropertyType>, &_MetaType::name<PropertyType &&>, nullptr) \
    )

/** @brief Register a constant readable and writable property (no signal) */
#define KUBE_REGISTER_PROPERTY_SIGLESS(PropertyType, name, ...) \
    KUBE_REGISTER_PROPERTY_CUSTOM_SIGLESS(PropertyType, name, \
        static_cast<const PropertyType &(_MetaType::*)(void) const noexcept>(&_MetaType::name), \
        ConstexprTernary(std::is_copy_assignable_v<PropertyType>, &_MetaType::name<const PropertyType &>, nullptr), \
        ConstexprTernary(std::is_move_assignable_v<PropertyType>, &_MetaType::name<PropertyType &&>, nullptr) \
    )

/** @brief Register a volatile readable, writable and signalable property */
#define KUBE_REGISTER_PROPERTY_VOLATILE(PropertyType, name, ...) \
    KUBE_REGISTER_PROPERTY_CUSTOM(PropertyType, name, \
        static_cast<PropertyType &(_MetaType::*)(void) noexcept>(&_MetaType::name), \
        ConstexprTernary(std::is_copy_assignable_v<PropertyType>, &_MetaType::name<const PropertyType &>, nullptr), \
        ConstexprTernary(std::is_move_assignable_v<PropertyType>, &_MetaType::name<PropertyType &&>, nullptr) \
    )

/** @brief Register a volatile readable and writable property (no signal) */
#define KUBE_REGISTER_PROPERTY_VOLATILE_SIGLESS(PropertyType, name, ...) \
    KUBE_REGISTER_PROPERTY_CUSTOM_SIGLESS(PropertyType, name, \
        static_cast<PropertyType &(_MetaType::*)(void) noexcept>(&_MetaType::name), \
        ConstexprTernary(std::is_copy_assignable_v<PropertyType>, &_MetaType::name<const PropertyType &>, nullptr), \
        ConstexprTernary(std::is_move_assignable_v<PropertyType>, &_MetaType::name<PropertyType &&>, nullptr) \
    )

/** @brief Register a const readable and signalable property */
#define KUBE_REGISTER_PROPERTY_GETONLY(PropertyType, name, ...) \
    KUBE_REGISTER_PROPERTY_CUSTOM(PropertyType, name, \
        static_cast<const PropertyType &(_MetaType::*)(void) const noexcept>(&_MetaType::name), \
        nullptr, \
        nullptr \
    )

/** @brief Register a const readable property (no signal) */
#define KUBE_REGISTER_PROPERTY_GETONLY_SIGLESS(PropertyType, name, ...) \
    KUBE_REGISTER_PROPERTY_CUSTOM_SIGLESS(PropertyType, name, \
        static_cast<const PropertyType &(_MetaType::*)(void) const noexcept>(&_MetaType::name), \
        nullptr, \
        nullptr \
    )

/** @brief Register a const readable and signalable property */
#define KUBE_REGISTER_PROPERTY_VOLATILE_GETONLY(PropertyType, name, ...) \
    KUBE_REGISTER_PROPERTY_CUSTOM(PropertyType, name, \
        static_cast<PropertyType &(_MetaType::*)(void) noexcept>(&_MetaType::name), \
        nullptr, \
        nullptr \
    )

/** @brief Register a const readable property (no signal) */
#define KUBE_REGISTER_PROPERTY_VOLATILE_GETONLY_SIGLESS(PropertyType, name, ...) \
    KUBE_REGISTER_PROPERTY_CUSTOM_SIGLESS(PropertyType, name, \
        static_cast<PropertyType &(_MetaType::*)(void) noexcept>(&_MetaType::name), \
        nullptr, \
        nullptr \
    )

/** @brief Register a custom property using getter, copy setter, move setter and a signal */
#define KUBE_REGISTER_PROPERTY_CUSTOM(PropertyType, name, getter, copySetter, moveSetter) \
    .template data<getter, copySetter, moveSetter, &_MetaType::name##Changed>(kF::Hash(#name), kF::Hash(_STRINGIFY(name##Changed)))

/** @brief Register a custom property using getter, copy setter and move setter (no signal) */
#define KUBE_REGISTER_PROPERTY_CUSTOM_SIGLESS(PropertyType, name, getter, copySetter, moveSetter) \
    .template data<getter, copySetter, moveSetter, nullptr>(kF::Hash(#name))

/** @brief Register a custom property using getter, copy setter and a signal */
#define KUBE_REGISTER_PROPERTY_CUSTOM_COPY(PropertyType, name, getter, copySetter) \
    .template data<getter, copySetter, nullptr, &_MetaType::name##Changed>(kF::Hash(#name), kF::Hash(_STRINGIFY(name##Changed)))

/** @brief Register a custom property using getter and copy setter (no signal) */
#define KUBE_REGISTER_PROPERTY_CUSTOM_COPY_SIGLESS(PropertyType, name, getter, copySetter) \
    .template data<getter, copySetter, nullptr>(kF::Hash(#name))

/** @brief Register a custom property using getter, move setter and a signal */
#define KUBE_REGISTER_PROPERTY_CUSTOM_MOVE(PropertyType, name, getter, moveSetter) \
    .template data<getter, nullptr, moveSetter, &_MetaType::name##Changed>(kF::Hash(#name), kF::Hash(_STRINGIFY(name##Changed)))

/** @brief Register a custom property using getter and move setter (no signal) */
#define KUBE_REGISTER_PROPERTY_CUSTOM_MOVE_SIGLESS(PropertyType, name, getter, moveSetter) \
    .template data<getter, nullptr, moveSetter, nullptr>(kF::Hash(#name))

/** @brief Register a custom property using getter and signal */
#define KUBE_REGISTER_PROPERTY_CUSTOM_READONLY(PropertyType, name, getter) \
    .template data<getter, nullptr, nullptr, &_MetaType::name##Changed>(kF::Hash(#name), kF::Hash(_STRINGIFY(name##Changed)))

/** @brief Register a custom property using getter (no signal) */
#define KUBE_REGISTER_PROPERTY_CUSTOM_READONLY_SIGLESS(PropertyType, name, getter) \
    .template data<getter, nullptr, nullptr, nullptr>(kF::Hash(#name))

/** @brief Register a function */
#define KUBE_REGISTER_FUNCTION(name) \
    .template function<&_MetaType::name>(kF::Hash(#name))

/** @brief Register an overloaded function */
#define KUBE_REGISTER_FUNCTION_OVERLOAD(name, FuncType) \
    .template function<static_cast<FuncType>(&_MetaType::name)>(kF::Hash(#name))

/** @brief Register a non-property bound signal */
#define KUBE_REGISTER_SIGNAL(name, ...) \
    .template signal<&_MetaType::name>(kF::Hash(#name))

namespace kF::Internal
{
    /** @brief Helper used internally to query the name of a specialized template type */
    template<typename TemplateType>
    [[nodiscard]] HashedName GetTemplateSpecializedName(HashedName hash)
    {
        using Decomposer = Meta::Internal::TemplateDecomposer<TemplateType>;

        if constexpr (Decomposer::IsTemplate) {
            if constexpr (Decomposer::HasTypes && !Decomposer::HasVariables)
                return GetTemplateTypeNames<typename Decomposer::TypesTuple>(hash);
            else if constexpr (!Decomposer::HasTypes && Decomposer::HasVariables)
                return GetTemplateVariableNames<Decomposer::VarsTuple>(hash);
            else if constexpr (Decomposer::TypesBeforeArguments)
                return GetTemplateVariableNames<Decomposer::VarsTuple>(
                    GetTemplateTypeNames<typename Decomposer::TypesTuple>(hash)
                );
            else
                return GetTemplateTypeNames<typename Decomposer::TypesTuple>(
                    GetTemplateVariableNames<Decomposer::VarsTuple>(hash)
                );
        }
        else
            return hash;
    }

    /** @brief Helper used internally to query the sum of parameter template type names of a specialized template type */
    template<typename TypesTuple, std::size_t Index = 0ul>
    [[nodiscard]] HashedName GetTemplateTypeNames(HashedName hash)
    {
        const auto name = Meta::Factory<std::tuple_element_t<Index, TypesTuple>>::Resolve().name();
        kFAssert(name,
            throw std::logic_error("GetTemplateTypeNames: Couldn't register a template specialization of unregistered parameter type"));
        hash += name;
        if constexpr (Index + 1 == std::tuple_size_v<TypesTuple>)
            return hash;
        else
            return GetTemplateTypeNames<TypesTuple, Index + 1>(hash);
    }

    /** @brief Helper used internall to query the sum of parameter template variable value names of a specialized template type */
    template<auto VarsTuple, std::size_t Index = 0ul>
    [[nodiscard]] HashedName GetTemplateVariableNames(HashedName hash)
    {
        using Tuple = decltype(VarsTuple);

        hash += Hash(std::to_string(std::get<Index>(VarsTuple)));
        if constexpr (Index + 1 == std::tuple_size_v<Tuple>)
            return hash;
        else
            return GetTemplateVariableNames<VarsTuple, Index + 1>(hash);
    }
}