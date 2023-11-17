#pragma once
#ifndef CURRY_FUNCTION_HPP
#define CURRY_FUNCTION_HPP

#include <functional>
#include <tuple>
#include <utility>

namespace currying {

template <typename R, typename... Args>
struct CurryingFunction {
    template<typename... T>
    class Currying;

    // Solely used to deduce template types
    explicit CurryingFunction(const std::function<R(Args...)> &/*unused*/){}
};

template <typename R, typename... Args>
template <typename... T>
class CurryingFunction<R, Args...>::Currying {
    std::function<R(Args...)> func;
    std::tuple<T...> tuple_args;

    static constexpr auto TOT_NUM_OF_ARGS = sizeof...(Args);
    static constexpr auto CUR_NUM_OF_ARGS = sizeof...(T);
public:
    Currying() = delete;
    template<typename U>
    explicit Currying(U &&func, T &&...args) : func(std::forward<U>(func)), tuple_args(std::forward_as_tuple(args...)) {}

    template<typename... U>
    auto operator()(U &&...rest) -> decltype(auto)
    {
        constexpr auto NEW_NUM_OF_ARGS = sizeof...(T) + sizeof...(U);
        static_assert(NEW_NUM_OF_ARGS <= TOT_NUM_OF_ARGS, "Too many arguments passed to function");

        auto new_tuple_args = std::tuple_cat(tuple_args, std::forward_as_tuple(rest...));

        if constexpr (NEW_NUM_OF_ARGS == TOT_NUM_OF_ARGS) {
            return (call_func(new_tuple_args));
        } else {
            return (apply_arguments(std::move(new_tuple_args), std::make_index_sequence<std::tuple_size_v<std::decay_t<decltype(new_tuple_args)>>>{}));
        }
    }

private:
    template<typename U>
    auto call_func(U &&args) -> decltype(auto) {
        return (std::apply(func, std::forward<U>(args)));
    }

    template<typename U, std::size_t ...I>
    auto apply_arguments(U &&args, std::index_sequence<I...> /*unused*/) -> decltype(auto) {
        using NewCurryingType = Currying<std::tuple_element_t<I, U>...>;
        return (NewCurryingType(func, std::get<I>(std::forward<U>(args))...));
    }
};

template<typename T>
auto make_currying(T &&func_arg)
{
    // Convert to std::function
    auto func = std::function{ std::forward<T>(func_arg) };

    // Deduce correct types for Currying and CurryingFunction
    using CurryingFunctionType = decltype(CurryingFunction(func));
    using CurryingType = typename CurryingFunctionType::template Currying<>;

    // Construct object
    return CurryingType(std::move(func));
}

const inline auto curry_function = [](auto&& func) -> decltype(auto) {
    // Helper functions
    auto apply_tuple = []<std::size_t... I>(
                           auto&& func, auto&& arg_tuple,
                           std::index_sequence<I...>) -> decltype(auto) {
        return (func(std::get<I>(std::forward<decltype(arg_tuple)>(arg_tuple))...));
    };

    auto apply_from_tuple = [apply_tuple](auto&& func,
                                auto&& arg_tuple) -> decltype(auto) {
        static constexpr auto size = std::tuple_size_v<std::decay_t<decltype(arg_tuple)>>;
        return (apply_tuple(std::forward<decltype(func)>(func),
            std::forward<decltype(arg_tuple)>(arg_tuple),
            std::make_index_sequence<size> {}));
    };

    // Convert callable object 'func' to std::function
    return ([apply_from_tuple]<typename R, typename... Args>(
                std::function<R(Args...)>&& func) -> decltype(auto) {
        // This is the function that will be returned.
        auto curried_recurse =
            [apply_from_tuple, func = std::forward<decltype(func)>(func)](
                auto&& curried_recurse, auto&&... args) -> decltype(auto) {
            static_assert(sizeof...(args) <= sizeof...(Args),
                "Too many arguments passed to function");

            if constexpr (sizeof...(args) == sizeof...(Args)) {
                // If we have all the arguments, call the function.
                return (apply_from_tuple(func, std::forward_as_tuple(args...)));
            } else {
                // If we don't have all the arguments, return a new function that
                // takes the remaining arguments.
                return ([apply_from_tuple, curried_recurse,
                            args_tuple = std::forward_as_tuple(args...)](
                            auto&&... rest) -> decltype(auto) {
                    return (apply_from_tuple(
                        curried_recurse,
                        std::tuple_cat(std::forward_as_tuple(curried_recurse), args_tuple,
                            std::forward_as_tuple(rest...))));
                });
            }
        };

        // Return the function.
        return (curried_recurse(curried_recurse));
    }(std::function { std::forward<decltype(func)>(func) }));
};
} // namespace currying

#endif // CURRY_FUNCTION_HPP
