#pragma once
#ifndef CURRY_FUNCTION_HPP
#define CURRY_FUNCTION_HPP

#include <utility>
#include <tuple>
#include <functional>


inline auto curry_function = [](auto &&func) -> decltype(auto) {

    // Helper functions
    auto apply_tuple = []<std::size_t... I>(auto&& func, auto&& t, std::index_sequence<I...>) -> decltype(auto) {
        return (func(std::get<I>(std::forward<decltype(t)>(t))...));
    };

    auto apply_from_tuple = [apply_tuple](auto&& func, auto&& t) -> decltype(auto) {
        static constexpr auto size = std::tuple_size_v<std::decay_t<decltype(t)>>;
        return (apply_tuple(std::forward<decltype(func)>(func), std::forward<decltype(t)>(t), std::make_index_sequence<size>{}));
    };


    // Convert callable object 'func' to std::function
    return ([apply_from_tuple]<typename R, typename... Args>(std::function<R(Args...)> &&func) -> decltype(auto) {

        // This is the function that will be returned.
        auto curried_recurse =
            [apply_from_tuple, func = std::forward<decltype(func)>(func)]
        (auto &&curried_recurse, auto&&... args) -> decltype(auto) {
            static_assert(sizeof...(args) <= sizeof...(Args), "Too many arguments passed to function");

            if constexpr (sizeof...(args) == sizeof...(Args)) {
                // If we have all the arguments, call the function.
                return (apply_from_tuple(func, std::forward_as_tuple(args...)));
            } else {
                // If we don't have all the arguments, return a new function that takes the remaining arguments.
                return ([apply_from_tuple, curried_recurse, args_tuple = std::forward_as_tuple(args...)](auto&&... rest) -> decltype(auto) {
                    return (apply_from_tuple(curried_recurse, std::tuple_cat(std::forward_as_tuple(curried_recurse), args_tuple, std::forward_as_tuple(rest...))));
                });
            }
        };

        // Return the function.
        return (curried_recurse(curried_recurse));
    }(std::function{std::forward<decltype(func)>(func)}));
};

#endif //CURRY_FUNCTION_HPP
