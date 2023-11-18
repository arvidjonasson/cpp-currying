#define BOOST_TEST_MODULE CurryingTest
#include <boost/test/unit_test.hpp>
#include "curry_function.hpp"

int add(int a, int b) {
    return a + b;
}

int multiply(int a, int b, int c) {
    return a * b * c;
}

BOOST_AUTO_TEST_SUITE(CurryingTests)

BOOST_AUTO_TEST_CASE(TestBasicCurrying) {
    auto curriedAdd = currying::v1::Currying(add, currying::v1::ownArguments, 5);
    BOOST_CHECK_EQUAL(curriedAdd(10), 15); // Should return 15
}

BOOST_AUTO_TEST_CASE(TestOwnership) {
    int a = 5, b = 10;
    auto curriedAddRef = currying::v1::Currying(add, currying::v1::referenceArguments, a);
    a = 15; // Modify a after currying
    BOOST_CHECK_EQUAL(curriedAddRef(b), 25); // Should reflect the change in 'a'

    auto curriedAddOwn = currying::v1::Currying(add, currying::v1::ownArguments, a);
    a = 20; // Modify a after currying
    BOOST_CHECK_EQUAL(curriedAddOwn(b), 25); // Should not reflect the change in 'a' (ownArguments)
}

BOOST_AUTO_TEST_CASE(TestFunctionalityWithDifferentArgTypes) {
    auto curriedMultiply = currying::v1::Currying(multiply, currying::v1::ownArguments, 2);
    BOOST_CHECK_EQUAL(curriedMultiply(3, 4), 24); // 2 * 3 * 4 = 24
}

BOOST_AUTO_TEST_CASE(TestReferenceQualifierOverloads) {
    auto curriedAdd = currying::v1::Currying(add, currying::v1::ownArguments, 5);
    BOOST_CHECK_EQUAL(std::move(curriedAdd)(10), 15); // Using rvalue overload
}

BOOST_AUTO_TEST_CASE(TestArgCountFunctions) {
    auto curriedAdd = currying::v1::Currying(add, currying::v1::ownArguments, 5);
    BOOST_CHECK_EQUAL(curriedAdd.argCount(), 2);  // add function takes 2 arguments
    BOOST_CHECK_EQUAL(curriedAdd.argsFilled(), 1); // 1 argument already provided
    BOOST_CHECK_EQUAL(curriedAdd.argsLeft(), 1);   // 1 argument left to be provided
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(CurryingCallableTests)

BOOST_AUTO_TEST_CASE(TestWithNonCapturingLambda) {
    auto lambda = [](int x, int y) { return x - y; };
    auto curriedLambda = currying::v1::Currying(lambda, currying::v1::ownArguments, 10);
    BOOST_CHECK_EQUAL(curriedLambda(5), 5); // 10 - 5 = 5
}

BOOST_AUTO_TEST_CASE(TestWithCapturingLambda) {
    int z = 5;
    auto capturingLambda = [z](int x, int y) { return x * y + z; };
    auto curriedLambda = currying::v1::Currying(capturingLambda, currying::v1::ownArguments, 3);
    BOOST_CHECK_EQUAL(curriedLambda(4), 17); // 3 * 4 + 5 = 17
}

BOOST_AUTO_TEST_CASE(TestWithStdFunction) {
    std::function<int(int, int)> stdFunc = [](int a, int b) { return a + b; };
    auto curriedStdFunc = currying::v1::Currying(stdFunc, currying::v1::ownArguments, 7);
    BOOST_CHECK_EQUAL(curriedStdFunc(8), 15); // 7 + 8 = 15
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(CurryingAdvancedTests)

BOOST_AUTO_TEST_CASE(TestLambdaCapturingByReference) {
    int capturedValue = 10;
    auto capturingLambda = [&capturedValue](int x) { return x * capturedValue; };
    auto curriedLambda = currying::v1::Currying(capturingLambda, currying::v1::ownArguments, 5);
    capturedValue = 2; // Change the captured variable after currying
    BOOST_CHECK_EQUAL(curriedLambda(), 10); // 5 * 2 = 10
}

BOOST_AUTO_TEST_CASE(TestWithNonPrimitiveArguments) {
    struct CustomType {
        int value;
        CustomType(int v) : value(v) {}
        int multiply(const CustomType& other) const { return value * other.value; }
    };

    CustomType obj1(3), obj2(4);
    auto lambda = [](const CustomType& a, const CustomType& b) { return a.multiply(b); };
    auto curriedLambda = currying::v1::Currying(lambda, currying::v1::ownArguments, obj1);
    BOOST_CHECK_EQUAL(curriedLambda(obj2), 12); // 3 * 4 = 12
}

BOOST_AUTO_TEST_SUITE_END()
