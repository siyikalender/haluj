#ifndef HALUJ_BASE_UTILITY_HPP
#define HALUJ_BASE_UTILITY_HPP

#include <cstdint>

template<typename T, std::size_t N>
constexpr std::size_t array_size(const T (&arr)[N])
{
  return N;
}

template<typename T, T lower, T upper>
inline T limit(const T& p_value)
{
  return (p_value < lower) ? lower : (
            (p_value > upper) ? upper : p_value);
}

#endif
