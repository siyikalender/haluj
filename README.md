# haluj
## Description

This is a header only lightweight toolbox written in C++. It consists several small utilities, which are hoped to be helpful when developing on small devices such as microcontrollers.

(Developers Note, previously this library was intended to be a bare metal framework for microcontroller based applications. Due to some difficulties faced, microcontroller related parts are decided to be removed. These parts will be handled as separate repository.)


## Requirements

- C++17 (`state_machine.hpp` uses `auto` template parameters)
- Nothing else: the library has no dependencies

No heap, exceptions or RTTI are needed; the library builds with
`-fno-exceptions -fno-rtti` for bare metal targets such as ARM Cortex-M.

## Using with CMake

```cmake
include(FetchContent)
FetchContent_Declare(haluj GIT_REPOSITORY https://github.com/siyikalender/haluj.git GIT_TAG master)
FetchContent_MakeAvailable(haluj)

target_link_libraries(firmware PRIVATE haluj::haluj)
```

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
