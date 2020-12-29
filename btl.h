#ifndef BELTINOS_TEST_LIBRARY_H_
#define BELTINOS_TEST_LIBRARY_H_

#include <cfloat>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace std {

// Some std::to_string() extensions

const string& to_string(const string& s) { return s; }
string to_string(const bool& value) { return value ? "true" : "false"; }

// I recommend implementing std::to_string for your types.

}  // namespace std

namespace btl {

// NUMBER CONSTANTS

// Espilons are different (and more lenient) than the default ones because for most mathematical
// uses, float is naturally innaccurate, and there's nothing we can do about it.
// If you don't like that, feel free to use different epsilon values by altering these.
#define BTL_FLOAT_EPSILON 1e-5f;
#define BTL_DOUBLE_EPSILON 1e-7;

// OUTPUT COLOR CODES

#define BTL_LOCATION_TEXT_COLOUR "\033[94;1m"
#define BTL_FAILURE_TEXT_COLOUR "\033[91m"
#define BTL_NORMAL_TEXT_COLOUR "\033[0m"
#define BTL_NORMAL_TEXT_COLOUR_BOLD "\033[0;1m"
#define BTL_SUCCESS_TEXT_COLOUR "\033[92;1m"
#define BTL_CLASS_TEXT_COLOUR "\033[95;1m"

// UTILITY

#define BTL_SYMBOL_STRING(SYMBOL) #SYMBOL
#define BTL_GET_SYMBOL_NAME(SYMBOL) BTL_SYMBOL_STRING(SYMBOL)
#define BTL_GET_LOCATION_MESSAGE \
    BTL_LOCATION_TEXT_COLOUR __FILE__ "(" BTL_GET_SYMBOL_NAME(__LINE__) ")" \
                                                                        ":" BTL_NORMAL_TEXT_COLOUR
#define BTL_ASSERTION_FAIL_MESSAGE                   \
    BTL_GET_LOCATION_MESSAGE BTL_FAILURE_TEXT_COLOUR \
        " Assertion failed! "                        \
        "❌ " BTL_NORMAL_TEXT_COLOUR

#define BTL_PRINT_TEST_SUCCESS() \
    std::cout << BTL_SUCCESS_TEXT_COLOUR "OK!" BTL_NORMAL_TEXT_COLOUR << std::endl
#define BTL_PRINT_TEST_FAILURE(ERROR_MSG) \
    std::cerr << BTL_ASSERTION_FAIL_MESSAGE << ERROR_MSG << std::endl
#define BTL_PRINT_TEST_DESCRIPTION()                                                   \
    std::cout << "\t" << BTL_CLASS_TEXT_COLOUR << btl::TestRunner<class_t>::CLASS_NAME \
              << BTL_NORMAL_TEXT_COLOUR << "::" << CURRENT_TEST << " "

#define BTL_ASSERT(EXPRESSION, ERROR_MSG)  \
    BTL_PRINT_TEST_DESCRIPTION();          \
    if (!(EXPRESSION)) {                   \
        btl::_BTL_ERROR_COUNT++;           \
        BTL_PRINT_TEST_FAILURE(ERROR_MSG); \
    } else {                               \
        BTL_PRINT_TEST_SUCCESS();          \
    }

// END OF INTERNAL UTILITY MACROS

// This is public, but try to use btl::has_errors() instead.
static size_t _BTL_ERROR_COUNT = 0;

static bool has_errors() { return _BTL_ERROR_COUNT > 0; }

template <typename T>
bool are_equal(const T& a, const T& b) {
    return a == b;
}

template <>
bool are_equal(const float& a, const float& b) {
    return std::fabs(a - b) <= BTL_FLOAT_EPSILON;
}

template <>
bool are_equal(const double& a, const double& b) {
    return std::fabs(a - b) <= BTL_DOUBLE_EPSILON;
}

template <typename T>
class TestRunner {
   public:
    TestRunner();

    static const char* CLASS_NAME;

    static void run() {
        const TestRunner<T> runner = TestRunner<T>();
        for (const auto& test : runner._tests) {
            test();
        }
        std::cout << std::endl;
    }

   private:
    using class_t = T;
    std::vector<std::function<void(void)>> _tests;
};

#define ASSERT_ARE_EQUAL(VALUE, EXPECTED)                                                 \
    {                                                                                     \
        std::stringstream stream{};                                                       \
        stream << std::to_string(EXPECTED) << " expected; got " << std::to_string(VALUE); \
        BTL_ASSERT(btl::are_equal(VALUE, EXPECTED), stream.str());                        \
    }

#define ASSERT_ARE_SAME(VALUE, EXPECTED)                           \
    {                                                              \
        void* value_ptr = reinterpret_cast<void*>(&VALUE);         \
        void* expected_ptr = reinterpret_cast<void*>(&EXPECTED);   \
        std::stringstream stream{};                                \
        stream << expected_ptr << " expected; got " << value_ptr;  \
        BTL_ASSERT(btl::are_equal(VALUE, EXPECTED), stream.str()); \
    }

#define ASSERT_IS_TRUE(VALUE) ASSERT_ARE_EQUAL((VALUE), true);
#define ASSERT_IS_FALSE(VALUE) ASSERT_ARE_EQUAL((VALUE), false);

#define ASSERT_ARRAYS_ARE_EQUAL(VALUE, EXPECTED, RANGE_START, RANGE_LENGTH)                   \
    {                                                                                         \
        BTL_PRINT_TEST_DESCRIPTION();                                                         \
        std::stringstream stream{};                                                           \
        stream << std::endl;                                                                  \
        bool success = true;                                                                  \
        for (size_t idx = RANGE_START; idx < RANGE_LENGTH; idx++) {                           \
            if (!(btl::are_equal(VALUE[idx], EXPECTED[idx]))) {                               \
                success = false;                                                              \
                stream << "\t\t* " << std::to_string(EXPECTED[idx]) << " expected at index #" \
                       << idx << "; got " << std::to_string(VALUE[idx]) << std::endl;         \
            }                                                                                 \
        }                                                                                     \
        if (success) {                                                                        \
            BTL_PRINT_TEST_SUCCESS();                                                         \
        } else {                                                                              \
            _BTL_ERROR_COUNT++;                                                               \
            BTL_PRINT_TEST_FAILURE(stream.str());                                             \
        };                                                                                    \
    }

/*

=== Usage Example ===

#include <btl.h>

DESCRIBE_CLASS(YourType) {
    DESCRIBE_TEST(method_name, DescribedSituationInPresentPerfectTense,
                               DescribeExpectationInImperativeTense) {
        YourType foo;
        foo.method_name();
        ASSERT_ARE_EQUAL(foo.some_state(), YourType::EXPECTED_STATE);
    };
}

*/

#define DESCRIBE_CLASS(CLASS_TYPE)                                     \
    template <>                                                        \
    const char* btl::TestRunner<CLASS_TYPE>::CLASS_NAME = #CLASS_TYPE; \
    template <>                                                        \
    btl::TestRunner<CLASS_TYPE>::TestRunner() : _tests()

#define DESCRIBE_TEST(METHOD, SITUATION, EXPECTATION)                                \
    _tests.push_back([]() {});                                                       \
    _tests[_tests.size() - 1] = [CURRENT_TEST = BTL_NORMAL_TEXT_COLOUR_BOLD #METHOD  \
                                 "()" BTL_NORMAL_TEXT_COLOUR " should " #EXPECTATION \
                                 " when " #SITUATION]()

// Undefine what can be undefined

#undef BTL_FLOAT_EPSILON
#undef BTL_DOUBLE_EPSILON

/*

These are for internal use only, but cannot be undef:

BTL_LOCATION_TEXT_COLOUR
BTL_FAILURE_TEXT_COLOUR
BTL_NORMAL_TEXT_COLOUR
BTL_NORMAL_TEXT_COLOUR_BOLD
BTL_SUCCESS_TEXT_COLOUR
BTL_CLASS_TEXT_COLOUR
BTL_SYMBOL_STRING
BTL_GET_SYMBOL_NAME
BTL_GET_LOCATION_MESSAGE
BTL_ASSERTION_FAIL_MESSAGE
BTL_PRINT_TEST_SUCCESS
BTL_PRINT_TEST_FAILURE
BTL_PRINT_TEST_DESCRIPTION
BTL_ASSERT

*/

}  // namespace btl

#endif
