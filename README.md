
# Beltino's Test Library

A simple and header-only C++ test library for unit-tests.

## Install

There are a few ways of using this library.
In any case, it is your responsibility to build your test environment.

### Copy & paste

Just copy the `btl.h` to your project.

### CMake

1. Clone the repository into your project (via submodules or whatever)
2. In your `CMakeLists.txt` file, add the line:

```python
add_subdirectory("${PROJECT_SOURCE_DIR}/path/to/btl")
target_link_libraries(YourTarget BTL)
```

## Usage

###  For the actual test files

* `DESCRIBE_CLASS(CLASS_TYPE) {}`  
    Describe tests of a class type. Contents must be enclosed in curly braces.
* `DESCRIBE_TEST(CLASS_TYPE) {};`  
    Describe single unit test. Contents must be enclosed in curly braces and must end with semicolon.
* `ASSERT_IS_TRUE(VALUE)`  
    Compare if provided value is true.
* `ASSERT_IS_FALSE(VALUE)`  
    Compare if provided value is false.
* `ASSERT_ARE_EQUAL(VALUE, EXPECTED)`  
    Compare if provided value evaluates the same as the provided expected value.
* `ASSERT_ARE_SAME(VALUE, EXPECTED)`  
    Compare if provided value instance is exactly the same as provided expected instance. Basically, this will cast both `&VALUE` and `&EXPECTED` to `(void*)` and compare their memory addresses.
* `ASSERT_ARRAYS_ARE_EQUAL(VALUE, EXPECTED, RANGE_START, RANGE_LENGTH)`  
    Compare if provided array contains values that evaluate equal to the provided expected array. You **ABSOLUTELY MUST** provide the range of comparison.

### For test runner

* `void btl::TestRunner<T>::run()`  
    Run this for each tested class. Successful tests are printed to `stdout`. If there are errors, they will be printed to `stderr`. The global error count will be updated upon errors as well.
* `bool btl::has_errors()`  
    Check if there were errors in the tests. Return true if there were errors.

### Code Example

```C++
// your_type.spec.cxx

#include "your_type.h"

#include <btl.h>

DESCRIBE_CLASS(YourType) {
    DESCRIBE_TEST(method_name, CalledMethod, ChangeInstanceToExpectedState) {
        YourType foo;
        foo.method_name();
        ASSERT_ARE_EQUAL(foo.get_state(), YourType::EXPECTED_STATE);
    };

    DESCRIBE_TEST(method_name, CreatedSituation, ExpectResult) {
        // <insert test code here>
    };

    // <insert other tests here>
}
```

```C++
// test.cxx

#include "your_type.spec.cxx"

#include <btl.h>

int main() {
    btl::TestRunner<YourType>::run();

    if (btl::has_errors()) {
        // Error exit code
        return -1;
    }

    // Success exit code
    return 0;
}
```

### Real example

If you want to see a real example of some project that uses this, check out [Slippy's Math Library](https://github.com/aki-cat/sml).
