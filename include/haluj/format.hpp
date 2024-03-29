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
  
  if ((first != last))
  {
    if (n < 0) 
    {
      n = -n; // clear sign 
      *first++ = '-';
    }

    precision = 
      std::max
      (
        std::size_t(0U), 
        std::min
        (
          std::size_t(5U), 
          precision
        )
      );
    
    n *= multipliers[precision];
    
    n = std::round(n);
    
    int  i_n = int(n);

    auto it   = format(i_n, first, last); // Leading 0 conversion may simplify the code 
    
    auto d    = std::distance(first, it);
    
    auto nit  = it;
    
    if (precision < d)
    {
      nit++;
      
      while (precision--)
      {
        *it = *(it - 1);
        it--;
      }
      
      *it = '.';

    }
    else
    {
      auto r    =   precision + 2 - d;
      nit +=  r;
      
      while (d--)
      {
        it--;
        *(it + r) = *it;
      }

      *first++ = '0';
      *first++ = '.';
      std::fill(first, it + r, '0');

    }
    
    first = nit;
  }

  return first;
}

} // namespace haluj

#endif  // HALUJ_FORMAT_HPP
