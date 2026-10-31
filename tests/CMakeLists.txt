# Tests of the haluj library
#
# This is free and unencumbered software released into the public domain.
# For more information, please refer to <http://unlicense.org> and the
# LICENSE file in the root of this repository.
#
# Author: Claude Opus 5.5 <noreply@anthropic.com>
# Date:   2026

# Run time tests
foreach(test test_constinit test_containers test_format test_parser test_state_machine
             test_timer test_utilities)
  add_executable(haluj_${test} ${test}.cpp)
  target_link_libraries(haluj_${test} PRIVATE haluj::haluj haluj_warnings)
  add_test(NAME ${test} COMMAND haluj_${test})
endforeach()

# ring_buffer with the spsc policy between two threads
find_package(Threads REQUIRED)
add_executable(haluj_test_ring_buffer_spsc test_ring_buffer_spsc.cpp)
target_link_libraries(haluj_test_ring_buffer_spsc PRIVATE haluj::haluj haluj_warnings Threads::Threads)
add_test(NAME test_ring_buffer_spsc COMMAND haluj_test_ring_buffer_spsc)

# blackboard with the spsc policy between two threads
add_executable(haluj_test_blackboard_spsc test_blackboard_spsc.cpp)
target_link_libraries(haluj_test_blackboard_spsc PRIVATE haluj::haluj haluj_warnings Threads::Threads)
add_test(NAME test_blackboard_spsc COMMAND haluj_test_blackboard_spsc)

# Compile failure tests: each source must fail to build with the expected
# diagnostic. The target is excluded from the normal build and built by the
# test itself; matching the message makes sure it fails for the right reason.
function(haluj_compile_fail name expected)
  add_executable(haluj_cf_${name} EXCLUDE_FROM_ALL compile_fail/${name}.cpp)
  target_link_libraries(haluj_cf_${name} PRIVATE haluj::haluj)

  add_test(NAME compile_fail.${name}
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}
                             --target haluj_cf_${name}
                             --config $<CONFIG>)

  set_tests_properties(compile_fail.${name} PROPERTIES
    PASS_REGULAR_EXPRESSION "${expected}"
    # A build that succeeds fails the test even if the expected text shows
    # up somewhere else in the output, for example in the target name
    FAIL_REGULAR_EXPRESSION "Built target|Linking CXX executable"
    # Builds in the same build tree must not run in parallel
    RUN_SERIAL TRUE)
endfunction()

# on_t keeps its strategy by reference: a temporary one must not compile
haluj_compile_fail(on_t_without_strategy "no matching")
haluj_compile_fail(on_temporary_strategy "deleted")

# cyclic_index rejects index types and ring sizes it cannot work with
haluj_compile_fail(cyclic_index_not_integral
  "satisfaction of 'integral|does not satisfy 'integral'")
haluj_compile_fail(cyclic_index_empty_ring   "needs at least one position")

# A backward timer with an unsigned duration would never expire on a step
# larger than the remaining time
haluj_compile_fail(timer_backward_unsigned "needs a signed duration type")

# Concepts: an argument that does not provide the interface is reported by
# the name of the concept it fails. The two patterns are the wording of GCC
# and of Clang.
haluj_compile_fail(parser_not_a_rule
  "parser_rule<Types>\\) \\[with|does not satisfy 'parser_rule'")
haluj_compile_fail(timer_not_an_implementation
  "satisfaction of 'timer_implementation|does not satisfy 'timer_implementation'")
haluj_compile_fail(ring_buffer_not_a_policy
  "satisfaction of 'concurrency_policy|does not satisfy 'concurrency_policy'")
haluj_compile_fail(on_without_test
  "satisfaction of 'event_strategy|constraints not satisfied for class template 'on_t'")
haluj_compile_fail(format_bool
  "satisfaction of 'format_integer|does not satisfy 'format_integer'")

# A filter without samples has nothing to filter
haluj_compile_fail(digital_input_filter_no_samples "needs at least one sample")

# The arguments of a call go to several callbacks, so they are passed as
# lvalues and a callback taking a concrete rvalue reference cannot bind them
haluj_compile_fail(state_machine_rvalue_reference_action
  "cannot bind rvalue reference|expects an rvalue")

# Every public header must compile on its own and survive being included
# twice. All headers together in one translation unit catch clashing include
# guards and names.
set(HALUJ_HEADERS
  bounded/vector
  concurrency
  cyclic_index
  digital_input_filter
  event_strategy
  format
  fragment
  pair_lookup
  parser
  ring_buffer
  set_and_wait
  state_machine
  timer
  timer_implementations/chrono
  timer_implementations/proxy
  timer_implementations/software
  utility
)

# Headers that do not compile on their own yet. Remove an entry once the
# header is fixed; its check then joins the build.
set(HALUJ_BROKEN_HEADERS
)

# Each header in include/haluj must be in exactly one of the two lists, so a
# new header cannot be left out of the check
file(GLOB_RECURSE HALUJ_FOUND_HEADERS RELATIVE ${PROJECT_SOURCE_DIR}/include/haluj
  ${PROJECT_SOURCE_DIR}/include/haluj/*.hpp)

foreach(found ${HALUJ_FOUND_HEADERS})
  string(REGEX REPLACE "\\.hpp$" "" found ${found})
  list(FIND HALUJ_HEADERS        ${found} in_headers)
  list(FIND HALUJ_BROKEN_HEADERS ${found} in_broken)
  if(in_headers EQUAL -1 AND in_broken EQUAL -1)
    message(FATAL_ERROR "haluj/${found}.hpp is in neither HALUJ_HEADERS nor HALUJ_BROKEN_HEADERS")
  endif()
  if(NOT in_headers EQUAL -1 AND NOT in_broken EQUAL -1)
    message(FATAL_ERROR "haluj/${found}.hpp is in both HALUJ_HEADERS and HALUJ_BROKEN_HEADERS")
  endif()
endforeach()

set(HALUJ_HEADER_CHECK_SOURCES "")
set(HALUJ_ALL_HEADERS_SOURCE "")

foreach(header ${HALUJ_HEADERS})
  string(MAKE_C_IDENTIFIER ${header} name)
  set(source ${CMAKE_CURRENT_BINARY_DIR}/header_check/${name}.cpp)
  file(WRITE ${source}.in
    "#include \"haluj/${header}.hpp\"\n#include \"haluj/${header}.hpp\"\n")
  configure_file(${source}.in ${source} COPYONLY)
  list(APPEND HALUJ_HEADER_CHECK_SOURCES ${source})
  string(APPEND HALUJ_ALL_HEADERS_SOURCE "#include \"haluj/${header}.hpp\"\n")
endforeach()

set(source ${CMAKE_CURRENT_BINARY_DIR}/header_check/all_headers.cpp)
file(WRITE ${source}.in "${HALUJ_ALL_HEADERS_SOURCE}")
configure_file(${source}.in ${source} COPYONLY)
list(APPEND HALUJ_HEADER_CHECK_SOURCES ${source})

add_library(haluj_header_check OBJECT ${HALUJ_HEADER_CHECK_SOURCES})
target_link_libraries(haluj_header_check PRIVATE haluj::haluj haluj_warnings)

# A test that hangs (an endless loop is a typical parser bug) fails instead of
# blocking the run
get_property(HALUJ_TESTS DIRECTORY PROPERTY TESTS)
set_tests_properties(${HALUJ_TESTS} PROPERTIES TIMEOUT 60)
