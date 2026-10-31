/// \file format.hpp
/// Text formatting
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
/// \author Selcuk Iyikalender
/// \date   2018

#ifndef HALUJ_FORMAT_HPP
#define HALUJ_FORMAT_HPP

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <limits>
#include <type_traits>

namespace haluj
{
  
namespace format_detail
{

template<typename T>
using enable_if_integer_t =
  std::enable_if_t<std::is_integral<T>::value && !std::is_same<T, bool>::value>;

/// The number of characters from first to last
template<typename Iterator>
inline std::size_t room(Iterator first, Iterator last)
{
  return static_cast<std::size_t>(std::distance(first, last));
}

} // namespace format_detail

/// Writes n in decimal, with a leading '-' when it is negative, and returns
/// the end of the text. Works for every integer type, signed or unsigned, of
/// any width.
///
/// Nothing is written when the text does not fit in [first, last); first is
/// returned then.
template<typename Integer,
         typename Iterator,
         typename = format_detail::enable_if_integer_t<Integer> >
Iterator format(Integer n, Iterator first, Iterator last)
{
  typedef std::make_unsigned_t<Integer> magnitude_type;

  bool           negative  = false;
  magnitude_type magnitude = static_cast<magnitude_type>(n);

  if constexpr (std::is_signed<Integer>::value)
  {
    if (n < Integer(0))
    {
      // Negated in the unsigned type, where it cannot overflow: this gives
      // the magnitude of the most negative value too
      negative  = true;
      magnitude = static_cast<magnitude_type>(magnitude_type(0) - magnitude);
    }
  }

  // Digits from the least significant end; 20 digits hold any 64 bit value
  char  digits[std::numeric_limits<magnitude_type>::digits10 + 1];
  char* const digits_end = digits + sizeof(digits);
  char* digit            = digits_end;

  do
  {
    *--digit = static_cast<char>('0' + (magnitude % 10U));
    magnitude = static_cast<magnitude_type>(magnitude / 10U);
  }
  while (magnitude != 0U);

  const std::size_t length =
    static_cast<std::size_t>(digits_end - digit) + (negative ? 1U : 0U);

  if (format_detail::room(first, last) < length)
  {
    return first;
  }

  if (negative)
  {
    *first++ = '-';
  }

  return std::copy(digit, digits_end, first);
}

/// Writes the size lowest hexadecimal digits of n, most significant first and
/// with leading zeros, in upper case: 0xBEEF with size 4 gives "BEEF", 0x0A
/// with size 2 gives "0A" and 0x1234 with size 2 gives "34". Returns the end
/// of the text. Works for every integer type; a negative value is written as
/// its unsigned equivalent (two's complement).
///
/// Nothing is written when size characters do not fit in [first, last);
/// first is returned then.
template<typename Integer,
         typename Iterator,
         typename = format_detail::enable_if_integer_t<Integer> >
Iterator format_hex(Integer n, Iterator first, Iterator last, std::size_t size)
{
  typedef std::make_unsigned_t<Integer> value_type;

  const value_type  value = static_cast<value_type>(n);
  const std::size_t bits  = std::numeric_limits<value_type>::digits;

  if (format_detail::room(first, last) < size)
  {
    return first;
  }

  for (std::size_t i = size; i > 0U; --i)
  {
    // Digits beyond the width of the type are leading zeros; shifting by
    // the width or more would be undefined
    const std::size_t shift = 4U * (i - 1U);
    const unsigned    d     = (shift < bits) ? unsigned((value >> shift) & 0xFU) : 0U;

    *first++ = static_cast<char>(d < 10U ? ('0' + d) : ('A' + (d - 10U)));
  }

  return first;
}

/// Formats n with a fixed number of decimals (at most 5), for example
/// 3.14159f with precision 2 gives "3.14" and 0.05f with precision 3 gives
/// "0.050". Returns the end of the written text.
///
/// Nothing is written when the text does not fit in [first, last); first is
/// returned then. n times 10^precision must fit in an int.
template<typename Iterator>
Iterator format(float n, Iterator first, Iterator last, std::size_t precision)
{
  static const float multipliers[] =
  {
    1,
    10,
    100,
    1000,
    10000,
    100000
  };

  const bool negative = (n < 0);

  if (negative)
  {
    n = -n; // clear sign
  }

  precision = std::min(std::size_t(5U), precision);

  n *= multipliers[precision];

  n = std::round(n);

  // Digits of the scaled value, without any decimal point
  char        digits[16];
  char* const digits_end = format(int(n), digits, digits + sizeof(digits));
  const auto  d = static_cast<std::size_t>(digits_end - digits);

  // A value below 1 gets a leading "0" and zeros after the point
  const std::size_t integer_digits = (d > precision) ? (d - precision) : 0U;
  const std::size_t leading_zeros  = (d > precision) ? 0U : (precision - d);

  const std::size_t length =
    (negative ? 1U : 0U) +
    (integer_digits > 0U ? integer_digits : 1U) +
    1U +            // decimal point
    precision;

  if (static_cast<std::size_t>(std::distance(first, last)) < length)
  {
    return first;
  }

  const char* digit = digits;

  if (negative)
  {
    *first++ = '-';
  }

  if (integer_digits == 0U)
  {
    *first++ = '0';
  }

  for (std::size_t i = 0; i < integer_digits; ++i)
  {
    *first++ = *digit++;
  }

  *first++ = '.';

  for (std::size_t i = 0; i < leading_zeros; ++i)
  {
    *first++ = '0';
  }

  while (digit != digits_end)
  {
    *first++ = *digit++;
  }

  return first;
}

} // namespace haluj

#endif  // HALUJ_FORMAT_HPP
