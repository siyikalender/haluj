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

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>
#include <type_traits>

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

namespace
{

// Formats value into buffers of every length from 0 to one more than it
// needs, with guard bytes after each: the text must be written whole and
// right, or not at all, and nothing past the buffer may change
template<typename Integer>
void check_integer(Integer value)
{
  char expected[32];
  if (std::is_signed<Integer>::value)
    std::snprintf(expected, sizeof(expected), "%lld", static_cast<long long>(value));
  else
    std::snprintf(expected, sizeof(expected), "%llu", static_cast<unsigned long long>(value));

  const std::size_t length = std::strlen(expected);

  for (std::size_t room = 0; room <= length + 1; ++room)
  {
    char buffer[40];
    std::memset(buffer, '#', sizeof(buffer));

    char* end = haluj::format(value, buffer, buffer + room);

    if (room < length)
    {
      HALUJ_CHECK(end == buffer);
    }
    else
    {
      HALUJ_CHECK(end == buffer + length);
      HALUJ_CHECK(std::memcmp(buffer, expected, length) == 0);
    }

    for (std::size_t k = (room < length ? 0 : length); k < sizeof(buffer); ++k)
      HALUJ_CHECK(buffer[k] == '#');
  }
}

template<typename Integer>
void check_integer_type()
{
  using limits = std::numeric_limits<Integer>;

  check_integer<Integer>(limits::min());
  check_integer<Integer>(Integer(limits::min() + 1));
  check_integer<Integer>(limits::max());
  check_integer<Integer>(Integer(limits::max() - 1));
  check_integer<Integer>(Integer(0));
  check_integer<Integer>(Integer(1));
  check_integer<Integer>(Integer(9));
  check_integer<Integer>(Integer(10));
  check_integer<Integer>(Integer(99));
  check_integer<Integer>(Integer(100));
  if (std::is_signed<Integer>::value)
  {
    check_integer<Integer>(Integer(-1));
    check_integer<Integer>(Integer(-10));
  }
}

} // namespace

// Every integer type, at its limits and where the number of digits changes
static void test_format_integers()
{
  check_integer_type<signed char>();
  check_integer_type<unsigned char>();
  check_integer_type<std::int16_t>();
  check_integer_type<std::uint16_t>();
  check_integer_type<std::int32_t>();
  check_integer_type<std::uint32_t>();
  check_integer_type<std::int64_t>();
  check_integer_type<std::uint64_t>();
  check_integer_type<long>();
  check_integer_type<unsigned long long>();

  // The cases from the review
  buffer b;
  HALUJ_CHECK(equal(b([](char* f, char*) { return haluj::format(12345, f, f + 3); }), ""));
  HALUJ_CHECK(equal(b([](char* f, char*) { return haluj::format(-5, f, f + 1); }), ""));
  HALUJ_CHECK(equal(b([](char* f, char* l) { return haluj::format(3000000000u, f, l); }), "3000000000"));
}

static void test_format_hex()
{
  buffer b;

  HALUJ_CHECK(equal(b([](char* f, char* l) { return haluj::format_hex(0xBEEFu, f, l, 4); }), "BEEF"));
  HALUJ_CHECK(equal(b([](char* f, char* l) { return haluj::format_hex(0x0Au, f, l, 2); }), "0A"));
  HALUJ_CHECK(equal(b([](char* f, char* l) { return haluj::format_hex(0x1234u, f, l, 2); }), "34"));
}

namespace
{

// format_hex of value with every width up to 20 digits, into buffers of
// every length around it, against snprintf
template<typename Integer>
void check_hex(Integer value)
{
  typedef std::make_unsigned_t<Integer> value_type;

  char full[40];
  std::snprintf(full, sizeof(full), "0000000000000000%016llX",
                static_cast<unsigned long long>(static_cast<value_type>(value)));
  const std::size_t full_length = std::strlen(full);

  for (std::size_t size = 0; size <= 20; ++size)
  {
    const char* expected = full + full_length - size;

    for (std::size_t room = (size > 0 ? size - 1 : 0); room <= size + 1; ++room)
    {
      char buffer[40];
      std::memset(buffer, '#', sizeof(buffer));

      char* end = haluj::format_hex(value, buffer, buffer + room, size);

      if (room < size)
      {
        HALUJ_CHECK(end == buffer);
      }
      else
      {
        HALUJ_CHECK(end == buffer + size);
        HALUJ_CHECK(std::memcmp(buffer, expected, size) == 0);
      }

      for (std::size_t k = (room < size ? 0 : size); k < sizeof(buffer); ++k)
        HALUJ_CHECK(buffer[k] == '#');
    }
  }
}

} // namespace

static void test_format_hex_types()
{
  check_hex<std::uint8_t>(0xA5u);
  check_hex<std::uint16_t>(0xBEEFu);
  check_hex<std::uint32_t>(0xDEADBEEFu);
  check_hex<std::uint64_t>(0x0123456789ABCDEFull);
  check_hex<std::uint64_t>(std::numeric_limits<std::uint64_t>::max());
  check_hex<int>(0x0A);
  check_hex<int>(-1);                 // two's complement: FFFFFFFF
  check_hex<std::int8_t>(-2);         // FE

  // The case from the review: a width of 4 into 2 characters
  buffer b;
  HALUJ_CHECK(equal(b([](char* f, char*) { return haluj::format_hex(0x1234u, f, f + 2, 4); }), ""));
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

// Values whose scaled form does not fit in an int, NaN and infinity write
// nothing; a small negative value that rounds to zero keeps its sign
static void test_format_float_range()
{
  const float values[] =
  {
    std::numeric_limits<float>::quiet_NaN(),
    std::numeric_limits<float>::infinity(),
    -std::numeric_limits<float>::infinity(),
    1e10f,
    -1e10f,
    3e7f,       // 3e9 with precision 2
  };

  for (float v : values)
  {
    char buffer[40];
    std::memset(buffer, '#', sizeof(buffer));
    HALUJ_CHECK(haluj::format(v, buffer, buffer + sizeof(buffer), 2) == buffer);
    HALUJ_CHECK(buffer[0] == '#');
  }

  buffer b;

  // Large values that still fit
  HALUJ_CHECK(equal(b([](char* f, char* l) { return haluj::format(2e7f, f, l, 2); }), "20000000.00"));
  HALUJ_CHECK(equal(b([](char* f, char* l) { return haluj::format(-2e7f, f, l, 2); }), "-20000000.00"));

  // The sign of a small negative value is kept on purpose
  HALUJ_CHECK(equal(b([](char* f, char* l) { return haluj::format(-0.004f, f, l, 2); }), "-0.00"));
  HALUJ_CHECK(equal(b([](char* f, char* l) { return haluj::format(-0.0f, f, l, 2); }), "0.00"));
}

// The integer formats run at compile time too: a table of texts can be built
// by the compiler
namespace
{

template<std::size_t N>
struct text
{
  char        chars[N] = {};
  std::size_t length   = 0;

  constexpr bool is(const char* expected) const
  {
    std::size_t i = 0;
    for (; expected[i] != '\0'; ++i)
      if (i >= length || chars[i] != expected[i])
        return false;
    return i == length;
  }
};

template<typename Integer>
constexpr text<24> decimal(Integer n)
{
  text<24> t;
  t.length = std::size_t(haluj::format(n, t.chars, t.chars + 24) - t.chars);
  return t;
}

template<typename Integer>
constexpr text<24> hex(Integer n, std::size_t size)
{
  text<24> t;
  t.length = std::size_t(haluj::format_hex(n, t.chars, t.chars + 24, size) - t.chars);
  return t;
}

static_assert(decimal(0).is("0"), "zero");
static_assert(decimal(-1250).is("-1250"), "negative");
static_assert(decimal(std::numeric_limits<std::int64_t>::min()).is("-9223372036854775808"),
              "most negative");
static_assert(decimal(std::uint8_t(255)).is("255"), "uint8_t");
static_assert(hex(0xBEEFu, 4).is("BEEF"), "hex");
static_assert(hex(std::int8_t(-1), 4).is("00FF"), "hex of a negative value");

} // namespace

int main()
{
  test_format_float_range();
  test_format_int();
  test_format_integers();
  test_format_hex();
  test_format_hex_types();
  test_format_float();
  test_format_float_bounds();
  return haluj_test::report("test_format");
}
