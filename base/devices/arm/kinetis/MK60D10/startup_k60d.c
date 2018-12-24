/** \file startup_k60d.c */
/** specific peripheral definitions for Kinetis MK60D10 family devices */
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

#include "MK60D10.h"

#ifdef __cplusplus
extern "C" {
#endif

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

void DMA0_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void DMA1_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void DMA2_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void DMA3_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void DMA4_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void DMA5_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void DMA6_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void DMA7_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void DMA8_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void DMA9_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void DMA10_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void DMA11_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void DMA12_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void DMA13_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void DMA14_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void DMA15_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void DMAError_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void MCM_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void FTFL_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void ReadCollision_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void LVDLVW_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void LLW_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void Watchdog_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void RNG_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void I2C0_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void I2C1_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void SPI0_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void SPI1_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void SPI2_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void CAN0ORedMessageBuffer_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void CAN0BusOff_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void CAN0Error_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void CAN0TxWarning_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void CAN0RxWarning_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void CAN0WakeUp_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void CAN1ORedMessageBuffer_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void CAN1BusOff_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void CAN1Error_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void CAN1TxWarning_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void CAN1RxWarning_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void CAN1WakeUp_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void uart0_tx_rx_isr(void) __attribute__ ((weak, alias ("default_isr")));
void UART0ERR_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void UART1RXTX_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void UART1ERR_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void uart2_tx_rx_isr(void) __attribute__ ((weak, alias ("default_isr")));
void UART2ERR_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void UART3RXTX_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void UART3ERR_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void UART4RXTX_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void UART4ERR_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void UART5RXTX_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void UART5ERR_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void adc0_isr(void)  __attribute__ ((weak, alias ("default_isr")));
void ADC1_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void CMP0_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void CMP1_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void CMP2_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void FTM0_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void FTM1_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void FTM2_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void CMT_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void RTC_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void PIT0_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void PIT1_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void PIT2_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void PIT3_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void PDB0_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void USB0_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void USB_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void ENET1588Timer_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void ENETTransmit_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void ENETReceive_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void ENETError_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void I2S0_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void SDHC_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void DAC0_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void DAC1_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void TSI0_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void MCG_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void LPTimer_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void PORTA_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void PORTB_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void PORTC_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void PORTD_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void PORTE_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));
void Software_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));

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
  NMI_isr,                              // The NMI _isr
  HardFault_isr,                        // The hard fault _isr
  MemManage_isr,                        // The MPU fault _isr
  BusFault_isr,                         // The bus fault _isr
  UsageFault_isr,                       // The usage fault _isr
  0,                                    // Reserved
  0,                                    // Reserved
  0,                                    // Reserved
  0,                                    // Reserved
  SVC_isr,                              // SVCall _isr
  DebugMonitor_isr,                     // Debug monitor _isr
  0,                                    // Reserved
  PendSV_isr,                           // The PendSV _isr
  SysTick_isr,                          // The SysTick _isr
  DMA0_irq_isr,                         // DMA Channel 0 Transfer Complete
  DMA1_irq_isr,                         // DMA Channel 1 Transfer Complete
  DMA2_irq_isr,                         // DMA Channel 2 Transfer Complete
  DMA3_irq_isr,                         // DMA Channel 3 Transfer Complete
  DMA4_irq_isr,                         // DMA Channel 4 Transfer Complete
  DMA5_irq_isr,                         // DMA Channel 5 Transfer Complete
  DMA6_irq_isr,                         // DMA Channel 6 Transfer Complete
  DMA7_irq_isr,                         // DMA Channel 7 Transfer Complete
  DMA8_irq_isr,                         // DMA Channel 8 Transfer Complete
  DMA9_irq_isr,                         // DMA Channel 9 Transfer Complete
  DMA10_irq_isr,                        // DMA Channel 10 Transfer Complete
  DMA11_irq_isr,                        // DMA Channel 11 Transfer Complete
  DMA12_irq_isr,                        // DMA Channel 12 Transfer Complete
  DMA13_irq_isr,                        // DMA Channel 13 Transfer Complete
  DMA14_irq_isr,                        // DMA Channel 14 Transfer Complete
  DMA15_irq_isr,                        // DMA Channel 15 Transfer Complete
  DMAError_irq_isr,                     // DMA Error Interrupt
  MCM_irq_isr,                          // Normal Interrupt
  FTFL_irq_isr,                         // FTFL Interrupt
  ReadCollision_irq_isr,                // Read Collision Interrupt
  LVDLVW_irq_isr,                       // Low Voltage Detect, Low Voltage Warning
  LLW_irq_isr,                          // Low Leakage Wakeup
  Watchdog_irq_isr,                     // WDOG Interrupt
  RNG_irq_isr,                          // RNGB Interrupt
  I2C0_irq_isr,                         // I2C0 interrupt
  I2C1_irq_isr,                         // I2C1 interrupt
  SPI0_irq_isr,                         // SPI0 Interrupt
  SPI1_irq_isr,                         // SPI1 Interrupt
  SPI2_irq_isr,                         // SPI2 Interrupt
  CAN0ORedMessageBuffer_irq_isr,        // CAN0 OR'd Message Buffers Interrupt
  CAN0BusOff_irq_isr,                   // CAN0 Bus Off Interrupt
  CAN0Error_irq_isr,                    // CAN0 Error Interrupt
  CAN0TxWarning_irq_isr,                // CAN0 Tx Warning Interrupt
  CAN0RxWarning_irq_isr,                // CAN0 Rx Warning Interrupt
  CAN0WakeUp_irq_isr,                   // CAN0 Wake Up Interrupt
  default_isr,                          // Reserved interrupt 51
  default_isr,                          // Reserved interrupt 52
  CAN1ORedMessageBuffer_irq_isr,        // CAN1 OR'd Message Buffers Interrupt
  CAN1BusOff_irq_isr,                   // CAN1 Bus Off Interrupt
  CAN1Error_irq_isr,                    // CAN1 Error Interrupt
  CAN1TxWarning_irq_isr,                // CAN1 Tx Warning Interrupt
  CAN1RxWarning_irq_isr,                // CAN1 Rx Warning Interrupt
  CAN1WakeUp_irq_isr,                   // CAN1 Wake Up Interrupt
  default_isr,                          // Reserved interrupt 59
  default_isr,                          // Reserved interrupt 60
  uart0_tx_rx_isr,                      // UART0 Receive/Transmit interrupt
  UART0ERR_irq_isr,                     // UART0 Error interrupt
  UART1RXTX_irq_isr,                    // UART1 Receive/Transmit interrupt
  UART1ERR_irq_isr,                     // UART1 Error interrupt
  uart2_tx_rx_isr,                      // UART2 Receive/Transmit interrupt
  UART2ERR_irq_isr,                     // UART2 Error interrupt
  UART3RXTX_irq_isr,                    // UART3 Receive/Transmit interrupt
  UART3ERR_irq_isr,                     // UART3 Error interrupt
  UART4RXTX_irq_isr,                    // UART4 Receive/Transmit interrupt
  UART4ERR_irq_isr,                     // UART4 Error interrupt
  UART5RXTX_irq_isr,                    // UART5 Receive/Transmit interrupt
  UART5ERR_irq_isr,                     // UART5 Error interrupt
  adc0_isr,                             // ADC0 interrupt
  ADC1_irq_isr,                         // ADC1 interrupt
  CMP0_irq_isr,                         // CMP0 interrupt
  CMP1_irq_isr,                         // CMP1 interrupt
  CMP2_irq_isr,                         // CMP2 interrupt
  FTM0_irq_isr,                         // FTM0 fault, overflow and channels interrupt
  FTM1_irq_isr,                         // FTM1 fault, overflow and channels interrupt
  FTM2_irq_isr,                         // FTM2 fault, overflow and channels interrupt
  CMT_irq_isr,                          // CMT interrupt
  RTC_irq_isr,                          // RTC interrupt
  default_isr,                          // Reserved interrupt 83
  PIT0_irq_isr,                         // PIT timer channel 0 interrupt
  PIT1_irq_isr,                         // PIT timer channel 1 interrupt
  PIT2_irq_isr,                         // PIT timer channel 2 interrupt
  PIT3_irq_isr,                         // PIT timer channel 3 interrupt
  PDB0_irq_isr,                         // PDB0 Interrupt
  USB0_irq_isr,                         // USB0 interrupt
  USB_irq_isr,                          // USBInterrupt
  ENET1588Timer_irq_isr,                // Ethernet MAC IEEE 1588 Timer Interrupt
  ENETTransmit_irq_isr,                 // Ethernet MAC Transmit Interrupt
  ENETReceive_irq_isr,                  // Ethernet MAC Receive Interrupt
  ENETError_irq_isr,                    // Ethernet MAC Error and miscelaneous Interrupt
  I2S0_irq_isr,                         // I2S0 Interrupt
  SDHC_irq_isr,                         // SDHC Interrupt
  DAC0_irq_isr,                         // DAC0 interrupt
  DAC1_irq_isr,                         // DAC1 interrupt
  TSI0_irq_isr,                         // TSI0 Interrupt
  MCG_irq_isr,                          // MCG Interrupt
  LPTimer_irq_isr,                      // LPTimer interrupt
  default_isr,                          // Reserved interrupt 102
  PORTA_irq_isr,                        // Port A interrupt
  PORTB_irq_isr,                        // Port B interrupt
  PORTC_irq_isr,                        // Port C interrupt
  PORTD_irq_isr,                        // Port D interrupt
  PORTE_irq_isr,                        // Port E interrupt
  default_isr,                          // Reserved interrupt 108
  default_isr,                          // Reserved interrupt 109
  Software_irq_isr,                     // Software Interrupt
};

/** 
 * \brief   Flash configuration settings required for Kinetis family
 */
__attribute__ ((section(".flash_config")))
unsigned long g_flash_config_field_values[]={
  0xFFFFFFFF,                      // 0x400-0x407 Backdoor Comp Key
  0xFFFFFFFF,                      //
  0xFFFFFFFF,                      // 0x408-0x40B Program Flash Protection
  0xFFFFF9FE                       // 0x40C(FSEC)=0xFE, 0x40D(FOPT=0xFD)
                                   // Device is Unsecure, Ez-Port Diabled, NMI disabled, Normal Boot Mode
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
  while (1); /* This infinite loop should be reconsidered */
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

	// set M0PFD through M7PFD to 1 to disable prefetch */
	FMC_PFAPR |= FMC_PFAPR_M3PFD_MASK | FMC_PFAPR_M2PFD_MASK |
				 FMC_PFAPR_M1PFD_MASK | FMC_PFAPR_M0PFD_MASK;

	// set clock dividers to desired value */
	SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(outdiv1) |
				        SIM_CLKDIV1_OUTDIV2(outdiv2) |
				        SIM_CLKDIV1_OUTDIV3(outdiv3) |
				        SIM_CLKDIV1_OUTDIV4(outdiv4);

	// wait for dividers to change
	for (i = 0; i < outdiv4; i++) {
	}

	FMC_PFAPR = temp_reg;		// re-store original value of FMC_PFAPR

	return;
}		// set_sys_dividers

