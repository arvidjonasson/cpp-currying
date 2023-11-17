#define BOOST_TEST_MODULE CurryTest
#include "curry_function.hpp"
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE(lambda_test)
{
    auto add_lambda = [](int a, int b, int c) -> int { return a + b + c; };

    auto curried_add = currying::curry_function(add_lambda);

    BOOST_CHECK_EQUAL(curried_add(10)(11)(12), 10 + 11 + 12);
    BOOST_CHECK_EQUAL(curried_add(10, 11)(12), 10 + 11 + 12);
    BOOST_CHECK_EQUAL(curried_add(10)(11, 12), 10 + 11 + 12);
    BOOST_CHECK_EQUAL(curried_add(10, 11, 12), 10 + 11 + 12);
}

BOOST_AUTO_TEST_CASE(lambda_capture_test)
{
    int x = -100;
    auto add_lambda = [&x](int a, int b, int c) -> int { return a + b + c + x; };

    auto curried_add = currying::curry_function(add_lambda);

    x = 10;
    BOOST_CHECK_EQUAL(curried_add(10)(11)(12), 10 + 11 + 12 + x);
    x = 11;
    BOOST_CHECK_EQUAL(curried_add(10, 11)(12), 10 + 11 + 12 + x);
    x = 12;
    BOOST_CHECK_EQUAL(curried_add(10)(11, 12), 10 + 11 + 12 + x);
    x = 13;
    BOOST_CHECK_EQUAL(curried_add(10, 11, 12), 10 + 11 + 12 + x);
}

auto add_function(int a, int b, int c) -> int { return a + b + c; }

BOOST_AUTO_TEST_CASE(function_test)
{

    auto curried_add = currying::curry_function(add_function);

    BOOST_CHECK_EQUAL(curried_add(10)(11)(12), 10 + 11 + 12);
    BOOST_CHECK_EQUAL(curried_add(10, 11)(12), 10 + 11 + 12);
    BOOST_CHECK_EQUAL(curried_add(10)(11, 12), 10 + 11 + 12);
    BOOST_CHECK_EQUAL(curried_add(10, 11, 12), 10 + 11 + 12);
}

BOOST_AUTO_TEST_CASE(reference_test)
{
    auto setter = [](int& set, int val) { set = val; };

    int x = 5;

    auto curried_add = currying::curry_function(setter);

    auto f = curried_add(x);

    BOOST_CHECK_EQUAL(x, 5);
    f(10);
    BOOST_CHECK_EQUAL(x, 10);
    f(20);
    BOOST_CHECK_EQUAL(x, 20);
}

BOOST_AUTO_TEST_CASE(test_class)
{
    auto x = currying::make_currying(add_function);

    auto y = x(10);

    BOOST_CHECK_EQUAL(y(10, 0), 20);

    auto z = y(5);

    BOOST_CHECK_EQUAL(z(5), 20);
}

BOOST_AUTO_TEST_CASE(reference_test_class)
{
    auto setter = [](int& set, int val) { set = val; };

    int x = 5;

    auto curried_add = currying::make_currying(setter);

    auto f = curried_add(x);

    BOOST_CHECK_EQUAL(x, 5);
    f(10);
    BOOST_CHECK_EQUAL(x, 10);
    f(20);
    BOOST_CHECK_EQUAL(x, 20);
}

BOOST_AUTO_TEST_CASE(function_test_class)
{
    auto curried_add = currying::make_currying(add_function);

    BOOST_CHECK_EQUAL(curried_add(10)(11)(12), 10 + 11 + 12);
    BOOST_CHECK_EQUAL(curried_add(10, 11)(12), 10 + 11 + 12);
    BOOST_CHECK_EQUAL(curried_add(10)(11, 12), 10 + 11 + 12);
    BOOST_CHECK_EQUAL(curried_add(10, 11, 12), 10 + 11 + 12);

    int x = 10;

    auto f = curried_add(x);

    x = 20;
    BOOST_CHECK_EQUAL(f(0, 0), 10);
}
