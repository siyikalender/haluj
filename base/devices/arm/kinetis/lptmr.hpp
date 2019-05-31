/// \file lptmr.hpp
/// LPTMR peripheral for kinetis devices
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

#ifndef HALUJ_BASE_DEVICES_KINETIS_LPTMR_HPP
#define HALUJ_BASE_DEVICES_KINETIS_LPTMR_HPP

#include "peripheral.hpp"
#include "options.hpp"
#include "bitops.hpp"

namespace haluj
{

namespace base
{

namespace devices
{

namespace arm
{
  
namespace kinetis
{

template <typename Specifier>
struct lptmr : peripheral<Specifier>
{
  static constexpr LPTMR_Type* lptmr_addr()  {return reinterpret_cast<LPTMR_Type*>(Specifier::lptmr_base);}

  static void start()
  {
    uint32_t  v = 0;
    v = bit_set(v, LPTMR_CSR_TIE_SHIFT);
    v = bit_set(v, LPTMR_CSR_TEN_SHIFT);
    lptmr_addr()->CSR = v;
  }
  
  static inline void stop()
  {
    lptmr_addr()->CSR = bit_clear(lptmr_addr()->CSR, LPTMR_CSR_TEN_SHIFT);
  }
  
  static inline void clear()
  {
    clear_interrupt();
  }

  static inline  bool test_interrupt()
  {
    return bit_test(lptmr_addr()->CSR, LPTMR_CSR_TCF_SHIFT);
  }

  static inline  void clear_interrupt()
  {
    lptmr_addr()->CSR |= LPTMR_CSR_TCF_MASK;
  }

  // template<typename Options>
  static inline void configure(/*Options p_opts*/)
  {
    stop();
    clear();
    // p_opts.template accept<uint32_t>(lptmr<Specifier>(), null_op());
    lptmr_addr()->PSR = LPTMR_PSR_PRESCALE(5);
    start();
  }  
  
  static inline bool is_running()
  {
    return bit_test(lptmr_addr()->CSR, LPTMR_CSR_TEN_SHIFT); 
  }
  
  static inline unsigned get_count()
  {
    return lptmr_addr()->CNR;
  }
  
  static inline void set_compare(const unsigned p_value)
  {
    lptmr_addr()->CMR = LPTMR_CMR_COMPARE(p_value);
  }

  static inline void set_prescale(const unsigned p_value)
  {
    lptmr_addr()->PSR = LPTMR_PSR_PRESCALE(p_value);
  }

};

} // namespace kinetis

} // namespace arm

} // namespace devices

} // namespace base

} // namespace haluj

// HALUJ_BASE_DEVICES_KINETIS_LPTMR_HPP
#endif
