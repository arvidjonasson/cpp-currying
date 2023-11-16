#pragma once
#ifndef CURRY_FUNCTION_HPP
#define CURRY_FUNCTION_HPP

#include <utility>
#include <functional>


inline auto curry_function = [](auto &&func) {
    return []<typename R, typename... Args>(std::function<R(Args...)> &&func) {

        // This is the function that will be returned.
        auto curried_recurse = [func = std::forward<decltype(func)>(func)](auto &&curried_recurse, auto&&... args) {
            static_assert(sizeof...(args) <= sizeof...(Args), "Too many arguments passed to function");

            if constexpr (sizeof...(args) == sizeof...(Args)) {
                // If we have all the arguments, call the function.
                return func(std::forward<decltype(args)>(args)...);
            } else {
                // If we don't have all the arguments, return a new function that takes the remaining arguments.
                return [curried_recurse, ...args = std::forward<decltype(args)>(args)](auto&&... rest) {
                    return curried_recurse(curried_recurse, args..., std::forward<decltype(rest)>(rest)...);
                };
            }
        };

        // Return the function.
        return curried_recurse(curried_recurse);
    }(std::function{std::forward<decltype(func)>(func)});
};

#endif //CURRY_FUNCTION_HPP
