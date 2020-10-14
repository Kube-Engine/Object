/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: A set of macro helpers
 */

#pragma once

// Concatenate arguments
#define _CONCATENATE(arg1, arg2) arg1##arg2
#define _CONCATENATE5(_0, _1, _2, _3, _4) _0 ## _1 ## _2 ## _3 ## _4

// Check if variadic arguments has a coma (one zero arguments)
#define _HAS_COMMA(...) _ARG16(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0)
#define _ARG16(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, ...) _15
#define _TRIGGER_PARENTHESIS_(...) ,

// Check if variadic list of arguments is empty
#define IS_EMPTY(...) _IS_EMPTY( \
        /* test if there is just one argument, eventually an empty one */ \
        _HAS_COMMA(__VA_ARGS__), \
        /* test if _TRIGGER_PARENTHESIS_ together with the argument adds a comma */ \
        _HAS_COMMA(_TRIGGER_PARENTHESIS_ __VA_ARGS__), \
        /* test if the argument together with a parenthesis adds a comma */ \
        _HAS_COMMA(__VA_ARGS__ (/*empty*/)), \
        /* test if placing it between _TRIGGER_PARENTHESIS_ and the parenthesis adds a comma */ \
        _HAS_COMMA(_TRIGGER_PARENTHESIS_ __VA_ARGS__ (/*empty*/)) \
    )
#define _IS_EMPTY(_0, _1, _2, _3) _HAS_COMMA(_CONCATENATE5(_IS_EMPTY_CASE_, _0, _1, _2, _3))
#define _IS_EMPTY_CASE_0001 ,

// Helper to get the number of argument of variadic arguments
#define VA_ARGC(...) _VA_ARGC_BRANCH(IS_EMPTY(__VA_ARGS__), __VA_ARGS__)
#define _VA_ARGC_BRANCH(N, ...) _CONCATENATE(_VA_ARGC_BRANCH_, N)(__VA_ARGS__)
#define _VA_ARGC_BRANCH_0(...) _VA_ARGC(__VA_ARGS__, _VA_ARGC_RSEQ_N())
#define _VA_ARGC_BRANCH_1(...) 0
#define _VA_ARGC(...) _VA_ARGC_N(__VA_ARGS__)
#define _VA_ARGC_N(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N
#define _VA_ARGC_RSEQ_N() 8, 7, 6, 5, 4, 3, 2, 1, 0

// Iterate of up to 8 arguments, applying a functor
#define FOR_EACH(what, ...)         _FOR_EACH(VA_ARGC(__VA_ARGS__), what, __VA_ARGS__)
#define _FOR_EACH(N, what, ...)     _CONCATENATE(_FOR_EACH, N)(what, __VA_ARGS__)
#define _FOR_EACH_0(what)
#define _FOR_EACH_1(what, x)        what(x)
#define _FOR_EACH_2(what, x, ...)   what(x); _FOR_EACH_1(what, __VA_ARGS__);
#define _FOR_EACH_3(what, x, ...)   what(x); _FOR_EACH_2(what, __VA_ARGS__);
#define _FOR_EACH_4(what, x, ...)   what(x); _FOR_EACH_3(what, __VA_ARGS__);
#define _FOR_EACH_5(what, x, ...)   what(x); _FOR_EACH_4(what, __VA_ARGS__);
#define _FOR_EACH_6(what, x, ...)   what(x); _FOR_EACH_5(what, __VA_ARGS__);
#define _FOR_EACH_7(what, x, ...)   what(x); _FOR_EACH_6(what, __VA_ARGS__);
#define _FOR_EACH_8(what, x, ...)   what(x); _FOR_EACH_7(what, __VA_ARGS__);

// Automatically give names to a list of arguments
#define NAME_EACH(...)              _NAME_EACH(VA_ARGC(__VA_ARGS__), __VA_ARGS__)
#define _NAME_EACH(N, ...)          _CONCATENATE(_NAME_EACH_, N)(__VA_ARGS__)
#define _NAME_EACH_0()
#define _NAME_EACH_1(x)             x arg1
#define _NAME_EACH_2(x, ...)        x arg2, _NAME_EACH_1(__VA_ARGS__)
#define _NAME_EACH_3(x, ...)        x arg3, _NAME_EACH_2(__VA_ARGS__)
#define _NAME_EACH_4(x, ...)        x arg4, _NAME_EACH_3(__VA_ARGS__)
#define _NAME_EACH_5(x, ...)        x arg5, _NAME_EACH_4(__VA_ARGS__)
#define _NAME_EACH_6(x, ...)        x arg6, _NAME_EACH_5(__VA_ARGS__)
#define _NAME_EACH_7(x, ...)        x arg7, _NAME_EACH_6(__VA_ARGS__)
#define _NAME_EACH_8(x, ...)        x arg8, _NAME_EACH_7(__VA_ARGS__)

// Automatically forward newly generated names of an argument list
#define FORWARD_NAME_EACH(...)          _FORWARD_NAME_EACH(VA_ARGC(__VA_ARGS__), __VA_ARGS__)
#define _FORWARD_NAME_EACH(N, ...)      _CONCATENATE(_FORWARD_NAME_EACH_, N)(__VA_ARGS__)
#define _FORWARD_NAME_EACH_0()
#define _FORWARD_NAME_EACH_1(x)         std::forward<x>(arg1)
#define _FORWARD_NAME_EACH_2(x, ...)    std::forward<x>(arg2), _FORWARD_NAME_EACH_1(__VA_ARGS__)
#define _FORWARD_NAME_EACH_3(x, ...)    std::forward<x>(arg3), _FORWARD_NAME_EACH_2(_VA_ARGS__)
#define _FORWARD_NAME_EACH_4(x, ...)    std::forward<x>(arg4), _FORWARD_NAME_EACH_3(__VA_ARGS__)
#define _FORWARD_NAME_EACH_5(x, ...)    std::forward<x>(arg5), _FORWARD_NAME_EACH_4(__VA_ARGS__)
#define _FORWARD_NAME_EACH_6(x, ...)    std::forward<x>(arg6), _FORWARD_NAME_EACH_5(__VA_ARGS__)
#define _FORWARD_NAME_EACH_7(x, ...)    std::forward<x>(arg7), _FORWARD_NAME_EACH_6(__VA_ARGS__)
#define _FORWARD_NAME_EACH_8(x, ...)    std::forward<x>(arg8), _FORWARD_NAME_EACH_7(__VA_ARGS__)
