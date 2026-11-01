# Examples of the haluj library
#
# This is free and unencumbered software released into the public domain.
# For more information, please refer to <http://unlicense.org> and the
# LICENSE file in the root of this repository.
#
# Author: Claude Opus 5.5 <noreply@anthropic.com>
# Date:   2026

foreach(example parse_text)
  add_executable(haluj_example_${example} ${example}.cpp)
  target_link_libraries(haluj_example_${example} PRIVATE haluj::haluj haluj_warnings)
  add_test(NAME example.${example} COMMAND haluj_example_${example})
endforeach()
