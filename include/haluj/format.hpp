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

#include <cstdint>
#include <cmath>
#include <algorithm>
#include <iterator>

namespace haluj
{
  
template<typename Iterator>
Iterator format_hex(unsigned n, Iterator first, Iterator last, std::size_t size)
{
  Iterator    initial = first;
  
  for (std::size_t i = 0; (i < size) && (first != last); i++)
  {
    unsigned d = n & 0xF;
    if (d < 10)
      *first++ = ('0' + d);
    else
      *first++ = ('A' + (d - 10));
    n >>= 4;
  }
  
  std::reverse(initial, first);  

  return first; // check for premature end
}

template<typename Iterator>
Iterator format(int n, Iterator first, Iterator last)
{
  if (n < 0 && (first != last)) 
  {
    n = -n;
    *first++ = '-';
  }
  
  Iterator initial = first;
  
  if (n > 0)
  {
    while((n > 0) && (first != last))
    {
      int d = n % 10;
      *first++ = ('0' + d);
      n /= 10;
    }
  }
  else
  {
    *first++ = '0';
  }
  
  std::reverse(initial, first);
  
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
