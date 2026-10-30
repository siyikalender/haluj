/// \file test_format.cpp
/// Tests of the number formatting functions.
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

#include "haluj/format.hpp"

#include "check.hpp"

namespace
{

// Formats into a buffer that is large enough and returns it as a C string
struct buffer
{
  char text[24];

  template<typename Format>
  const char* operator()(Format f)
  {
    std::memset(text, 0, sizeof(text));
    char* end = f(text, text + sizeof(text) - 1);
    *end = '\0';
    return text;
  }
};

bool equal(const char* a, const char* b)
{
  return std::strcmp(a, b) == 0;
}

} // namespace

static void test_format_int()
{
  buffer b;

  HALUJ_CHECK(equal(b([](char* f, char* l) { return haluj::format(0, f, l); }), "0"));
  HALUJ_CHECK(equal(b([](char* f, char* l) { return haluj::format(42, f, l); }), "42"));
  HALUJ_CHECK(equal(b([](char* f, char* l) { return haluj::format(-7, f, l); }), "-7"));
  HALUJ_CHECK(equal(b([](char* f, char* l) { return haluj::format(2147483647, f, l); }), "2147483647"));
}

static void test_format_hex()
{
  buffer b;

  HALUJ_CHECK(equal(b([](char* f, char* l) { return haluj::format_hex(0xBEEFu, f, l, 4); }), "BEEF"));
  HALUJ_CHECK(equal(b([](char* f, char* l) { return haluj::format_hex(0x0Au, f, l, 2); }), "0A"));
  HALUJ_CHECK(equal(b([](char* f, char* l) { return haluj::format_hex(0x1234u, f, l, 2); }), "34"));
}

static void test_format_float()
{
  buffer b;

  HALUJ_CHECK(equal(b([](char* f, char* l) { return haluj::format(3.14159f, f, l, 2); }), "3.14"));
  HALUJ_CHECK(equal(b([](char* f, char* l) { return haluj::format(2.5f, f, l, 3); }), "2.500"));
  HALUJ_CHECK(equal(b([](char* f, char* l) { return haluj::format(0.05f, f, l, 3); }), "0.050"));
  HALUJ_CHECK(equal(b([](char* f, char* l) { return haluj::format(-1.5f, f, l, 1); }), "-1.5"));
}

// A float that does not fit writes nothing and returns first
static void test_format_float_bounds()
{
  struct guarded
  {
    char text[4];
    char guard[4] = {'#', '#', '#', '#'};
  } g;

  // "0.500" needs 5 characters
  char* end = haluj::format(0.5f, g.text, g.text + 2, 3);
  HALUJ_CHECK(end == g.text);
  HALUJ_CHECK(std::memcmp(g.guard, "####", 4) == 0);

  // "1.50" fits exactly
  end = haluj::format(1.5f, g.text, g.text + 4, 2);
  HALUJ_CHECK(end == g.text + 4);
  HALUJ_CHECK(std::memcmp(g.text, "1.50", 4) == 0);
  HALUJ_CHECK(std::memcmp(g.guard, "####", 4) == 0);

  // "-1.50" is one character too long
  end = haluj::format(-1.5f, g.text, g.text + 4, 2);
  HALUJ_CHECK(end == g.text);
  HALUJ_CHECK(std::memcmp(g.guard, "####", 4) == 0);

  buffer b;
  HALUJ_CHECK(equal(b([](char* f, char* l) { return haluj::format(7.0f, f, l, 0); }), "7."));
  HALUJ_CHECK(equal(b([](char* f, char* l) { return haluj::format(0.0f, f, l, 2); }), "0.00"));
}

int main()
{
  test_format_int();
  test_format_hex();
  test_format_float();
  test_format_float_bounds();
  return haluj_test::report("test_format");
}
