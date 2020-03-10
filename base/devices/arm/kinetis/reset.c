/// \file reset.h
/// Reset ISR routine for Kinetis devices
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

#include "reset.h"
#include "vendor.h"

extern unsigned long __ramcode_load__;
extern unsigned long __ramcode_start__;
extern unsigned long __ramcode_end__;
extern unsigned long __VECTOR_TABLE;
extern unsigned long __interrupts_ram_start__;
extern unsigned long __interrupts_ram_end__;
extern unsigned long __DATA_ROM;
extern unsigned long __data_start__;
extern unsigned long __data_end__;
extern unsigned long __bss_start__;
extern unsigned long __bss_end__;

void _start(void);

void reset_isr(void)
{
  unsigned char *source;
  unsigned char *destination;

  __disable_irq();

  disable_watchdog();

  #ifndef USE_RAM_LD

  #ifndef DONT_USE_RAM_VECTOR
  /* Copy the vector table from ROM to RAM */
  source = (unsigned char *) &__VECTOR_TABLE;
  destination = (unsigned char *)&__interrupts_ram_start__;
  while (destination < (unsigned char*)&__interrupts_ram_end__)
  {
      *(destination++) = *(source++);
  }
  /* Point the VTOR to the position of vector table */
  SCB->VTOR = (uint32_t) &__interrupts_ram_start__;
  #endif

  #ifndef DONT_USE_RAM_CODE
  /* copy ram code */
  source = (unsigned char *)&__ramcode_load__;
  destination = (unsigned char *)&__ramcode_start__;
  while (destination < (unsigned char*)&__ramcode_end__) {
    *(destination++) = *(source++);
  }
  #endif

  /* copy data values from ROM to RAM */
  source = (unsigned char *)&__DATA_ROM;
  destination = (unsigned char *)&__data_start__;
  while (destination < (unsigned char*)&__data_end__) {
    *(destination++) = *(source++);
  }

  /* clear bss section */
  source = (unsigned char *)&__bss_start__;
  destination = (unsigned char *)&__bss_end__;
  while (source < destination ) {
    *source++ = 0;
  }

  /* USE_RAM_LD */
  #else

  SCB->VTOR = (uint32_t) &__VECTOR_TABLE;

  /* clear bss section */
  source = (unsigned char *)&__bss_start__;
  destination = (unsigned char *)&__bss_end__;
  while (source < destination ) {
    *source++ = 0;
  }
  
  /* USE_RAM_LD */
  #endif 

  system_init();

  __enable_irq();

  _start();

  while(1);
}

#ifdef NO_LIBC_START

// This section may be used, if tight on memory requirements. 
extern int main(void);

void __libc_init_array(void);
void __libc_fini_array(void);

void _init(void)
{}

void _fini(void)
{}

void _start(void)
{
  __libc_init_array();

  main();
  
  __libc_fini_array();
}

#endif
