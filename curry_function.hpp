#pragma once
#ifndef CURRY_FUNCTION_HPP
#define CURRY_FUNCTION_HPP

#include <functional>
#include <tuple>
#include <utility>

namespace currying {

template <typename R, typename... Args>
struct CurryingFunction {
    template <std::size_t... I>
        requires(sizeof...(I) <= sizeof...(Args))
    class Currying;

    // Solely used to deduce template types
    explicit CurryingFunction(const std::function<R(Args...)>& /*unused*/) { }
};

template <typename R, typename... Args>
template <std::size_t... I>
    requires(sizeof...(I) <= sizeof...(Args))
class CurryingFunction<R, Args...>::Currying {

    // Create a tuple with the same types as func's args
    using TupleType = std::tuple<std::tuple_element_t<I, std::tuple<Args...>>...>;

    std::function<R(Args...)> func;
    TupleType tuple_args;

    static constexpr auto tot_num_of_args = sizeof...(Args);
    static constexpr auto cur_num_of_args = sizeof...(I);

public:
    Currying() = delete;
    template <typename U>
    explicit Currying(U&& func, auto&&... args)
        : func(std::forward<U>(func))
        , tuple_args(std::forward_as_tuple(args...))
    {
    }

    template <typename... U>
    auto operator()(U&&... rest) const -> decltype(auto)
    {
        constexpr auto new_num_of_args = sizeof...(I) + sizeof...(U);
        static_assert(new_num_of_args <= tot_num_of_args, "Too many arguments passed to function");

        if constexpr (new_num_of_args == tot_num_of_args) {
            return (func(std::get<I>(tuple_args)..., std::forward<U>(rest)...));
        } else {
            return (apply_arguments(std::make_index_sequence<new_num_of_args> {}, std::get<I>(tuple_args)..., std::forward<U>(rest)...));
        }
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

private:
    template <typename... U, std::size_t... Idx>
    auto apply_arguments(std::index_sequence<Idx...> /*unused*/, U&&... args) const -> decltype(auto)
    {
        using NewCurryingType = Currying<Idx...>;
        return (NewCurryingType(func, std::forward<U>(args)...));
    }
};

template <typename T>
auto make_currying(T&& func_arg)
{
    // Convert to std::function
    auto func = std::function { std::forward<T>(func_arg) };

    // Deduce correct types for Currying and CurryingFunction
    using CurryingFunctionType = decltype(CurryingFunction(func));
    using CurryingType = typename CurryingFunctionType::template Currying<>;

    // Construct object
    return CurryingType(std::move(func));
}
} // namespace currying

#endif // CURRY_FUNCTION_HPP
