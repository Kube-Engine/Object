/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Node
 */

#pragma once

#include <Kube/Core/Utils.hpp>

namespace kF::AST
{
    struct Node;
}

struct alignas_eighth_cacheline kF::AST::Node
{
    enum class Type : std::uint32_t {
        // Unary operators
        UnaryMinus,

        // Binary operators
        BinaryAddition,
        BinarySubstraction,
        BinaryMultiplication,
        BinaryDivision,
        BinaryModulo,

        // Assignment operators
        AssignmentAddition,
        AssignmentSubstraction,
        AssignmentMultiplication,
        AssignmentDivision,
        AssignmentModulo,

        // Variables
        Local,
        LValue,
        RValue,
        LValueLocal,
        RValueLocal,
        Constant,

        // Variable access
        Access,
        Derefence,
        DereferenceAccess,

        // Functions
        Call,

        // Signals
        Emit,

        // Properties
        Get,
        Set,

        // Statements
        Branch,
        Switch,
        While,
        For,
        ForEach
    };

    std::uint32_t token;
    Type type;
};

static_assert_fit_eighth_cacheline(kF::AST::Node);