/// \file test_state_machine.cpp
/// Tests of the compile time state machine.
/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
*/

/// \author Claude Opus 5.5 <noreply@anthropic.com>
/// \date   2026

#include <cstring>
#include <tuple>

#include "haluj/state_machine.hpp"

#include "check.hpp"

namespace
{

enum class st { idle, run, stop };

enum event { none, start, pause, halt };

// Records the order of the callbacks
struct trace
{
  char text[64] = {};

  void add(const char* s)
  {
    std::strncat(text, s, sizeof(text) - std::strlen(text) - 1);
  }

  bool is(const char* expected) const
  {
    return std::strcmp(text, expected) == 0;
  }

  void clear()
  {
    text[0] = '\0';
  }
};

trace calls;

} // namespace

int main()
{
  using namespace haluj;

  auto on = [](event e) { return [e](event x) { return x == e; }; };

  auto m = machine(
    graph(
      transition<st::idle, st::run>(on(start), [](event) { calls.add("A "); }),
      transition<st::run,  st::idle>(on(pause)),
      // Order matters: the first transition whose guard holds wins
      transition<st::run,  st::stop>(on(halt)),
      transition<st::run,  st::idle>(on(halt))
    ),
    map(
      entry<st::idle>(std::make_tuple(
        [](event) { calls.add("Ei "); },
        [](event) { calls.add("Di "); },
        [](event) { calls.add("Xi "); })),
      entry<st::run>(std::make_tuple(
        [](event) { calls.add("Er "); },
        nullptr,
        [](event) { calls.add("Xr "); }))
    )
  );

  st s = st::idle;

  // No matching transition: only the do action runs
  s = m(s, none);
  HALUJ_CHECK(s == st::idle);
  HALUJ_CHECK(calls.is("Di "));

  // do, exit of the source, transition action, entry of the target
  calls.clear();
  s = m(s, start);
  HALUJ_CHECK(s == st::run);
  HALUJ_CHECK(calls.is("Di Xi A Er "));

  // nullptr as an empty do action, transition without action
  calls.clear();
  s = m(s, pause);
  HALUJ_CHECK(s == st::idle);
  HALUJ_CHECK(calls.is("Xr Ei "));

  s = m(s, start);
  calls.clear();
  s = m(s, halt);
  HALUJ_CHECK(s == st::stop);
  HALUJ_CHECK(calls.is("Xr "));

  // A state without a map entry and without transitions stays put
  s = m(s, start);
  HALUJ_CHECK(s == st::stop);

  return haluj_test::report("test_state_machine");
}
