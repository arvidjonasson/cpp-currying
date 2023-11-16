#define BOOST_TEST_MODULE CurryTest
#include <boost/test/included/unit_test.hpp>
#include "curry_function.hpp"

BOOST_AUTO_TEST_CASE(lambda_test)
{
    auto add_lambda = [](int a, int b, int c) -> int {
        return a + b + c;
    };

    auto curried_add = curry_function(add_lambda);

    BOOST_CHECK_EQUAL(curried_add(10)(11)(12), 10 + 11 + 12);
    BOOST_CHECK_EQUAL(curried_add(10, 11)(12), 10 + 11 + 12);
    BOOST_CHECK_EQUAL(curried_add(10)(11, 12), 10 + 11 + 12);
    BOOST_CHECK_EQUAL(curried_add(10, 11, 12), 10 + 11 + 12);
}

BOOST_AUTO_TEST_CASE(lambda_capture_test)
{
    int x = -100;
    auto add_lambda = [&x](int a, int b, int c) -> int {
        return a + b + c + x;
    };

    auto curried_add = curry_function(add_lambda);

    x = 10;
    BOOST_CHECK_EQUAL(curried_add(10)(11)(12), 10 + 11 + 12 + x);
    x = 11;
    BOOST_CHECK_EQUAL(curried_add(10, 11)(12), 10 + 11 + 12 + x);
    x = 12;
    BOOST_CHECK_EQUAL(curried_add(10)(11, 12), 10 + 11 + 12 + x);
    x = 13;
    BOOST_CHECK_EQUAL(curried_add(10, 11, 12), 10 + 11 + 12 + x);
}

int add_function(int a, int b, int c) {
    return a + b + c;
}

BOOST_AUTO_TEST_CASE(function_test)
{

    auto curried_add = curry_function(add_function);

    BOOST_CHECK_EQUAL(curried_add(10)(11)(12), 10 + 11 + 12);
    BOOST_CHECK_EQUAL(curried_add(10, 11)(12), 10 + 11 + 12);
    BOOST_CHECK_EQUAL(curried_add(10)(11, 12), 10 + 11 + 12);
    BOOST_CHECK_EQUAL(curried_add(10, 11, 12), 10 + 11 + 12);
}

BOOST_AUTO_TEST_CASE(reference_test)
{
    auto setter = [](int &set, int val) {
        set = val;
    };

    int x = 5;

    auto curried_add = curry_function(setter);

    auto f = curried_add(x);

    BOOST_CHECK_EQUAL(x, 5);
    f(10);
    BOOST_CHECK_EQUAL(x, 10);
    f(20);
    BOOST_CHECK_EQUAL(x, 20);
}
