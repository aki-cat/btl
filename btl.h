#ifndef BELTINOS_TEST_LIBRARY_H_
#define BELTINOS_TEST_LIBRARY_H_

#include <cfloat>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

// END OF TEMP MACROS

namespace std {

// Some std::to_string() extensions

const string& to_string(const string& s) { return s; }
string to_string(const bool& value) { return value ? "true" : "false"; }

}  // namespace std

namespace btl {

// NUMBER CONSTANTS

#define BTL_FLOAT_EPSILON 1e-5f;
#define BTL_DOUBLE_EPSILON 1e-7;

// OUTPUT COLOR CODES

#define LOCATION_TEXT_COLOUR "\033[94;1m"
#define FAILURE_TEXT_COLOUR "\033[91m"
#define NORMAL_TEXT_COLOUR "\033[0m"
#define NORMAL_TEXT_COLOUR_BOLD "\033[0;1m"
#define SUCCESS_TEXT_COLOUR "\033[92;1m"
#define CLASS_TEXT_COLOUR "\033[95;1m"

// UTILITY

#define S1(SYMBOL) #SYMBOL
#define S2(SYMBOL) S1(SYMBOL)
#define LOCATION_MESSAGE LOCATION_TEXT_COLOUR __FILE__ "(" S2(__LINE__) "):" NORMAL_TEXT_COLOUR
#define ASSERTION_FAIL_MESSAGE \
    LOCATION_MESSAGE FAILURE_TEXT_COLOUR " Assertion failed! ❌ " NORMAL_TEXT_COLOUR

#define PRINT_TEST_SUCCESS() std::cout << SUCCESS_TEXT_COLOUR "OK!" NORMAL_TEXT_COLOUR << std::endl
#define PRINT_TEST_FAILURE(ERROR_MSG) std::cerr << ASSERTION_FAIL_MESSAGE << ERROR_MSG << std::endl
#define PRINT_TEST_DESCRIPTION()                                                   \
    std::cout << "\t" << CLASS_TEXT_COLOUR << btl::TestRunner<class_t>::CLASS_NAME \
              << NORMAL_TEXT_COLOUR << "::" << CURRENT_TEST << " "

#define ASSERT(EXPRESSION, ERROR_MSG)  \
    PRINT_TEST_DESCRIPTION();          \
    if (!(EXPRESSION)) {               \
        btl::BTL_ERROR_COUNT++;        \
        PRINT_TEST_FAILURE(ERROR_MSG); \
    } else {                           \
        PRINT_TEST_SUCCESS();          \
    }

static uint32_t BTL_ERROR_COUNT = 0;

static bool has_errors() { return BTL_ERROR_COUNT > 0; }

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

    static const char* CLASS_NAME = nullptr;

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
        ASSERT(btl::are_equal(VALUE, EXPECTED), stream.str());                            \
    }

#define ASSERT_ARE_SAME(VALUE, EXPECTED)                          \
    {                                                             \
        void* value_ptr = reinterpret_cast<void*>(&VALUE);        \
        void* expected_ptr = reinterpret_cast<void*>(&EXPECTED);  \
        std::stringstream stream{};                               \
        stream << expected_ptr << " expected; got " << value_ptr; \
        ASSERT(btl::are_equal(VALUE, EXPECTED), stream.str());    \
    }

#define ASSERT_IS_TRUE(VALUE) ASSERT_ARE_EQUAL((VALUE), true);
#define ASSERT_IS_FALSE(VALUE) ASSERT_ARE_EQUAL((VALUE), false);

#define ASSERT_ARRAYS_ARE_EQUAL(VALUE, EXPECTED, RANGE_START, RANGE_LENGTH)                   \
    {                                                                                         \
        PRINT_TEST_DESCRIPTION();                                                             \
        std::stringstream stream{};                                                           \
        stream << std::endl;                                                                  \
        bool success = true;                                                                  \
        for (uint32_t range_index = RANGE_START; range_index < RANGE_LENGTH; range_index++) { \
            if (!(btl::are_equal(VALUE[range_index], EXPECTED[range_index]))) {               \
                success = false;                                                              \
                stream << "\t\t- " << std::to_string(EXPECTED[range_index])                   \
                       << " expected at index #" << range_index << "; got "                   \
                       << std::to_string(VALUE[range_index]) << std::endl;                    \
            }                                                                                 \
        }                                                                                     \
        if (success) {                                                                        \
            PRINT_TEST_SUCCESS();                                                             \
        } else {                                                                              \
            BTL_ERROR_COUNT++;                                                                \
            PRINT_TEST_FAILURE(stream.str());                                                 \
        };                                                                                    \
    }

#define DESCRIBE_CLASS(CLASS_TYPE)                                            \
    template <>                                                               \
    static const char* btl::TestRunner<CLASS_TYPE>::CLASS_NAME = #CLASS_TYPE; \
    template <>                                                               \
    btl::TestRunner<CLASS_TYPE>::TestRunner() : _tests()

#define DESCRIBE_TEST(METHOD, SITUATION, EXPECTATION)                            \
    _tests.push_back([]() {});                                                   \
    _tests[_tests.size() - 1] = [CURRENT_TEST = NORMAL_TEXT_COLOUR_BOLD #METHOD  \
                                 "()" NORMAL_TEXT_COLOUR " should " #EXPECTATION \
                                 " when " #SITUATION]()

// Don't leak macros to the rest of the code!

#undef BTL_FLOAT_EPSILON
#undef BTL_DOUBLE_EPSILON

#undef LOCATION_TEXT_COLOUR
#undef FAILURE_TEXT_COLOUR
#undef NORMAL_TEXT_COLOUR
#undef NORMAL_TEXT_COLOUR_BOLD
#undef SUCCESS_TEXT_COLOUR
#undef CLASS_TEXT_COLOUR

#undef S1
#undef S2
#undef LOCATION_MESSAGE
#undef ASSERTION_FAIL_MESSAGE
#undef PRINT_TEST_SUCCESS
#undef PRINT_TEST_FAILURE
#undef PRINT_TEST_DESCRIPTION
#undef ASSERT

}  // namespace btl

#endif
