
#include <cstdint>
#include "vendor.h"

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

void configure_uart(UART_Type& device_, 
                    const uint32_t p_baud_rate)
{
  unsigned sbr;
  unsigned temp;
  unsigned brfa;

  device_.C2 &= (uint8_t) ~(UART_C2_TE_MASK | UART_C2_RE_MASK);

  device_.C1 = 0;

  // Calculate baud settings
  sbr = (unsigned)(SystemCoreClock / (p_baud_rate * 16)); 

  // Save off the current value of the UARTx_BDH except for the SBR field
  temp = device_.BDH & ~(UART_BDH_SBR(0x1F));

  device_.BDH = temp | UART_BDH_SBR(((sbr & 0x1F00) >> 8));
  device_.BDL = (uint8_t)(sbr & UART_BDL_SBR_MASK);

  // Determine if a fractional divider is needed to get closer to the baud rate
  brfa = (((SystemCoreClock * 2)/(p_baud_rate)) - (sbr * 32));

  temp = device_.C4 & ~(UART_C4_BRFA(0x1F));

  device_.C4  = temp |  UART_C4_BRFA(brfa);

  device_.PFIFO |= UART_PFIFO_TXFE_MASK | UART_PFIFO_RXFE_MASK;
  device_.CFIFO = UART_CFIFO_TXFLUSH_MASK | UART_CFIFO_TXFLUSH_MASK;

  device_.TWFIFO = 7;

  // Enable receiver and transmitter
  device_.C2  |= (UART_C2_TE_MASK | UART_C2_RE_MASK);    
}

} // namespace kinetis

} // namespace arm

} // namespace devices

} // namespace base

} // namespace haluj
