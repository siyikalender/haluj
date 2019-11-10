/** \file startup_kl03.c 
 * interrupts, handlers, flash configuration, bootloader configuration 
*/
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
/**
 * \author Martin Kojtal - 0xc0170 (Author of orijinal)
 * \author Selcuk Iyikalender - siyikalender 
 * \date   2018
*/

/*
Based on 0xc0170's startup_k60.c, WTFPL 2

https://github.com/0xc0170/kinetis_kxx_gcc/blob/master/gpio_demo_twrk60n512/cpu/startup_k60.c
*/

#include "MKL03Z4.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void __StackTop(void);

void reset_isr(void);
__attribute__ ((weak)) void NonMaskableInt_isr (void); //*< Non Maskable Interrupt 
__attribute__ ((weak)) void HardFault_isr      (void); //*< Cortex-M0 SV Hard Fault Interrupt 
__attribute__ ((weak)) void SVCall_isr         (void); //*< Cortex-M0 SV Call Interrupt 
__attribute__ ((weak)) void PendSV_isr         (void); //*< Cortex-M0 Pend SV Interrupt 
__attribute__ ((weak)) void SysTick_isr        (void); //*< Cortex-M0 System Tick Interrupt 
__attribute__ ((weak)) void default_isr(void);

void FTFA_isr           (void) __attribute__ ((weak, alias ("default_isr"))); //*< Command complete and read collision 
void PMC_isr            (void) __attribute__ ((weak, alias ("default_isr"))); //*< Low-voltage detect, low-voltage warning 
void LLWU_isr           (void) __attribute__ ((weak, alias ("default_isr"))); //*< Low leakage wakeup 
void I2C0_isr           (void) __attribute__ ((weak, alias ("default_isr"))); //*< I2C0 interrupt 
void SPI0_isr           (void) __attribute__ ((weak, alias ("default_isr"))); //*< SPI0 single interrupt vector for all sources 
void LPUART0_isr        (void) __attribute__ ((weak, alias ("default_isr"))); //*< LPUART0 status and error 
void ADC0_isr           (void) __attribute__ ((weak, alias ("default_isr"))); //*< ADC0 interrupt 
void CMP0_isr           (void) __attribute__ ((weak, alias ("default_isr"))); //*< CMP0 interrupt 
void TPM0_isr           (void) __attribute__ ((weak, alias ("default_isr"))); //*< TPM0 single interrupt vector for all sources 
void TPM1_isr           (void) __attribute__ ((weak, alias ("default_isr"))); //*< TPM1 single interrupt vector for all sources 
void RTC_isr            (void) __attribute__ ((weak, alias ("default_isr"))); //*< RTC alarm 
void RTC_Seconds_isr    (void) __attribute__ ((weak, alias ("default_isr"))); //*< RTC seconds 
void LPTMR0_isr         (void) __attribute__ ((weak, alias ("default_isr"))); //*< LPTMR0 interrupt 
void PORTA_isr          (void) __attribute__ ((weak, alias ("default_isr"))); //*< PORTA Pin detect 
void PORTB_isr          (void) __attribute__ ((weak, alias ("default_isr"))); //*< PORTB Pin detect 

#ifdef __cplusplus
}
#endif

/** 
 * \brief  Interrupt service routines located in flash
 */
__attribute__ ((section(".isr_vector")))
void (* const g_pfnVectors[])(void) = {
  &__StackTop,                          // The initial stack pointer
  reset_isr,                            // The reset _isr
  NonMaskableInt_isr          ,         // < Non Maskable Interrupt
  HardFault_isr               ,         // < Cortex-M0 SV Hard Fault Interrupt
  0,                                    // The MPU fault _isr
  0,                                    // The bus fault _isr
  0,                                    // The usage fault _isr
  0,                                    // Reserved
  0,                                    // Reserved
  0,                                    // Reserved
  0,   
  SVCall_isr                  ,         // < Cortex-M0 SV Call Interrupt
  0,                                    // Reserved
  0,   
  PendSV_isr                  ,         // < Cortex-M0 Pend SV Interrupt
  SysTick_isr                 ,         // < Cortex-M0 System Tick Interrupt
  0                           ,         //*< Reserved interrupt 
  0                           ,         //*< Reserved interrupt 
  0                           ,         //*< Reserved interrupt 
  0                           ,         //*< Reserved interrupt 
  0                           ,         //*< Reserved interrupt 
  FTFA_isr                    ,         //*< Command complete and read collision 
  PMC_isr                     ,         //*< Low-voltage detect, low-voltage warning 
  LLWU_isr                    ,         //*< Low leakage wakeup 
  I2C0_isr                    ,         //*< I2C0 interrupt 
  0                           ,         //*< Reserved interrupt 
  SPI0_isr                    ,         //*< SPI0 single interrupt vector for all sources 
  0                           ,         //*< Reserved interrupt 
  LPUART0_isr                 ,         //*< LPUART0 status and error 
  0                           ,         //*< Reserved interrupt 
  0                           ,         //*< Reserved interrupt 
  ADC0_isr                    ,         //*< ADC0 interrupt 
  CMP0_isr                    ,         //*< CMP0 interrupt 
  TPM0_isr                    ,         //*< TPM0 single interrupt vector for all sources 
  TPM1_isr                    ,         //*< TPM1 single interrupt vector for all sources 
  0                           ,         //*< Reserved interrupt 
  RTC_isr                     ,         //*< RTC alarm 
  RTC_Seconds_isr             ,         //*< RTC seconds 
  0                           ,         //*< Reserved interrupt 
  0                           ,         //*< Reserved interrupt 
  0                           ,         //*< Reserved interrupt 
  0                           ,         //*< Reserved interrupt 
  0                           ,         //*< Reserved interrupt 
  0                           ,         //*< Reserved interrupt 
  LPTMR0_isr                  ,         //*< LPTMR0 interrupt 
  0                           ,         //*< Reserved interrupt 
  PORTA_isr                   ,         //*< PORTA Pin detect 
  PORTB_isr                             //*< PORTB Pin detect 
};

/** 
 * \brief   Bootloader configuration settings required for MKL03 family
 */
__attribute__ ((section(".bootloader_config")))
uint8_t g_bootloader_field_values[]={
  // tag (4 bytes) Magic number 0x6766636B
  0x6B, 0x63, 0x66, 0x67,
  // Crc Start Address (4 bytes)
  0xFF, 0xFF, 0xFF, 0xFF,   // 0xFFFFFFFF to disable
  // Crc Byte Count    (4 bytes)
  0xFF, 0xFF, 0xFF, 0xFF,   // 0xFFFFFFFF to disable
  // Crc Expected Value (4 bytes) 
  0xFF, 0xFF, 0xFF, 0xFF,   // 0xFFFFFFFF to disable
  // Enabled Peripherals (1 byte)
  0x01, // Enable UART
  // I2C address (1 byte)
  0x10, // Enable UART
  // Peripheral Detection Timeout (2 bytes)
  0xE8, 0x03,
  // USB Vendor Id (2 bytes)
  0xFF, 0xFF,
  // USB Product Id (2 bytes)
  0xFF, 0xFF,
  // Clock Flags : Clear Bit 0 for High Speed Clock
  0xFF,
  // Clock Divider
  0xFF
};

/** 
 * \brief   Flash configuration settings required for Kinetis family
 */
__attribute__ ((section(".flash_config")))
uint8_t g_flash_config_field_values[]={
  // 0x400-0x407 Backdoor Comparison Key 8 bytes
  0xFF,   // 0x400  
  0xFF,   // 0x401 
  0xFF,   // 0x402
  0xFF,   // 0x403
  0xFF,   // 0x404
  0xFF,   // 0x405
  0xFF,   // 0x406
  0xFF,   // 0x407
  // 0x408-0x40B Program Flash Protection 4 bytes
  0xFF,   // 0x408
  0xFF,   // 0x409
  0xFF,   // 0x40A
  0xFF,   // 0x40B
  // 0x40C Secure Flash (FSEC) - 0xFE is Unsecured
  0xFE,
  // 0x40D Nonvolatile Option (FOPT) - 0x3F Fast Initialize, Boot from Flash ..
  0x3F,
  0xFF,   // 0x40E
  0xFF    // 0x40F
};

/** 
 * \brief   section for RAM interrupt service table copied from flash section
 */
__attribute__ ((section(".isr_vector_ram")))
void (* const g_isr_vectors[64])(void) = {};

/** 
 * \fn      default_isr
 * \brief   default interrupt service handler for unused interrupts
 * \param   void
 * \return  void
 */
void default_isr(void)
{
  while (1);  /* This infinite loop should be reconsidered */
}

