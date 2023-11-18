#pragma once
#ifndef CURRY_FUNCTION_HPP
#define CURRY_FUNCTION_HPP

#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace currying::v1 {

template <bool Owns>
struct ArgOwnership { };

using OwnArguments = ArgOwnership<true>;
using ReferenceArguments = ArgOwnership<false>;

inline constexpr auto ownArguments = OwnArguments {};
inline constexpr auto referenceArguments = ReferenceArguments {};

template <bool Owns, typename FuncType, typename IndexSeq, typename... Args>
class Currying; /* undefined */

// Concept to ensure the arity of function matches with provided arguments
template <auto NumTupArgs, auto NumIndicies, auto NumArgs>
concept ValidCurrying = requires {
    {
        NumTupArgs
    } -> std::convertible_to<std::size_t>;
    {
        NumIndicies
    } -> std::convertible_to<std::size_t>;
    {
        NumArgs
    } -> std::convertible_to<std::size_t>;

    NumIndicies == NumTupArgs;
    NumTupArgs <= NumArgs;
};

template <bool take_ownership, typename... TupArgs, std::size_t... I, typename R, typename... Args>
    requires ValidCurrying<sizeof...(TupArgs), sizeof...(I), sizeof...(Args)>
class Currying<take_ownership, std::function<R(Args...)>, std::index_sequence<I...>, TupArgs...> {
public:
    using FunctionType = std::function<R(Args...)>;
    using ArgumentsTupleType = std::conditional_t<
        take_ownership,
        std::tuple<std::decay_t<TupArgs>...>,
        std::tuple<TupArgs...>>;

private:
    FunctionType function;
    ArgumentsTupleType argumentsTuple;

    static constexpr std::size_t totalNumOfArgs = sizeof...(Args);
    static constexpr std::size_t currentNumOfArgs = sizeof...(I);

public:
    Currying() = delete;
    template <typename T, bool B = true, typename... U>
    explicit Currying(T&& function, ArgOwnership<B> /* ownership */ = ArgOwnership<B> {}, U&&... arguments)
        : function(std::forward<T>(function))
        , argumentsTuple(std::forward<U>(arguments)...)
    {
    }

    // Call operator for lvalue references
    template <typename... U>
    auto operator()(U&&... rest) & -> decltype(auto)
    {
        return callFunction(std::get<I>(argumentsTuple)..., std::forward<U>(rest)...);
    }

    // Call operator for rvalue references
    template <typename... U>
    auto operator()(U&&... rest) && -> decltype(auto)
    {
        return callFunction(std::move(std::get<I>(argumentsTuple))..., std::forward<U>(rest)...);
    }

    // Getters for function and arguments
    auto getFunction() const noexcept -> const FunctionType& { return function; }
    auto getArguments() const noexcept -> const ArgumentsTupleType& { return argumentsTuple; }

    // Static members for argument count information
    static constexpr auto argCount() noexcept -> std::size_t { return totalNumOfArgs; }
    static constexpr auto argsFilled() noexcept -> std::size_t { return currentNumOfArgs; }
    static constexpr auto argsLeft() noexcept -> std::size_t { return argCount() - argsFilled(); }

private:
    // Helper function to call the function or create new Currying class
    template <typename... T>
    auto callFunction(T&&... arguments) const -> decltype(auto)
    {
        constexpr auto newNumOfArgs = sizeof...(T);
        static_assert(newNumOfArgs <= totalNumOfArgs, "Too many arguments passed to function");

        if constexpr (newNumOfArgs == totalNumOfArgs) {
            return function(std::forward<T>(arguments)...);
        } else {
            return v1::Currying(function, ArgOwnership<take_ownership> {}, std::forward<T>(arguments)...);
        }
    }
};

template <typename T, bool B = true, typename... U>
Currying(T&& function, ArgOwnership<B>, U&&... arguments)
    -> Currying<B, decltype(std::function { std::forward<T>(function) }), std::make_index_sequence<sizeof...(U)>,
        std::conditional_t<B, std::decay_t<decltype(std::forward<U>(arguments))>, decltype(std::forward<U>(arguments))>...>;

} // namespace currying::v1

#endif // CURRY_FUNCTION_HPP
