/// \file fragment.hpp
/// Fragments a large sized block to smaller chunks. 
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

#ifndef FRAGMENT_HPP
#define FRAGMENT_HPP

#include <cstdint>
#include <utility>

namespace haluj
{

namespace base
{

struct fragment
{
  fragment(const std::size_t p_current = 0u, 
           const std::size_t p_size    = 0u)
  : current(p_current),
    size(p_size)
  {}

  fragment(const fragment& p_other)
  : current(p_other.current),
    size(p_other.size)
  {}

  operator bool () const
  {
    return (size > 0);
  }
  
  void clear()
  {
    current = 0u;
    size    = 0u;
  }

  std::size_t   current;
  std::size_t   size;
};

inline std::size_t 
calculate_fragment_size(const std::size_t total_size,
                        const std::size_t fragment_size,
                        const std::size_t current)
{
  std::size_t remaining = total_size - current;
  return (remaining > fragment_size) ? fragment_size : remaining;
}                                  

inline fragment 
make_fragment(const std::size_t p_total_size,
              const std::size_t p_fragment_size)
{
  return
    fragment(0u, 
             calculate_fragment_size(p_total_size, 
                                     p_fragment_size, 
                                     0u));
}

inline fragment 
make_fragment(const std::size_t p_total_size,
              const std::size_t p_fragment_size,
              const fragment&   p_f)
{
  std::size_t next = p_f.current + p_f.size;
  return 
    fragment(next, 
             calculate_fragment_size(p_total_size, 
                                     p_fragment_size, 
                                     next));
}

template<typename Iterator>
inline std::pair<Iterator, Iterator>
make_iterator_range(Iterator base, const fragment& f)
{
  auto first = base + f.current;
  return std::make_pair(first, first + f.size);
}

/* Sample Code

auto f = make_fragment(container.size(), c_fragment_size);

do
{
  f = make_fragment(container.size(), c_fragment_size, f);
  if (f)
  {
    send (&container[f.current], &container[f.current + f.size]);
  }
} while(f);
*/

} // namespace base

} // namespace haluj

#endif // FRAGMENT_HPP

