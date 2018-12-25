/** \file startup_k64f.c */
/** specific peripheral definitions for Kinetis MK64F12 family devices */
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


#include "MK64F12.h"

#ifdef __cplusplus
extern "C" {
#endif

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

extern void (*__init_array_start)();
extern void (*__init_array_end)();

//extern int main(void);
void _start(void);
extern void __StackTop(void);

void reset_isr(void);
__attribute__ ((weak)) void NMI_isr(void);
__attribute__ ((weak)) void HardFault_isr(void);
__attribute__ ((weak)) void MemManage_isr(void);
__attribute__ ((weak)) void BusFault_isr(void);
__attribute__ ((weak)) void UsageFault_isr(void);
__attribute__ ((weak)) void SVC_isr(void);
__attribute__ ((weak)) void DebugMonitor_isr(void);
__attribute__ ((weak)) void PendSV_isr(void);
__attribute__ ((weak)) void SysTick_isr(void);
__attribute__ ((weak)) void default_isr(void);

void DMA0_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< DMA Channel 0 Transfer Complete */
void DMA1_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< DMA Channel 1 Transfer Complete */
void DMA2_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< DMA Channel 2 Transfer Complete */
void DMA3_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< DMA Channel 3 Transfer Complete */
void DMA4_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< DMA Channel 4 Transfer Complete */
void DMA5_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< DMA Channel 5 Transfer Complete */
void DMA6_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< DMA Channel 6 Transfer Complete */
void DMA7_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< DMA Channel 7 Transfer Complete */
void DMA8_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< DMA Channel 8 Transfer Complete */
void DMA9_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< DMA Channel 9 Transfer Complete */
void DMA10_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< DMA Channel 10 Transfer Complete */
void DMA11_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< DMA Channel 11 Transfer Complete */
void DMA12_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< DMA Channel 12 Transfer Complete */
void DMA13_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< DMA Channel 13 Transfer Complete */
void DMA14_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< DMA Channel 14 Transfer Complete */
void DMA15_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< DMA Channel 15 Transfer Complete */
void DMA_Error_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< DMA Error Interrupt */
void MCM_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< Normal Interrupt */
void FTFE_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< FTFE Command complete interrupt */
void Read_Collision_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< Read Collision Interrupt */
void LVD_LVW_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< Low Voltage Detect */
void LLW_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< Low Leakage Wakeup */
void Watchdog_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< WDOG Interrupt */
void RNG_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< RNG Interrupt */
void I2C0_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< I2C0 interrupt */
void I2C1_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< I2C1 interrupt */
void SPI0_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< SPI0 Interrupt */
void SPI1_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< SPI1 Interrupt */
void I2S0_Tx_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< I2S0 transmit interrupt */
void I2S0_Rx_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< I2S0 receive interrupt */
void UART0_LON_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< UART0 LON interrupt */
void UART0_RX_TX_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< UART0 Receive/Transmit interrupt */
void UART0_ERR_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< UART0 Error interrupt */
void UART1_RX_TX_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< UART1 Receive/Transmit interrupt */
void UART1_ERR_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< UART1 Error interrupt */
void UART2_RX_TX_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< UART2 Receive/Transmit interrupt */
void UART2_ERR_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< UART2 Error interrupt */
void UART3_RX_TX_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< UART3 Receive/Transmit interrupt */
void UART3_ERR_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< UART3 Error interrupt */
void ADC0_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< ADC0 interrupt */
void CMP0_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< CMP0 interrupt */
void CMP1_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< CMP1 interrupt */
void FTM0_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< FTM0 fault */
void FTM1_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< FTM1 fault */
void FTM2_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< FTM2 fault */
void CMT_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< CMT interrupt */
void RTC_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< RTC interrupt */
void RTC_Seconds_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< RTC seconds interrupt */
void PIT0_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< PIT timer channel 0 interrupt */
void PIT1_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< PIT timer channel 1 interrupt */
void PIT2_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< PIT timer channel 2 interrupt */
void PIT3_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< PIT timer channel 3 interrupt */
void PDB0_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< PDB0 Interrupt */
void USB0_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< USB0 interrupt */
void USBDCD_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< USBDCD Interrupt */
void Reserved71_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< Reserved interrupt 71 */
void DAC0_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< DAC0 interrupt */
void MCG_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< MCG Interrupt */
void LPTimer_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< LPTimer interrupt */
void PORTA_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< Port A interrupt */
void PORTB_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< Port B interrupt */
void PORTC_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< Port C interrupt */
void PORTD_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< Port D interrupt */
void PORTE_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< Port E interrupt */
void SWI_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< Software interrupt */
void SPI2_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< SPI2 Interrupt */
void UART4_RX_TX_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< UART4 Receive/Transmit interrupt */
void UART4_ERR_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< UART4 Error interrupt */
void UART5_RX_TX_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< UART5 Receive/Transmit interrupt */
void UART5_ERR_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< UART5 Error interrupt */
void CMP2_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< CMP2 interrupt */
void FTM3_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< FTM3 fault */
void DAC1_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< DAC1 interrupt */
void ADC1_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< ADC1 interrupt */
void I2C2_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< I2C2 interrupt */
void CAN0_ORed_Message_buffer_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< CAN0 OR'd message buffers interrupt */
void CAN0_Bus_Off_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< CAN0 bus off interrupt */
void CAN0_Error_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< CAN0 error interrupt */
void CAN0_Tx_Warning_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< CAN0 Tx warning interrupt */
void CAN0_Rx_Warning_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< CAN0 Rx warning interrupt */
void CAN0_Wake_Up_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< CAN0 wake up interrupt */
void SDHC_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< SDHC interrupt */
void ENET_1588_Timer_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< Ethernet MAC IEEE 1588 Timer Interrupt */
void ENET_Transmit_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< Ethernet MAC Transmit Interrupt */
void ENET_Receive_irq_isr (void) __attribute__ ((weak, alias ("default_isr"))); /**< Ethernet MAC Receive Interrupt */
void ENET_Error_irq_isr  (void) __attribute__ ((weak, alias ("default_isr")));/**< Ethernet MAC Error and miscelaneous Interrupt */

#ifdef __cplusplus
}
#endif

/** 
 * \brief  Interrupt service routines located in flash
 */
__attribute__ ((section(".isr_vector")))
void (* const g_pfnVectors[])(void) = {
  &__StackTop,                             // The initial stack pointer
  reset_isr,                           // The reset _isr
  NMI_isr,                             // The NMI _isr
  HardFault_isr,                       // The hard fault _isr
  MemManage_isr,                       // The MPU fault _isr
  BusFault_isr,                        // The bus fault _isr
  UsageFault_isr,                      // The usage fault _isr
  0,                                       // Reserved
  0,                                       // Reserved
  0,                                       // Reserved
  0,                                       // Reserved
  SVC_isr,                             // SVCall _isr
  DebugMonitor_isr,                    // Debug monitor _isr
  0,                                       // Reserved
  PendSV_isr,                          // The PendSV _isr
  SysTick_isr,                         // The SysTick _isr
  DMA0_irq_isr , /**< DMA Channel 0 Transfer Complete */
  DMA1_irq_isr , /**< DMA Channel 1 Transfer Complete */
  DMA2_irq_isr , /**< DMA Channel 2 Transfer Complete */
  DMA3_irq_isr , /**< DMA Channel 3 Transfer Complete */
  DMA4_irq_isr , /**< DMA Channel 4 Transfer Complete */
  DMA5_irq_isr , /**< DMA Channel 5 Transfer Complete */
  DMA6_irq_isr , /**< DMA Channel 6 Transfer Complete */
  DMA7_irq_isr , /**< DMA Channel 7 Transfer Complete */
  DMA8_irq_isr , /**< DMA Channel 8 Transfer Complete */
  DMA9_irq_isr , /**< DMA Channel 9 Transfer Complete */
  DMA10_irq_isr , /**< DMA Channel 10 Transfer Complete */
  DMA11_irq_isr , /**< DMA Channel 11 Transfer Complete */
  DMA12_irq_isr , /**< DMA Channel 12 Transfer Complete */
  DMA13_irq_isr , /**< DMA Channel 13 Transfer Complete */
  DMA14_irq_isr , /**< DMA Channel 14 Transfer Complete */
  DMA15_irq_isr , /**< DMA Channel 15 Transfer Complete */
  DMA_Error_irq_isr , /**< DMA Error Interrupt */
  MCM_irq_isr , /**< Normal Interrupt */
  FTFE_irq_isr , /**< FTFE Command complete interrupt */
  Read_Collision_irq_isr , /**< Read Collision Interrupt */
  LVD_LVW_irq_isr , /**< Low Voltage Detect */
  LLW_irq_isr , /**< Low Leakage Wakeup */
  Watchdog_irq_isr , /**< WDOG Interrupt */
  RNG_irq_isr , /**< RNG Interrupt */
  I2C0_irq_isr , /**< I2C0 interrupt */
  I2C1_irq_isr , /**< I2C1 interrupt */
  SPI0_irq_isr , /**< SPI0 Interrupt */
  SPI1_irq_isr , /**< SPI1 Interrupt */
  I2S0_Tx_irq_isr , /**< I2S0 transmit interrupt */
  I2S0_Rx_irq_isr , /**< I2S0 receive interrupt */
  UART0_LON_irq_isr , /**< UART0 LON interrupt */
  UART0_RX_TX_irq_isr , /**< UART0 Receive/Transmit interrupt */
  UART0_ERR_irq_isr , /**< UART0 Error interrupt */
  UART1_RX_TX_irq_isr , /**< UART1 Receive/Transmit interrupt */
  UART1_ERR_irq_isr , /**< UART1 Error interrupt */
  UART2_RX_TX_irq_isr , /**< UART2 Receive/Transmit interrupt */
  UART2_ERR_irq_isr , /**< UART2 Error interrupt */
  UART3_RX_TX_irq_isr , /**< UART3 Receive/Transmit interrupt */
  UART3_ERR_irq_isr , /**< UART3 Error interrupt */
  ADC0_irq_isr , /**< ADC0 interrupt */
  CMP0_irq_isr , /**< CMP0 interrupt */
  CMP1_irq_isr , /**< CMP1 interrupt */
  FTM0_irq_isr , /**< FTM0 fault */
  FTM1_irq_isr , /**< FTM1 fault */
  FTM2_irq_isr , /**< FTM2 fault */
  CMT_irq_isr , /**< CMT interrupt */
  RTC_irq_isr , /**< RTC interrupt */
  RTC_Seconds_irq_isr , /**< RTC seconds interrupt */
  PIT0_irq_isr , /**< PIT timer channel 0 interrupt */
  PIT1_irq_isr , /**< PIT timer channel 1 interrupt */
  PIT2_irq_isr , /**< PIT timer channel 2 interrupt */
  PIT3_irq_isr , /**< PIT timer channel 3 interrupt */
  PDB0_irq_isr , /**< PDB0 Interrupt */
  USB0_irq_isr , /**< USB0 interrupt */
  USBDCD_irq_isr , /**< USBDCD Interrupt */
  Reserved71_irq_isr , /**< Reserved interrupt 71 */
  DAC0_irq_isr , /**< DAC0 interrupt */
  MCG_irq_isr , /**< MCG Interrupt */
  LPTimer_irq_isr , /**< LPTimer interrupt */
  PORTA_irq_isr , /**< Port A interrupt */
  PORTB_irq_isr , /**< Port B interrupt */
  PORTC_irq_isr , /**< Port C interrupt */
  PORTD_irq_isr , /**< Port D interrupt */
  PORTE_irq_isr , /**< Port E interrupt */
  SWI_irq_isr , /**< Software interrupt */
  SPI2_irq_isr , /**< SPI2 Interrupt */
  UART4_RX_TX_irq_isr , /**< UART4 Receive/Transmit interrupt */
  UART4_ERR_irq_isr , /**< UART4 Error interrupt */
  UART5_RX_TX_irq_isr , /**< UART5 Receive/Transmit interrupt */
  UART5_ERR_irq_isr , /**< UART5 Error interrupt */
  CMP2_irq_isr , /**< CMP2 interrupt */
  FTM3_irq_isr , /**< FTM3 fault */
  DAC1_irq_isr , /**< DAC1 interrupt */
  ADC1_irq_isr , /**< ADC1 interrupt */
  I2C2_irq_isr , /**< I2C2 interrupt */
  CAN0_ORed_Message_buffer_irq_isr , /**< CAN0 OR'd message buffers interrupt */
  CAN0_Bus_Off_irq_isr , /**< CAN0 bus off interrupt */
  CAN0_Error_irq_isr , /**< CAN0 error interrupt */
  CAN0_Tx_Warning_irq_isr , /**< CAN0 Tx warning interrupt */
  CAN0_Rx_Warning_irq_isr , /**< CAN0 Rx warning interrupt */
  CAN0_Wake_Up_irq_isr , /**< CAN0 wake up interrupt */
  SDHC_irq_isr , /**< SDHC interrupt */
  ENET_1588_Timer_irq_isr , /**< Ethernet MAC IEEE 1588 Timer Interrupt */
  ENET_Transmit_irq_isr , /**< Ethernet MAC Transmit Interrupt */
  ENET_Receive_irq_isr , /**< Ethernet MAC Receive Interrupt */
  ENET_Error_irq_isr /**< Ethernet MAC Error and miscelaneous Interrupt */
};

/** 
 * \brief   Flash configuration settings required for Kinetis family
 */
 
__attribute__ ((section(".flash_config")))
unsigned long g_flash_config_field_values[]={
  0xFFFFFFFF,                      // 0x400-0x407 Backdoor Comp Key
  0xFFFFFFFF,                      //
  0xFFFFFFFF,                      // 0x408-0x40B Program Flash Protection
  0xFFFFFFFE                       // 0x40C(FSEC)=0xFE, 0x40D(FOPT=0xFD)
                                   // Device is Unsecure, Ez-Port Enabled, Fast Clock Mode On
};

/** 
 * \brief   section for RAM interrupt service table copied from flash section
 */
__attribute__ ((section(".isr_vector_ram")))
void (* const g_isr_vectors[256])(void) = {};

/** 
 * \fn      default_isr
 * \brief   default interrupt service handler for unused interrupts
 * \param   void
 * \return  void
 */
void default_isr(void)
{
  while (1);
}

/** 
 * \fn     set_sys_dividers
 * \brief  Sets system dividers
 * \param  outdiv1, outdiv2, outdiv3, outdiv4
 * \return void
 */
__attribute((section(".ramcode")))
void  set_sys_dividers(uint32_t outdiv1, uint32_t outdiv2, uint32_t outdiv3, uint32_t outdiv4)
{
  uint32_t temp_reg;
  uint8_t  i;

  temp_reg = FMC_PFAPR;		// store present value of FMC_PFAPR

  /* set M0PFD through M7PFD to 1 to disable prefetch */
  FMC_PFAPR |= FMC_PFAPR_M3PFD_MASK | FMC_PFAPR_M2PFD_MASK |
               FMC_PFAPR_M1PFD_MASK | FMC_PFAPR_M0PFD_MASK;

  /* set clock dividers to desired value */
  SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(outdiv1) |
                SIM_CLKDIV1_OUTDIV2(outdiv2) |
                SIM_CLKDIV1_OUTDIV3(outdiv3) |
                SIM_CLKDIV1_OUTDIV4(outdiv4);

  /* wait for dividers to change */
  for (i = 0; i < outdiv4; i++) {
  }

  FMC_PFAPR = temp_reg;   /* re-store original value of FMC_PFAPR */

  return;
} /* set_sys_dividers */

