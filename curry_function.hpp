#pragma once
#ifndef CURRY_FUNCTION_HPP
#define CURRY_FUNCTION_HPP

#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace currying {

template <bool>
struct ArgOwnership { };

using OwnArguments = ArgOwnership<true>;
using ReferenceArguments = ArgOwnership<false>;

inline constexpr auto own_arguments = OwnArguments {};
inline constexpr auto reference_arguments = ReferenceArguments {};

template <bool, typename, typename, typename...>
class Currying; /* undefined */

template <bool take_ownership, typename... TupArgs, std::size_t... I, typename R, typename... Args>
requires(sizeof...(I) == sizeof...(TupArgs) && sizeof...(TupArgs) <= sizeof...(Args))
class Currying<take_ownership, std::function<R(Args...)>, std::index_sequence<I...>, TupArgs...> {
public:
    using FunctionType = std::function<R(Args...)>;
    using ArgsTupleType = std::conditional_t<
        take_ownership,
        std::tuple<std::decay_t<TupArgs>...>,
        std::tuple<TupArgs...>>;

private:
    FunctionType func;
    ArgsTupleType args_tuple;

    static constexpr auto tot_num_of_args = sizeof...(Args);
    static constexpr auto cur_num_of_args = sizeof...(I);

public:
    Currying() = delete;
    template <typename T, bool B = true, typename... U>
    explicit Currying(T&& func, ArgOwnership<B> /*unused*/ = ArgOwnership<B> {}, U&&... args)
        : func(std::forward<T>(func))
        , args_tuple(std::forward<U>(args)...)
    {
    }

private:
    template <typename... T>
    auto call_func(T&&... args) const -> decltype(auto)
    {
        constexpr auto new_num_of_args = sizeof...(T);
        static_assert(new_num_of_args <= tot_num_of_args, "Too many arguments passed to function");

        if constexpr (new_num_of_args == tot_num_of_args) {
            return func(std::forward<T>(args)...);
        } else {
            return currying::Currying(func, ArgOwnership<take_ownership> {}, std::forward<T>(args)...);
        }
    }

public:
    template <typename... U>
    auto operator()(U&&... rest) & -> decltype(auto)
    {
        return call_func(std::get<I>(args_tuple)..., std::forward<U>(rest)...);
    }

    template <typename... U>
    auto operator()(U&&... rest) && -> decltype(auto)
    {
        return call_func(std::move(std::get<I>(args_tuple))..., std::forward<U>(rest)...);
    }

    auto get_func() const
    {
        return func;
    }

    auto get_args() const
    {
        return args_tuple;
    }

    static constexpr auto arg_count() noexcept
    {
        return tot_num_of_args;
    }

    static constexpr auto args_filled() noexcept
    {
        return cur_num_of_args;
    }

    static constexpr auto args_left() noexcept
    {
        return arg_count() - args_filled();
    }
};

template <typename T, bool B = true, typename... U>
Currying(T&& func, ArgOwnership<B> = ArgOwnership<B> {}, U&&... args)
    -> Currying<B, decltype(std::function { std::forward<T>(func) }), std::make_index_sequence<sizeof...(U)>,
std::conditional_t<B, std::decay_t<decltype(std::forward<U>(args))>, decltype(std::forward<U>(args))>...>;

} // namespace currying

#endif // CURRY_FUNCTION_HPP
