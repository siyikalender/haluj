# haluj
## Description

This is a header only lightweight toolbox written in C++. It consists several small utilities, which are hoped to be helpful when developing on small devices such as microcontrollers.

(Developers Note, previously this library was intended to be a bare metal framework for microcontroller based applications. Due to some difficulties faced, microcontroller related parts are decided to be removed. These parts will be handled as separate repository.)


## Requirements

- C++20: concepts, `constinit`, `[[no_unique_address]]`, `<=>` and ranges
  (GCC 10 or later, Clang 10 or later with libstdc++)
- Nothing else: the library has no dependencies

No heap, exceptions or RTTI are needed; the library builds with
`-fno-exceptions -fno-rtti` for bare metal targets such as ARM Cortex-M.

Every type can be built at compile time. Declare globals `constinit`: the
compiler then checks that they need no code before `main`, so they are
ready when the first interrupt comes.

```cpp
constinit haluj::ring_buffer<std::array<char, 64>, haluj::spsc> rx;
constinit haluj::digital_input_filter<std::uint32_t, 4>          buttons(0x3u);
```

The interfaces the templates expect (a timer implementation, a parser rule,
a concurrency policy...) are concepts, so a type that does not fit is
reported where it is used, by the name of the concept.

## Using with CMake

```cmake
include(FetchContent)
FetchContent_Declare(haluj GIT_REPOSITORY https://github.com/siyikalender/haluj.git GIT_TAG master)
FetchContent_MakeAvailable(haluj)

target_link_libraries(firmware PRIVATE haluj::haluj)
```

## Examples

`examples/` holds programs that show the library at work; they are built and
run with the tests.

- `parse_text.cpp`: the parser on a comma separated line, a date and a time
  of day, with the values checked and stored only when the whole text is
  valid

## Tests

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

- `tests/test_*.cpp`: run time tests
- `tests/compile_fail/`: code that must not compile, each with the
  diagnostic it must fail with
- Header checks: every header must compile on its own. Headers that do not
  yet are listed in `HALUJ_BROKEN_HEADERS` in `tests/CMakeLists.txt`.
- `tests/target/cortex_m.cpp`: compiled for Cortex-M in CI, which also checks
  that the object needs no heap or exception support.
