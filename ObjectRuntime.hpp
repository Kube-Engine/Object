/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Object
 */

#include <Kube/Meta/Meta.hpp>
#include <Kube/Core/SmallVector.hpp>
#include <Kube/Core/TrivialFunctor.hpp>

namespace kF::ObjectUtils
{
    class ObjectRuntime;
}

class alignas_half_cacheline kF::ObjectUtils::ObjectRuntime
{
public:
    /** @brief Small optimized name vector */
    using SmallNames = Core::TinySmallVector<HashedName, (Core::CacheLineSize - Core::CacheLineQuarterSize) / sizeof(HashedName)>;

    /** @brief Store a runtime data */
    struct alignas_cacheline RuntimeData
    {
        Meta::Data::Descriptor descriptor;
        Var value;
    };
    static_assert_sizeof_double_cacheline(RuntimeData);
    static_assert_alignof_cacheline(RuntimeData);

    /** @brief Store a runtime signal */
    struct alignas_cacheline RuntimeSignal
    {
        Meta::Signal::Descriptor descriptor;
    };
    static_assert_fit_cacheline(RuntimeSignal);

    /** @brief Store a runtime function */
    struct alignas_cacheline RuntimeFunction
    {
        Meta::Function::Descriptor descriptor;
    };
    static_assert_fit_cacheline(RuntimeFunction);

    /** @brief Add a runtime data to the cache */
    Meta::Data addData(const HashedName name, const Meta::Type type, Var &&value);

    /** @brief Add a runtime signal to the cache */
    Meta::Signal addSignal(const HashedName name, const std::size_t argsCount);

    /** @brief Add a runtime function to the cache */
    template<typename Functor>
    Meta::Function addFunction(const HashedName name, Functor &&functor);

    /** @brief Tries to find a runtime data */
    [[nodiscard]] Meta::Data findData(const HashedName name) const noexcept;

    /** @brief Tries to find a runtime signal */
    [[nodiscard]] Meta::Signal findSignal(const HashedName name) const noexcept;

    /** @brief Tries to find a runtime function */
    [[nodiscard]] Meta::Function findFunction(const HashedName name) const noexcept;

private:
    Core::TinyFlatVector<std::unique_ptr<RuntimeData>, SmallNames> _datas;
    Core::TinyFlatVector<std::unique_ptr<RuntimeSignal>, SmallNames> _signals;
    Core::TinyFlatVector<std::unique_ptr<RuntimeFunction>, SmallNames> _functions;
};

static_assert_fit_half_cacheline(kF::ObjectUtils::ObjectRuntime);

#include "ObjectRuntime.ipp"