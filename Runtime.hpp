/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Object runtime
 */

#pragma once

#include <Kube/Core/Functor.hpp>
#include <Kube/Core/Vector.hpp>
#include <Kube/Core/Hash.hpp>

namespace kF::ObjectUtils
{
    class Runtime;
}

/** @brief Object runtime data */
class kF::ObjectUtils::Runtime
{
public:


private:
    Core::TinyVector<HashedName> _functionNames {};
    // Core::TinyVector<Expression> _functions {};
};

#include "Runtime.ipp"