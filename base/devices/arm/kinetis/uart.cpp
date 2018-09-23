
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

} // namespace kinetis

} // namespace arm

} // namespace devices

} // namespace base

} // namespace haluj
