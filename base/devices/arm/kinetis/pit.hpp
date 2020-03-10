/// \file pit.hpp
/// timer complaint Kimetis PIT peripheral implementation
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
/// \date   2019

#ifndef HALUJ_BASE_DEVICES_ARM_SYSTICK_HPP
#define HALUJ_BASE_DEVICES_ARM_SYSTICK_HPP

#include <cstdint>

namespace haluj
{

namespace base
{

namespace devices
{
  
namespace arm
{

struct pit
{
  typedef uint32_t duration;
  
  static bool operator()(const uint32_t)
  {
    return predicate();
  }

  static bool predicate() const
  {
    return false;
  }

  static void reset()
  {}

  static void load(const uint32_t p_value) 
  {}

  static void start()
  {}
  
  static void stop()
  {}
  
  static bool is_running() const
  {
    return false;
  }
  
};
  
} // namespace devices

} // namespace arm

} // namespace base

} // namespace haluj

#endif // HALUJ_BASE_DEVICES_ARM_SYSTICK_HPP

