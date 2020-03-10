/**
 ** This file is released under Creative Commons Zero - CC0
 **/

#ifndef KINETIS_CRC_HPP
#define KINETIS_CRC_HPP

#include "peripheral.hpp"
#include "options.hpp"
#include "core.hpp"
#include "bitops.hpp"
#include "specific.hpp"

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
// Should compatible with boost::crc_optimal 

template <typename    Specifier,
          std::size_t Bits, 
          uint32_t    TruncPoly,
          uint32_t    InitRem, 
          uint32_t    FinalXor,
          bool        ReflectIn, 
          bool        ReflectRem>
struct crc : peripheral<Specifier>
{
  static CRC_Type*     crc_addr() {return reinterpret_cast<CRC_Type*>(Specifier::crc_base);}
  
  typedef uint32_t value_type;
/*  
  crc_accelerated()
  {
    static_assert((Bits == 16u) || (Bits == 32u), "CRC: Only 16-Bits and 32-Bits are supported");
    reset();
  }
*/  
  static void reset()
  {
    if (Bits == 16u)
    {
      crc_addr()->CTRL    &=  ~CRC_CTRL_TCRC_MASK;        // 16-Bit
      crc_addr()->GPOLY_ACCESS16BIT.GPOLYL  =   CRC_GPOLY_LOW(TruncPoly);
      crc_addr()->CTRL    |=  CRC_CTRL_WAS_MASK;          // Enable Seed
      crc_addr()->ACCESS16BIT.DATAL   =   CRC_DATAL_DATAL(InitRem);   // Write Seed
      crc_addr()->CTRL    &=  ~CRC_CTRL_WAS_MASK;          // Disable Seed
    }
    else if (Bits == 32u)
    {
      crc_addr()->CTRL    |=  CRC_CTRL_TCRC_MASK;         // 32-Bit
      crc_addr()->GPOLY   =   TruncPoly;
      crc_addr()->CTRL    |=  CRC_CTRL_WAS_MASK;          // Enable Seed
      crc_addr()->DATA    =   InitRem;                    // Write Seed
      crc_addr()->CTRL    &=  ~CRC_CTRL_WAS_MASK;          // Disable Seed
    }
  }
  
  static void process_byte( unsigned char byte )
  {
    CRC_DATALL = byte;
  }

  static void process_block( void const* bytes_begin, void const* bytes_end )
  {
    const unsigned char *first = reinterpret_cast<const unsigned char *>(bytes_begin);
    const unsigned char *last  = reinterpret_cast<const unsigned char *>(bytes_end);
  
    for (;first != last; ++first)
      process_byte(*first);
  }
  
  static void process_bytes( void const* buffer, const std::size_t byte_count )
  {
    process_block(buffer, reinterpret_cast<const unsigned char *>(buffer) + byte_count);
  }
  
  static value_type checksum()
  {
    if (Bits == 16)
    {
      return crc_addr()->ACCESS16BIT.DATAL;
    }
    else
    {
      return crc_addr()->DATA;
    }    
  }
  
};

} // namespace kinetis

} // namespace arm

} // namespace devices

} // namespace base

} // namespace haluj


// KINETIS_CRC_HPP
#endif 
