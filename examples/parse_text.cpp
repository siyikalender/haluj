/// \file parse_text.cpp
/// Parser examples: comma separated values, a date and a time of day.
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

/// \author Selcuk Iyikalender, Claude Opus 5.5 <noreply@anthropic.com>
/// \date   2026

// Examples of the parser: a comma separated line, a date and a time of day.
//
// The functions take a pair of iterators, so the text may be in any
// container: a std::string, a bounded::vector, a std::deque, which is not
// even contiguous. Each also has a std::string_view version for text at
// hand.
//
// The rules are built inside the functions because their actions write to
// local variables. The values go to locals first and to the caller's object
// only once the whole text is read and checked, so a failed parse leaves
// the caller's object as it was.

#include <cstdio>
#include <ctime>
#include <deque>
#include <iterator>
#include <string>
#include <string_view>

#include "haluj/bounded/vector.hpp"
#include "haluj/parser.hpp"

namespace example
{

// An action that turns the digits it gets into value. The rules give it at
// most four digits, so the number always fits. It works on any iterator
// and at compile time, which std::from_chars, taking only const char*, does
// not; it is also much smaller on a microcontroller.
constexpr auto store(int& value)
{
  return [&value](auto first, auto last)
  {
    value = 0;
    for (; first != last; ++first)
      value = value * 10 + (*first - '0');
  };
}

/// Calls handler(first, last) for each field of a comma separated line,
/// with iterators of the line. The fields may be empty: "a,,b" has three
/// fields, the second empty.
template<std::forward_iterator Iterator, typename FieldHandler>
constexpr bool parse_comma_separated_values(Iterator first, Iterator last,
                                            FieldHandler handler)
{
  using namespace haluj;

  // until, not except: the last field has no comma after it
  auto field  = action(until(ch(',')), handler);
  auto fields = seq(field, zm(seq(ch(','), field)));

  return fields.accept(first, last) && first == last;
}

/// The same over a string; handler gets const char* iterators
template<typename FieldHandler>
bool parse_comma_separated_values(std::string_view line, FieldHandler handler)
{
  return parse_comma_separated_values(line.data(), line.data() + line.size(), handler);
}

constexpr bool is_leap_year(int year)
{
  return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

constexpr int days_in_month(int year, int month)
{
  constexpr int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  return (month == 2 && is_leap_year(year)) ? 29 : days[month - 1];
}

/// Reads a date such as 2026-11-01 (or 2026-1-5) into the date fields of
/// t. The whole of [first, last) must be the date, and the date must exist;
/// otherwise false is returned and t is not changed.
template<std::forward_iterator Iterator>
constexpr bool parse_date(Iterator first, Iterator last, std::tm& t, char separator = '-')
{
  using namespace haluj;

  int year  = 0;
  int month = 0;
  int day   = 0;

  // Four digits for the year, one or two for the month and the day
  constexpr auto one_or_two = seq(digit(), opt(digit()));

  auto date = seq(action(nt(4, digit()), store(year)),
                  ch(separator),
                  action(one_or_two, store(month)),
                  ch(separator),
                  action(one_or_two, store(day)));

  if (!date.accept(first, last) || first != last)
    return false;

  if (month < 1 || month > 12 || day < 1 || day > days_in_month(year, month))
    return false;

  t.tm_year = year - 1900;    // std::tm counts years from 1900
  t.tm_mon  = month - 1;      // and months from 0
  t.tm_mday = day;
  return true;
}

/// The same over a string. A string literal goes here, not to a range: as
/// an array it would end with its '\0', which is not part of the date.
inline bool parse_date(std::string_view s, std::tm& t, char separator = '-')
{
  return parse_date(s.data(), s.data() + s.size(), t, separator);
}

/// Reads a time of day such as 08:30:00 (or 8:30:0) into the time fields
/// of t. 60 is a valid second, for a leap second. The whole of [first,
/// last) must be the time; otherwise false is returned and t is not
/// changed.
template<std::forward_iterator Iterator>
constexpr bool parse_time(Iterator first, Iterator last, std::tm& t)
{
  using namespace haluj;

  int hour   = 0;
  int minute = 0;
  int second = 0;

  constexpr auto one_or_two = seq(digit(), opt(digit()));

  auto time = seq(action(one_or_two, store(hour)),
                  ch(':'),
                  action(one_or_two, store(minute)),
                  ch(':'),
                  action(one_or_two, store(second)));

  if (!time.accept(first, last) || first != last)
    return false;

  if (hour > 23 || minute > 59 || second > 60)
    return false;

  t.tm_hour = hour;
  t.tm_min  = minute;
  t.tm_sec  = second;
  return true;
}

/// The same over a string
inline bool parse_time(std::string_view s, std::tm& t)
{
  return parse_time(s.data(), s.data() + s.size(), t);
}

} // namespace example

// Runs the examples and checks what they give
namespace
{

int failures = 0;

void expect(bool condition, const char* what)
{
  if (!condition)
  {
    std::printf("unexpected: %s\n", what);
    ++failures;
  }
}

void comma_separated_values()
{
  std::string fields;
  int         count = 0;

  auto collect = [&](const char* first, const char* last)
  {
    fields += '[';
    fields.append(first, last);
    fields += ']';
    ++count;
  };

  expect(example::parse_comma_separated_values("12,abc,,7", collect), "a line");
  std::printf("12,abc,,7  -> %d fields: %s\n", count, fields.c_str());
  expect(count == 4 && fields == "[12][abc][][7]", "the four fields");

  fields.clear();
  count = 0;
  expect(example::parse_comma_separated_values("", collect), "an empty line");
  expect(count == 1 && fields == "[]", "one empty field");
}

void dates()
{
  std::tm t{};

  expect(example::parse_date("2026-11-01", t), "2026-11-01");
  std::printf("2026-11-01 -> year %d, month %d, day %d\n",
              t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);
  expect(t.tm_year == 126 && t.tm_mon == 10 && t.tm_mday == 1, "2026-11-01 fields");

  expect(example::parse_date("2024.2.29", t, '.'), "a leap day, other separator");
  expect(t.tm_mon == 1 && t.tm_mday == 29, "2024.2.29 fields");

  const std::tm before = t;
  expect(!example::parse_date("2025-02-29", t), "no leap day in 2025");
  expect(!example::parse_date("2026-13-01", t), "month 13");
  expect(!example::parse_date("2026-11-01x", t), "text after the date");
  expect(!example::parse_date("26-11-01", t), "a two digit year");
  expect(!example::parse_date("2026-111-01", t), "a three digit month");
  expect(t.tm_year == before.tm_year && t.tm_mon == before.tm_mon &&
         t.tm_mday == before.tm_mday, "a failed parse changes nothing");
}

// The same functions on text in other containers
void containers()
{
  std::tm t{};

  const std::deque<char> in_pieces = {'2', '0', '2', '6', '-', '1', '1', '-', '0', '1'};
  expect(example::parse_date(in_pieces.begin(), in_pieces.end(), t), "a date in a deque");
  expect(t.tm_mon == 10 && t.tm_mday == 1, "the date of the deque");

  haluj::bounded::vector<char, 16> line;
  for (char c : std::string_view("23:05:59"))
    line.push_back(c);
  expect(example::parse_time(line.begin(), line.end(), t), "a time in a bounded::vector");
  expect(t.tm_hour == 23 && t.tm_min == 5 && t.tm_sec == 59, "the time of the vector");

  int  count = 0;
  auto count_fields = [&](auto, auto) { ++count; };
  expect(example::parse_comma_separated_values(in_pieces.begin(), in_pieces.end(), count_fields),
         "a line in a deque");
  expect(count == 1, "one field, there is no comma");
}

// And at compile time: a date checked by the compiler
static_assert([]
{
  std::tm t{};
  const char date[] = "2028-02-29";
  return example::parse_date(date, date + 10, t) && t.tm_mon == 1 && t.tm_mday == 29;
}(), "a leap day at compile time");

static_assert([]
{
  std::tm t{};
  const char date[] = "2027-02-29";
  return !example::parse_date(date, date + 10, t);
}(), "no leap day in 2027");

void times()
{
  std::tm t{};

  expect(example::parse_time("08:30:05", t), "08:30:05");
  std::printf("08:30:05   -> %d h %d min %d s\n", t.tm_hour, t.tm_min, t.tm_sec);
  expect(t.tm_hour == 8 && t.tm_min == 30 && t.tm_sec == 5, "08:30:05 fields");

  expect(example::parse_time("23:59:60", t), "a leap second");
  expect(!example::parse_time("24:00:00", t), "hour 24");
  expect(!example::parse_time("12:60:00", t), "minute 60");
  expect(!example::parse_time("12:00", t), "no seconds");
  expect(!example::parse_time("12:00:00 ", t), "text after the time");
}

} // namespace

int main()
{
  comma_separated_values();
  dates();
  times();
  containers();

  std::printf("%s\n", failures == 0 ? "examples: all as expected" : "examples: FAILED");
  return failures == 0 ? 0 : 1;
}
