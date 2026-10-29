/// \file test_parser.cpp
/// Tests of the parser combinators.
/// build mode, so the tests also run in release (NDEBUG) builds.
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

#include "haluj/parser.hpp"

#include "check.hpp"

namespace
{

// Runs a rule over a C string and tells how many characters it consumed
template<typename Rule>
bool parse(const Rule& rule, const char* text, std::size_t& consumed)
{
  const char* first = text;
  const char* last  = text + std::strlen(text);
  bool        ok    = rule.accept(first, last);
  consumed = static_cast<std::size_t>(first - text);
  return ok;
}

} // namespace

static void test_primitives()
{
  using namespace haluj;
  std::size_t n = 0;

  HALUJ_CHECK(parse(ch('a'), "abc", n) && n == 1);
  HALUJ_CHECK(!parse(ch('a'), "xbc", n) && n == 0);
  HALUJ_CHECK(!parse(ch('a'), "", n) && n == 0);
  HALUJ_CHECK(parse(digit(), "7", n) && n == 1);
  HALUJ_CHECK(parse(hex_digit(), "f", n) && n == 1);
  HALUJ_CHECK(!parse(oct_digit(), "8", n));
  HALUJ_CHECK(!parse(bin_digit(), "2", n));
  HALUJ_CHECK(parse(upper(), "Q", n) && !parse(upper(), "q", n));
  HALUJ_CHECK(parse(rest(), "anything", n) && n == 8);
}

static void test_combinators()
{
  using namespace haluj;
  std::size_t n = 0;

  HALUJ_CHECK(parse(om(digit()), "123x", n) && n == 3);
  HALUJ_CHECK(!parse(om(digit()), "x", n) && n == 0);
  HALUJ_CHECK(parse(zm(digit()), "x", n) && n == 0);
  HALUJ_CHECK(parse(opt(ch('-')), "5", n) && n == 0);
  HALUJ_CHECK(parse(any(ch('a'), ch('b')), "b", n) && n == 1);
  HALUJ_CHECK(!parse(any(ch('a'), ch('b')), "c", n));

  // A failed sequence gives back what it consumed
  HALUJ_CHECK(!parse(seq(ch('a'), ch('b')), "ac", n) && n == 0);
  HALUJ_CHECK(parse(seq(ch('a'), ch('b')), "abc", n) && n == 2);

  // except consumes up to, but not including, the terminator
  HALUJ_CHECK(parse(except(ch(';')), "abc;def", n) && n == 3);
}

static void test_command_line()
{
  using namespace haluj;

  // SET <number>, the kind of command a serial console receives
  int value = 0;
  auto number = action(om(digit()), [&value](const char* first, const char* last)
  {
    value = 0;
    for (; first != last; ++first)
      value = value * 10 + (*first - '0');
  });

  auto command = seq(ch('S'), ch('E'), ch('T'), om(space()), number, zm(space()));

  std::size_t n = 0;
  HALUJ_CHECK(parse(command, "SET  1250 ", n));
  HALUJ_CHECK(n == 10);
  HALUJ_CHECK(value == 1250);

  value = -1;
  HALUJ_CHECK(!parse(command, "SET x", n));
  HALUJ_CHECK(n == 0);
  HALUJ_CHECK(value == -1);
}

int main()
{
  test_primitives();
  test_combinators();
  test_command_line();
  return haluj_test::report("test_parser");
}
