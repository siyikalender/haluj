/**
 ** This file is released under Creative Commons Zero - CC0
 **/

/*
Based on 0xc0170's startup_k60.c, WTFPL 2

https://github.com/0xc0170/kinetis_kxx_gcc/blob/master/gpio_demo_twrk60n512/cpu/startup_k60.c
*/

#include "MKV31F51212.h"

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

void DMA0_irq_isr          (void) __attribute__ ((weak, alias ("default_isr")));  // DMA Channel 0 Transfer Complete
void DMA1_irq_isr          (void) __attribute__ ((weak, alias ("default_isr")));  // DMA Channel 1 Transfer Complete
void DMA2_irq_isr          (void) __attribute__ ((weak, alias ("default_isr")));  // DMA Channel 2 Transfer Complete
void DMA3_irq_isr          (void) __attribute__ ((weak, alias ("default_isr")));  // DMA Channel 3 Transfer Complete
void DMA4_irq_isr          (void) __attribute__ ((weak, alias ("default_isr")));  // DMA Channel 4 Transfer Complete
void DMA5_irq_isr          (void) __attribute__ ((weak, alias ("default_isr")));  // DMA Channel 5 Transfer Complete
void DMA6_irq_isr          (void) __attribute__ ((weak, alias ("default_isr")));  // DMA Channel 6 Transfer Complete
void DMA7_irq_isr          (void) __attribute__ ((weak, alias ("default_isr")));  // DMA Channel 7 Transfer Complete
void DMA8_irq_isr          (void) __attribute__ ((weak, alias ("default_isr")));  // DMA Channel 8 Transfer Complete
void DMA9_irq_isr          (void) __attribute__ ((weak, alias ("default_isr")));  // DMA Channel 9 Transfer Complete
void DMA10_irq_isr         (void) __attribute__ ((weak, alias ("default_isr")));  // DMA Channel 10 Transfer Complete
void DMA11_irq_isr         (void) __attribute__ ((weak, alias ("default_isr")));  // DMA Channel 11 Transfer Complete
void DMA12_irq_isr         (void) __attribute__ ((weak, alias ("default_isr")));  // DMA Channel 12 Transfer Complete
void DMA13_irq_isr         (void) __attribute__ ((weak, alias ("default_isr")));  // DMA Channel 13 Transfer Complete
void DMA14_irq_isr         (void) __attribute__ ((weak, alias ("default_isr")));  // DMA Channel 14 Transfer Complete
void DMA15_irq_isr         (void) __attribute__ ((weak, alias ("default_isr")));  // DMA Channel 15 Transfer Complete
void DMA_Error_irq_isr     (void) __attribute__ ((weak, alias ("default_isr")));  // DMA Error Interrupt
void MCM_irq_isr           (void) __attribute__ ((weak, alias ("default_isr")));  // Normal Interrupt
void FTF_irq_isr           (void) __attribute__ ((weak, alias ("default_isr")));  // FTFA Command complete interrupt
void Read_Collision_irq_isr(void) __attribute__ ((weak, alias ("default_isr")));  // Read Collision Interrupt
void LVD_LVW_irq_isr       (void) __attribute__ ((weak, alias ("default_isr")));  // Low Voltage Detect, Low Voltage Warning
void LLWU_irq_isr          (void) __attribute__ ((weak, alias ("default_isr")));  // Low Leakage Wakeup Unit
void WDOG_EWM_irq_isr      (void) __attribute__ ((weak, alias ("default_isr")));  // WDOG Interrupt
void RNG_irq_isr           (void) __attribute__ ((weak, alias ("default_isr")));  // RNG Interrupt
void I2C0_irq_isr          (void) __attribute__ ((weak, alias ("default_isr")));  // I2C0 interrupt
void I2C1_irq_isr          (void) __attribute__ ((weak, alias ("default_isr")));  // I2C1 interrupt
void SPI0_irq_isr          (void) __attribute__ ((weak, alias ("default_isr")));  // SPI0 Interrupt
void SPI1_irq_isr          (void) __attribute__ ((weak, alias ("default_isr")));  // SPI1 Interrupt
void Reserved44_irq_isr    (void) __attribute__ ((weak, alias ("default_isr")));  // Reserved Interrupt 44
void Reserved45_irq_isr    (void) __attribute__ ((weak, alias ("default_isr")));  // Reserved interrupt 45
void LPUART0_irq_isr       (void) __attribute__ ((weak, alias ("default_isr")));  // LPUART0 status/error interrupt
void UART0_RX_TX_irq_isr   (void) __attribute__ ((weak, alias ("default_isr")));  // UART0 Receive/Transmit interrupt
void UART0_ERR_irq_isr     (void) __attribute__ ((weak, alias ("default_isr")));  // UART0 Error interrupt
void UART1_RX_TX_irq_isr   (void) __attribute__ ((weak, alias ("default_isr")));  // UART1 Receive/Transmit interrupt
void UART1_ERR_irq_isr     (void) __attribute__ ((weak, alias ("default_isr")));  // UART1 Error interrupt
void UART2_RX_TX_irq_isr   (void) __attribute__ ((weak, alias ("default_isr")));  // UART2 Receive/Transmit interrupt
void UART2_ERR_irq_isr     (void) __attribute__ ((weak, alias ("default_isr")));  // UART2 Error interrupt
void Reserved53_irq_isr    (void) __attribute__ ((weak, alias ("default_isr")));  // Reserved interrupt 53
void Reserved54_irq_isr    (void) __attribute__ ((weak, alias ("default_isr")));  // Reserved interrupt 54
void ADC0_irq_isr          (void) __attribute__ ((weak, alias ("default_isr")));  // ADC0 interrupt
void CMP0_irq_isr          (void) __attribute__ ((weak, alias ("default_isr")));  // CMP0 interrupt
void CMP1_irq_isr          (void) __attribute__ ((weak, alias ("default_isr")));  // CMP1 interrupt
void FTM0_irq_isr          (void) __attribute__ ((weak, alias ("default_isr")));  // FTM0 fault, overflow and channels interrupt
void FTM1_irq_isr          (void) __attribute__ ((weak, alias ("default_isr")));  // FTM1 fault, overflow and channels interrupt
void FTM2_irq_isr          (void) __attribute__ ((weak, alias ("default_isr")));  // FTM2 fault, overflow and channels interrupt
void Reserved61_irq_isr    (void) __attribute__ ((weak, alias ("default_isr")));  // Reserved interrupt 61
void Reserved62_irq_isr    (void) __attribute__ ((weak, alias ("default_isr")));  // Reserved interrupt 62
void Reserved63_irq_isr    (void) __attribute__ ((weak, alias ("default_isr")));  // Reserved interrupt 63
void PIT0_irq_isr          (void) __attribute__ ((weak, alias ("default_isr")));  // PIT timer channel 0 interrupt
void PIT1_irq_isr          (void) __attribute__ ((weak, alias ("default_isr")));  // PIT timer channel 1 interrupt
void PIT2_irq_isr          (void) __attribute__ ((weak, alias ("default_isr")));  // PIT timer channel 2 interrupt
void PIT3_irq_isr          (void) __attribute__ ((weak, alias ("default_isr")));  // PIT timer channel 3 interrupt
void PDB0_irq_isr          (void) __attribute__ ((weak, alias ("default_isr")));  // PDB0 Interrupt
void Reserved69_irq_isr    (void) __attribute__ ((weak, alias ("default_isr")));  // Reserved interrupt 69
void Reserved70_irq_isr    (void) __attribute__ ((weak, alias ("default_isr")));  // Reserved interrupt 70
void Reserved71_irq_isr    (void) __attribute__ ((weak, alias ("default_isr")));  // Reserved interrupt 71
void DAC0_irq_isr          (void) __attribute__ ((weak, alias ("default_isr")));  // DAC0 interrupt
void MCG_irq_isr           (void) __attribute__ ((weak, alias ("default_isr")));  // MCG Interrupt
void LPTMR0_irq_isr        (void) __attribute__ ((weak, alias ("default_isr")));  // LPTimer interrupt
void PORTA_irq_isr         (void) __attribute__ ((weak, alias ("default_isr")));  // Port A interrupt
void PORTB_irq_isr         (void) __attribute__ ((weak, alias ("default_isr")));  // Port B interrupt
void PORTC_irq_isr         (void) __attribute__ ((weak, alias ("default_isr")));  // Port C interrupt
void PORTD_irq_isr         (void) __attribute__ ((weak, alias ("default_isr")));  // Port D interrupt
void PORTE_irq_isr         (void) __attribute__ ((weak, alias ("default_isr")));  // Port E interrupt
void SWI_irq_isr           (void) __attribute__ ((weak, alias ("default_isr")));  // Software interrupt
void Reserved81_irq_isr    (void) __attribute__ ((weak, alias ("default_isr")));  // Reserved interrupt 81
void Reserved82_irq_isr    (void) __attribute__ ((weak, alias ("default_isr")));  // Reserved interrupt 82
void Reserved83_irq_isr    (void) __attribute__ ((weak, alias ("default_isr")));  // Reserved interrupt 83
void Reserved84_irq_isr    (void) __attribute__ ((weak, alias ("default_isr")));  // Reserved interrupt 84
void Reserved85_irq_isr    (void) __attribute__ ((weak, alias ("default_isr")));  // Reserved interrupt 85
void Reserved86_irq_isr    (void) __attribute__ ((weak, alias ("default_isr")));  // Reserved interrupt 86
void FTM3_irq_isr          (void) __attribute__ ((weak, alias ("default_isr")));  // FTM3 fault, overflow and channels interrupt
void DAC1_irq_isr          (void) __attribute__ ((weak, alias ("default_isr")));  // DAC1 interrupt
void ADC1_irq_isr          (void) __attribute__ ((weak, alias ("default_isr")));  // ADC1 interrupt
void Reserved90_irq_isr    (void) __attribute__ ((weak, alias ("default_isr")));  // Reserved Interrupt 90
void Reserved91_irq_isr    (void) __attribute__ ((weak, alias ("default_isr")));  // Reserved Interrupt 91
void Reserved92_irq_isr    (void) __attribute__ ((weak, alias ("default_isr")));  // Reserved Interrupt 92
void Reserved93_irq_isr    (void) __attribute__ ((weak, alias ("default_isr")));  // Reserved Interrupt 93
void Reserved94_irq_isr    (void) __attribute__ ((weak, alias ("default_isr")));  // Reserved Interrupt 94
void Reserved95_irq_isr    (void) __attribute__ ((weak, alias ("default_isr")));  // Reserved Interrupt 95
void Reserved96_irq_isr    (void) __attribute__ ((weak, alias ("default_isr")));  // Reserved Interrupt 96
void Reserved97_irq_isr    (void) __attribute__ ((weak, alias ("default_isr")));  // Reserved Interrupt 97
void Reserved98_irq_isr    (void) __attribute__ ((weak, alias ("default_isr")));  // Reserved Interrupt 98
void Reserved99_irq_isr    (void) __attribute__ ((weak, alias ("default_isr")));  // Reserved Interrupt 99
void Reserved100_irq_isr   (void) __attribute__ ((weak, alias ("default_isr")));  // Reserved Interrupt 100
void Reserved101_irq_isr   (void) __attribute__ ((weak, alias ("default_isr")));  // Reserved Interrupt 101

#ifdef __cplusplus
}
#endif

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
  DMA0_irq_isr          ,// DMA Channel 0 Transfer Complete
  DMA1_irq_isr          ,// DMA Channel 1 Transfer Complete
  DMA2_irq_isr          ,// DMA Channel 2 Transfer Complete
  DMA3_irq_isr          ,// DMA Channel 3 Transfer Complete
  DMA4_irq_isr          ,// DMA Channel 4 Transfer Complete
  DMA5_irq_isr          ,// DMA Channel 5 Transfer Complete
  DMA6_irq_isr          ,// DMA Channel 6 Transfer Complete
  DMA7_irq_isr          ,// DMA Channel 7 Transfer Complete
  DMA8_irq_isr          ,// DMA Channel 8 Transfer Complete
  DMA9_irq_isr          ,// DMA Channel 9 Transfer Complete
  DMA10_irq_isr         ,// DMA Channel 10 Transfer Complete
  DMA11_irq_isr         ,// DMA Channel 11 Transfer Complete
  DMA12_irq_isr         ,// DMA Channel 12 Transfer Complete
  DMA13_irq_isr         ,// DMA Channel 13 Transfer Complete
  DMA14_irq_isr         ,// DMA Channel 14 Transfer Complete
  DMA15_irq_isr         ,// DMA Channel 15 Transfer Complete
  DMA_Error_irq_isr     ,// DMA Error Interrupt
  MCM_irq_isr           ,// Normal Interrupt
  FTF_irq_isr           ,// FTFA Command complete interrupt
  Read_Collision_irq_isr,// Read Collision Interrupt
  LVD_LVW_irq_isr       ,// Low Voltage Detect, Low Voltage Warning
  LLWU_irq_isr          ,// Low Leakage Wakeup Unit
  WDOG_EWM_irq_isr      ,// WDOG Interrupt
  RNG_irq_isr           ,// RNG Interrupt
  I2C0_irq_isr          ,// I2C0 interrupt
  I2C1_irq_isr          ,// I2C1 interrupt
  SPI0_irq_isr          ,// SPI0 Interrupt
  SPI1_irq_isr          ,// SPI1 Interrupt
  Reserved44_irq_isr    ,// Reserved Interrupt 44
  Reserved45_irq_isr    ,// Reserved interrupt 45
  LPUART0_irq_isr       ,// LPUART0 status/error interrupt
  UART0_RX_TX_irq_isr   ,// UART0 Receive/Transmit interrupt
  UART0_ERR_irq_isr     ,// UART0 Error interrupt
  UART1_RX_TX_irq_isr   ,// UART1 Receive/Transmit interrupt
  UART1_ERR_irq_isr     ,// UART1 Error interrupt
  UART2_RX_TX_irq_isr   ,// UART2 Receive/Transmit interrupt
  UART2_ERR_irq_isr     ,// UART2 Error interrupt
  Reserved53_irq_isr    ,// Reserved interrupt 53
  Reserved54_irq_isr    ,// Reserved interrupt 54
  ADC0_irq_isr          ,// ADC0 interrupt
  CMP0_irq_isr          ,// CMP0 interrupt
  CMP1_irq_isr          ,// CMP1 interrupt
  FTM0_irq_isr          ,// FTM0 fault, overflow and channels interrupt
  FTM1_irq_isr          ,// FTM1 fault, overflow and channels interrupt
  FTM2_irq_isr          ,// FTM2 fault, overflow and channels interrupt
  Reserved61_irq_isr    ,// Reserved interrupt 61
  Reserved62_irq_isr    ,// Reserved interrupt 62
  Reserved63_irq_isr    ,// Reserved interrupt 63
  PIT0_irq_isr          ,// PIT timer channel 0 interrupt
  PIT1_irq_isr          ,// PIT timer channel 1 interrupt
  PIT2_irq_isr          ,// PIT timer channel 2 interrupt
  PIT3_irq_isr          ,// PIT timer channel 3 interrupt
  PDB0_irq_isr          ,// PDB0 Interrupt
  Reserved69_irq_isr    ,// Reserved interrupt 69
  Reserved70_irq_isr    ,// Reserved interrupt 70
  Reserved71_irq_isr    ,// Reserved interrupt 71
  DAC0_irq_isr          ,// DAC0 interrupt
  MCG_irq_isr           ,// MCG Interrupt
  LPTMR0_irq_isr        ,// LPTimer interrupt
  PORTA_irq_isr         ,// Port A interrupt
  PORTB_irq_isr         ,// Port B interrupt
  PORTC_irq_isr         ,// Port C interrupt
  PORTD_irq_isr         ,// Port D interrupt
  PORTE_irq_isr         ,// Port E interrupt
  SWI_irq_isr           ,// Software interrupt
  Reserved81_irq_isr    ,// Reserved interrupt 81
  Reserved82_irq_isr    ,// Reserved interrupt 82
  Reserved83_irq_isr    ,// Reserved interrupt 83
  Reserved84_irq_isr    ,// Reserved interrupt 84
  Reserved85_irq_isr    ,// Reserved interrupt 85
  Reserved86_irq_isr    ,// Reserved interrupt 86
  FTM3_irq_isr          ,// FTM3 fault, overflow and channels interrupt
  DAC1_irq_isr          ,// DAC1 interrupt
  ADC1_irq_isr          ,// ADC1 interrupt
  Reserved90_irq_isr    ,// Reserved Interrupt 90
  Reserved91_irq_isr    ,// Reserved Interrupt 91
  Reserved92_irq_isr    ,// Reserved Interrupt 92
  Reserved93_irq_isr    ,// Reserved Interrupt 93
  Reserved94_irq_isr    ,// Reserved Interrupt 94
  Reserved95_irq_isr    ,// Reserved Interrupt 95
  Reserved96_irq_isr    ,// Reserved Interrupt 96
  Reserved97_irq_isr    ,// Reserved Interrupt 97
  Reserved98_irq_isr    ,// Reserved Interrupt 98
  Reserved99_irq_isr    ,// Reserved Interrupt 99
  Reserved100_irq_isr   ,// Reserved Interrupt 100
  Reserved101_irq_isr    // Reserved Interrupt 101
};

__attribute__ ((section(".flash_config")))
unsigned long g_flash_config_field_values[]={
  0xFFFFFFFF,                      // 0x400-0x407 Backdoor Comp Key
  0xFFFFFFFF,                      //
  0xFFFFFFFF,                      // 0x408-0x40B Program Flash Protection
  0xFFFFF9FE                       // 0x40C(FSEC)=0xFE, 0x40D(FOPT=0xFD)
                                   // Device is Unsecure, Ez-Port Disabled, NMI Disabled, Normal Boot
};

__attribute__ ((section(".isr_vector_ram")))
void (* const g_isr_vectors[256])(void) = {};

/**
 * \brief  default _isr
 *
 * \param  void
 *
 * \return void
 */
void default_isr(void)
{
  while (1);
}


/**
 * \brief  Sets system dividers
 *
 * \param  void
 *
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

