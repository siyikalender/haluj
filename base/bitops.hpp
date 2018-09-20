/// \file bitops.hpp
/// utilities for bit and mask operations
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

#ifndef HALUJ_BASE_BITOPS_HPP
#define HALUJ_BASE_BITOPS_HPP

template<typename T>
constexpr inline T 
mask(const T p_bit_offset)
{
  return T(T(1) << p_bit_offset);
}

template<typename T, typename... Args>
constexpr inline T 
mask(const T p_bit_offset, Args... args)
{
  return T(T(1) << p_bit_offset) | mask(args...);
}

template<typename T>
inline T 
mask_range(const T p_bit_first, const T p_bit_last)
{
  T result(0);
  
  for (T i = p_bit_first; i < p_bit_last; i++)
  {
    result |= mask(i);
  }
  
  return result;
}

template<typename T, typename U>
inline bool 
mask_test(const T p_value, 
          const U p_mask)
{
  return ((p_value & T(p_mask)) == T(p_mask));
}

template<typename T, typename U>
inline T
mask_set(const T  p_value,
         const U  p_mask)
{
  return p_value | T(p_mask);
}

template<typename T, typename U>
inline T 
mask_clear(const T  p_value, 
           const U  p_mask)
{
  return p_value & ~T(p_mask);
}

template<typename T, typename U>
inline T 
mask_toggle(const T  p_value, 
            const U  p_mask)
{
  return p_value ^ T(p_mask);
}

template<typename T, typename U>
inline bool 
bit_test(const T p_value, 
         const U p_bit_offset)
{
  return mask_test(p_value, mask(p_bit_offset));
}

template<typename T, typename U>
inline T 
bit_set(const T   p_value, 
        const U   p_bit_offset)
{
  return mask_set(p_value, mask(p_bit_offset));
}

template<typename T, typename U>
inline T 
bit_clear(const T p_value, 
          const U p_bit_offset)
{
  return mask_clear(p_value, mask(p_bit_offset));
}

template<typename T, typename U>
inline T
bit_toggle(const T p_value, 
           const U p_bit_offset)
{
  return mask_toggle(p_value, mask(p_bit_offset));
}

template<typename T, typename U>
inline T 
bit_assign(const T    p_value, 
           const U    p_bit_offset,
           const bool p_f)
{
  T result;

  if (p_f)
  {
    result = bit_set(p_value, p_bit_offset);
  }
  else
  {
    result = bit_clear(p_value, p_bit_offset);
  }

  return result;
}

template<unsigned Count,
         typename T,
         typename U>
inline void
wait_bit(T&       p_ref,
         const U  p_bit_offset)
{
  for (unsigned u = 0; (u < Count) && !bit_test(p_ref, p_bit_offset); u++);
}

template<unsigned Count,
         typename T,
         typename U>
inline void
wait_mask(T&      p_ref,
          const U p_mask)
{
  for (unsigned u = 0; (u < Count) && !mask_test(p_ref, p_mask); u++);
}

// HALUJ_BASE_BITOPS_HPP
#endif
