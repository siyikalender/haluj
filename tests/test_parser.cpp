/// \file test_parser.cpp
/// Tests of the parser combinators.
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

#include <cctype>
#include <cstdint>
#include <cstring>
#include <string>

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

// The character rules match exactly the "C" locale classes of <cctype> for
// every byte, and reject the bytes above 127, whatever the sign of char
static void test_character_classes()
{
  using namespace haluj;

  for (int byte = 0; byte < 256; ++byte)
  {
    const char  text[2] = {static_cast<char>(byte), '\0'};
    const char* first   = text;
    const char* last    = text + 1;

    auto matches = [&](const auto& rule)
    {
      const char* f = first;
      return rule.accept(f, last);
    };

    // <cctype> is defined here, as byte is an unsigned char value
    HALUJ_CHECK(matches(alpha())     == (std::isalpha(byte)  != 0));
    HALUJ_CHECK(matches(alnum())     == (std::isalnum(byte)  != 0));
    HALUJ_CHECK(matches(space())     == (std::isspace(byte)  != 0));
    HALUJ_CHECK(matches(digit())     == (std::isdigit(byte)  != 0));
    HALUJ_CHECK(matches(hex_digit()) == (std::isxdigit(byte) != 0));
    HALUJ_CHECK(matches(upper())     == (std::isupper(byte)  != 0));
  }
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

  // except consumes up to and including the terminator, and fails without
  // consuming anything when there is none
  HALUJ_CHECK(parse(except(ch(';')), "abc;def", n) && n == 4);
  HALUJ_CHECK(!parse(except(ch(';')), "abc", n) && n == 0);
}

// except: the text up to and including a terminator, with an optional escape
static void test_except()
{
  using namespace haluj;
  std::size_t n = 0;

  // C style: \" and \\ are pairs, any other \x is two ordinary characters
  constexpr auto c_text = except(ch('"'), ch('\\'));
  HALUJ_CHECK(parse(c_text, "ab\"x", n) && n == 3);
  HALUJ_CHECK(parse(c_text, "a\\\"b\"x", n) && n == 5);      // a\"b"
  HALUJ_CHECK(parse(c_text, "a\\\\\"x", n) && n == 4);       // a\\"  escaped backslash
  HALUJ_CHECK(parse(c_text, "a\\nb\"x", n) && n == 5);       // a\nb"
  HALUJ_CHECK(parse(c_text, "\"", n) && n == 1);             // empty text
  HALUJ_CHECK(!parse(c_text, "a\\\"b", n) && n == 0);        // only an escaped quote
  HALUJ_CHECK(!parse(c_text, "ab\\", n) && n == 0);          // escape at the end
  HALUJ_CHECK(!parse(c_text, "", n) && n == 0);

  // SQL style: the escape is the terminator itself, doubled
  constexpr auto sql_text = except(ch('\''), ch('\''));
  HALUJ_CHECK(parse(sql_text, "it''s'x", n) && n == 6);
  HALUJ_CHECK(parse(sql_text, "a'x", n) && n == 2);
  HALUJ_CHECK(parse(sql_text, "'", n) && n == 1);
  HALUJ_CHECK(parse(sql_text, "'''x", n) && n == 3);         // '''' : one quote
  HALUJ_CHECK(!parse(sql_text, "''", n) && n == 0);

  // A terminator of several characters
  constexpr auto comment = except(seq(ch('*'), ch('/')));
  HALUJ_CHECK(parse(comment, "a * b */x", n) && n == 8);
  HALUJ_CHECK(!parse(comment, "a * b *", n) && n == 0);

  // An escape that matches empty input does not loop forever
  HALUJ_CHECK(parse(except(ch(';'), opt(ch('\\'))), "ab;", n) && n == 3);
  HALUJ_CHECK(!parse(except(ch(';'), zm(space())), "ab", n) && n == 0);

  // until: up to the terminator or the end, without the terminator
  HALUJ_CHECK(parse(until(ch(',')), "ab,cd", n) && n == 2);
  HALUJ_CHECK(parse(until(ch(',')), "abcd", n) && n == 4);
  HALUJ_CHECK(parse(until(ch(',')), ",cd", n) && n == 0);
  HALUJ_CHECK(parse(until(ch(',')), "", n) && n == 0);
  HALUJ_CHECK(parse(until(seq(ch('\r'), ch('\n'))), "a\rb\r\n", n) && n == 3);
}


// Repeating a rule that can succeed without consuming input stops instead of
// looping forever
static void test_repeat_empty_match()
{
  using namespace haluj;
  std::size_t n = 0;

  HALUJ_CHECK(parse(zm(opt(ch('x'))), "abc", n) && n == 0);
  HALUJ_CHECK(parse(om(opt(ch('x'))), "abc", n) && n == 0);
  HALUJ_CHECK(parse(zm(zm(digit())), "12a", n) && n == 2);
  HALUJ_CHECK(parse(om(opt(ch('a'))), "aab", n) && n == 2);
  HALUJ_CHECK(parse(zm(rest()), "abc", n) && n == 3);
  HALUJ_CHECK(parse(seq(zm(opt(space())), ch('k')), "  k", n) && n == 3);
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

// Rules are constants: built and run at compile time, they can sit in read
// only memory
namespace
{

// Characters a rule accepts at the start of text, or -1 if it fails
template<typename Rule>
constexpr int consumed(const Rule& rule, const char* text)
{
  const char* first = text;
  const char* last  = text;
  while (*last != '\0')
    ++last;
  return rule.accept(first, last) ? int(first - text) : -1;
}

constexpr auto number  = haluj::seq(haluj::opt(haluj::ch('-')), haluj::om(haluj::digit()));
constexpr auto command = haluj::seq(haluj::ch('S'), haluj::ch('E'), haluj::ch('T'),
                                    haluj::om(haluj::space()), number);
constexpr auto word    = haluj::action(haluj::om(haluj::alpha()),
                                       [](const char*, const char*) {});
constexpr auto quoted  = haluj::seq(haluj::ch('"'),
                                    haluj::except(haluj::ch('"'), haluj::ch('\\')));

static_assert(consumed(number, "-123x") == 4,  "sign and digits");
static_assert(consumed(number, "x")     == -1, "no digit");
static_assert(consumed(command, "SET  42") == 7, "command");
static_assert(consumed(command, "SEX 1")   == -1, "wrong command");
static_assert(consumed(haluj::any(haluj::ch('a'), haluj::ch('b')), "b") == 1, "any");
static_assert(consumed(haluj::zm(haluj::opt(haluj::ch('x'))), "abc") == 0, "no endless loop");
static_assert(consumed(haluj::except(haluj::ch(';')), "ab;c") == 3, "except");
static_assert(consumed(haluj::until(haluj::ch(',')), "ab,c") == 2, "until");
static_assert(consumed(word, "abc1") == 3, "action");
static_assert(consumed(haluj::hex_digit(), "F") == 1, "hex digit");
static_assert(consumed(haluj::rest(), "rest") == 4, "rest");
static_assert(consumed(quoted, "\"a\\\"b\" tail") == 6, "quoted with escape");

// Empty rules take no room inside the rules that hold them
// ([[no_unique_address]]): only the characters of the four ch() are stored
static_assert(sizeof(command) == 4, "S, E, T and the sign: 4 bytes");
static_assert(sizeof(haluj::om(haluj::digit())) == 1, "an empty rule takes one byte");

} // namespace

// Recursive rules
namespace
{

using namespace haluj;

// expr   := term   (('+' | '-') term)*
// term   := factor (('*' | '/') factor)*
// factor := digit+ | '(' expr ')'
struct expr : recursive<expr>
{
  static constexpr auto definition();
};

constexpr auto factor = any(om(digit()), seq(ch('('), expr{}, ch(')')));
constexpr auto term   = seq(factor, zm(seq(any(ch('*'), ch('/')), factor)));

constexpr auto expr::definition()
{
  return seq(term, zm(seq(any(ch('+'), ch('-')), term)));
}

// parens := ('(' parens ')')*   balanced parentheses, possibly none
struct parens : recursive<parens>
{
  static constexpr auto definition()
  {
    return zm(seq(ch('('), parens{}, ch(')')));
  }
};

static_assert(sizeof(expr) == 1, "a recursive rule is an empty type");

static_assert(consumed(expr{}, "42") == 2,                "number");
static_assert(consumed(expr{}, "1+2*(3-4)/5") == 11,      "nested");
static_assert(consumed(expr{}, "((((7))))") == 9,         "deep");
static_assert(consumed(expr{}, "(1+2") == -1,             "unclosed");
static_assert(consumed(expr{}, "2*(3") == 1,              "stops before an unclosed group");
static_assert(consumed(expr{}, "+1") == -1,               "no operand");

static_assert(consumed(parens{}, "(()(()))x") == 8,       "balanced");
static_assert(consumed(parens{}, "(()") == 0,             "unbalanced matches nothing");
static_assert(consumed(parens{}, ")") == 0,               "empty match");

} // namespace

// nt and vt: a rule a fixed or a given number of times, for binary formats
static void test_counted()
{
  using namespace haluj;
  std::size_t n = 0;

  HALUJ_CHECK(parse(nt(3, digit()), "1234", n) && n == 3);
  HALUJ_CHECK(!parse(nt(3, digit()), "12x", n) && n == 0);   // gives back what it read
  HALUJ_CHECK(parse(nt(0, digit()), "x", n) && n == 0);
  HALUJ_CHECK(parse(nt(-2, digit()), "x", n) && n == 0);
  HALUJ_CHECK(parse(seq(ch('#'), nt(2, hex_digit())), "#1F", n) && n == 3);
  HALUJ_CHECK(!parse(seq(ch('#'), nt(2, hex_digit())), "#1", n) && n == 0);

  // A length prefixed packet of bytes: the first byte is the length
  std::uint8_t length = 0;
  std::size_t  payload = 0;
  auto packet = seq(action(anychar(), [&](auto p, auto) { length = std::uint8_t(*p); }),
                    action(vt(length, anychar()),
                           [&](auto first, auto last) { payload = std::size_t(last - first); }));

  const std::uint8_t data[] = {3, 0xAA, 0x00, 0xFF, 0x55};
  const std::uint8_t* first = data;
  HALUJ_CHECK(packet.accept(first, data + sizeof(data)));
  HALUJ_CHECK(first == data + 4 && length == 3 && payload == 3);

  const std::uint8_t short_data[] = {5, 1, 2};
  first = short_data;
  HALUJ_CHECK(!packet.accept(first, short_data + sizeof(short_data)));
  HALUJ_CHECK(first == short_data);

  // The count is read when the rule runs, not when it is built
  int count = 1;
  const auto letters = vt(count, alpha());
  HALUJ_CHECK(parse(letters, "abc", n) && n == 1);
  count = 3;
  HALUJ_CHECK(parse(letters, "abc", n) && n == 3);
}

// At compile time: a fixed field in a constant rule, and a length prefix
static_assert(consumed(haluj::nt(4, haluj::anychar()), "abcdef") == 4, "nt");
static_assert(consumed(haluj::nt(4, haluj::anychar()), "abc") == -1, "nt short");
static_assert([]
{
  using namespace haluj;
  int  length = 0;
  auto field  = seq(action(digit(), [&](const char* p, const char*) { length = *p - '0'; }),
                    vt(length, anychar()));
  return consumed(field, "3abcd") == 4;
}(), "vt");

// An empty rule repeated takes the room of its count only
static_assert(sizeof(haluj::nt(std::uint8_t(4), haluj::anychar())) == 1, "nt is its count");

// The same rules at run time, on std::string iterators and on input nested
// far deeper than a compile time evaluation would allow
static void test_recursive_rules()
{
  const std::string sum = "10*(2+3)-(4/(1+1))";
  auto first = sum.begin();
  HALUJ_CHECK(expr{}.accept(first, sum.end()));
  HALUJ_CHECK(first == sum.end());

  std::string deep(500, '(');
  deep += '1';
  deep += std::string(500, ')');
  first = deep.cbegin();
  HALUJ_CHECK(expr{}.accept(first, deep.cend()));
  HALUJ_CHECK(first == deep.cend());

  deep.pop_back();                         // one ')' missing
  first = deep.cbegin();
  HALUJ_CHECK(!expr{}.accept(first, deep.cend()));
  HALUJ_CHECK(first == deep.cbegin());     // a failed seq gives back its input
}

// The constant rules work at run time too, on any iterator
static void test_constexpr_rules()
{
  HALUJ_CHECK(consumed(command, "SET 7") == 5);

  const std::string text = "\"x\\\"y\"";   // "x\"y"
  auto first = text.begin();
  HALUJ_CHECK(quoted.accept(first, text.end()));
  HALUJ_CHECK(first == text.end());
}

int main()
{
  test_constexpr_rules();
  test_recursive_rules();
  test_primitives();
  test_character_classes();
  test_combinators();
  test_except();
  test_counted();
  test_repeat_empty_match();
  test_command_line();
  return haluj_test::report("test_parser");
}
