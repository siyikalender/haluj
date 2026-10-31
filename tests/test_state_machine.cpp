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
#include <string>
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

// The arguments of a call reach every guard and action of that call intact,
// even a temporary that a forwarding implementation would move from
static void test_arguments_reach_every_callback()
{
  using namespace haluj;
  enum class door { closed, open };

  static std::string seen;
  seen.clear();

  auto m = machine(
    graph(
      transition<door::closed, door::open>(
        [](std::string e) { seen += "g:" + e + " "; return e == "open"; },
        [](std::string e) { seen += "a:" + e + " "; })),
    map(
      entry<door::closed>(std::make_tuple(
        nullptr,
        [](std::string e) { seen += "d:" + e + " "; },
        [](std::string e) { seen += "x:" + e + " "; })),
      entry<door::open>(std::make_tuple(
        [](std::string e) { seen += "e:" + e + " "; }, nullptr, nullptr))));

  HALUJ_CHECK(m(door::closed, std::string("open")) == door::open);
  HALUJ_CHECK(seen == "d:open g:open x:open a:open e:open ");
}

namespace
{

struct counted
{
  static int copies;

  counted() = default;
  counted(const counted&) { ++copies; }
  counted(counted&&)      { ++copies; }
};

int counted::copies = 0;

} // namespace

// Guards and actions that take their argument by reference see the caller's
// object: no copy is made on the way
static void test_arguments_not_copied()
{
  using namespace haluj;
  enum class s { a, b };

  auto never = [](const counted&) { return false; };
  auto once  = [](const counted&) { return true; };

  auto m = machine(
    graph(
      transition<s::a, s::b>(never),
      transition<s::a, s::b>(never),
      transition<s::a, s::b>(once, [](const counted&) {})),
    map(
      entry<s::a>(std::make_tuple(nullptr, [](const counted&) {}, [](const counted&) {})),
      entry<s::b>(std::make_tuple([](const counted&) {}, nullptr, nullptr))));

  counted event;
  counted::copies = 0;

  HALUJ_CHECK(m(s::a, event) == s::b);
  HALUJ_CHECK(counted::copies == 0);

  HALUJ_CHECK(m(s::a, counted()) == s::b);
  HALUJ_CHECK(counted::copies == 0);
}

// An action may take a context by reference and change it, and generic
// callbacks taking auto&& work too
static void test_context_by_reference()
{
  using namespace haluj;
  enum class s { a, b };

  struct context
  {
    int entered = 0;
    int left    = 0;
  };

  auto m = machine(
    graph(
      transition<s::a, s::b>([](auto&& c) { return c.entered == 0; }),
      transition<s::b, s::a>(g_always)),
    map(
      entry<s::a>(std::make_tuple(nullptr, nullptr, [](context& c) { ++c.left; })),
      entry<s::b>(std::make_tuple([](context& c) { ++c.entered; }, nullptr, nullptr))));

  context c;
  HALUJ_CHECK(m(s::a, c) == s::b);
  HALUJ_CHECK(c.entered == 1 && c.left == 1);
  HALUJ_CHECK(m(s::b, c) == s::a);
  HALUJ_CHECK(m(s::a, c) == s::a); // the guard now refuses
}

// A machine of captureless lambdas is a constant: built and run at compile
// time, it can sit in read only memory
namespace
{

enum class light { red, green, yellow };

constexpr auto traffic = haluj::machine(
  haluj::graph(
    haluj::transition<light::red,    light::green >([](int tick) { return tick >= 3; }),
    haluj::transition<light::green,  light::yellow>([](int tick) { return tick >= 2; }),
    haluj::transition<light::yellow, light::red   >(haluj::g_always)),
  haluj::map(
    haluj::entry<light::red>(std::make_tuple(nullptr, nullptr, nullptr))));

static_assert(traffic(light::red, 0)    == light::red,    "guard refuses");
static_assert(traffic(light::red, 3)    == light::green,  "guard holds");
static_assert(traffic(light::green, 2)  == light::yellow, "next transition");
static_assert(traffic(light::yellow, 0) == light::red,    "always");

} // namespace

static void test_constexpr_machine()
{
  // The same machine at run time
  light l = light::red;
  for (int tick = 0; tick < 4; ++tick)
    l = traffic(l, tick);
  HALUJ_CHECK(l == light::green);
}

// The first transition whose guard holds wins, and a transition to the same
// state runs exit and entry
static void test_order_and_self_transition()
{
  using namespace haluj;
  enum class s { a, b };

  static int order = 0;
  static int exits = 0;
  static int entries = 0;
  order = exits = entries = 0;

  auto m = machine(
    graph(
      transition<s::a, s::a>([](int e) { return e == 1; }, [](int) { order = 1; }),
      transition<s::a, s::b>([](int e) { return e >= 1; }, [](int) { order = 2; })),
    map(
      entry<s::a>(std::make_tuple([](int) { ++entries; }, nullptr, [](int) { ++exits; }))));

  HALUJ_CHECK(m(s::a, 1) == s::a);
  HALUJ_CHECK(order == 1);
  HALUJ_CHECK(exits == 1 && entries == 1);

  HALUJ_CHECK(m(s::a, 2) == s::b);
  HALUJ_CHECK(order == 2);
}

int main()
{
  test_constexpr_machine();
  test_order_and_self_transition();
  test_arguments_reach_every_callback();
  test_arguments_not_copied();
  test_context_by_reference();

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
