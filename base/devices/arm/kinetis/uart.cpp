
#include "uart.hpp"
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

union word
{
  uint8_t   u8[8];
  uint16_t  u16[4];
  uint32_t  u32[2];
  uint8_t   u64;
};

inline uint32_t
calculate_baud_rate_value(const uint32_t  p_peripheral_clock,
                          const uint32_t  p_baud_rate)
{
  return (p_peripheral_clock / (p_baud_rate * 16U));
}

inline uint32_t 
calculate_fractional_divider_value(const uint32_t   p_peripheral_clock,
                                   const uint32_t   p_baud_rate,
                                   const uint32_t   p_sbr)
{
  return ((p_peripheral_clock * 2)/(p_baud_rate)) - (p_sbr * 32);
}

void configure_uart(UART_Type&      p_device, 
                    const uint32_t  p_peripheral_clock,
                    const uint32_t  p_baud_rate,
                    const uint32_t  p_bit_mode,
                    const uint32_t  p_parity,
                    const uint32_t  p_stop_bits)
{
  uint32_t sbr;
  uint32_t temp;
  uint32_t brfa;

  p_device.C1 = p_bit_mode | p_parity;
  // Calculate baud settings
  sbr = calculate_baud_rate_value(p_peripheral_clock, p_baud_rate); 
  // Save off the current value of the UARTx_BDH except for the SBR field
  // temp = mask_clear(p_device.BDH, UART_BDH_SBR_MASK);
  p_device.BDH = UART_BDH_SBR(sbr >> 8);
  #ifdef UART_BDH_SBNS_SHIFT
  p_device.BDH = bit_assign(p_device.BDH, UART_BDH_SBNS_SHIFT, (p_stop_bits == stop_bits::_2));
  #endif
  p_device.BDL = (uint8_t)(sbr & UART_BDL_SBR_MASK);
  // Determine if a fractional divider is needed to get closer to the baud rate
  brfa = calculate_fractional_divider_value(p_peripheral_clock, p_baud_rate, sbr);

  temp          = mask_clear(p_device.C4, UART_C4_BRFA_MASK);
  p_device.C4   = mask_set(temp, UART_C4_BRFA(brfa));
}

} // namespace kinetis

} // namespace arm

} // namespace devices

} // namespace base

} // namespace haluj
