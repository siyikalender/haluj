#ifndef HALUJ_BASE_UTILITY_HPP
#define HALUJ_BASE_UTILITY_HPP

#include <cstdint>

namespace haluj
{

namespace base
{

template<typename T, std::size_t N>
constexpr std::size_t array_size(const T (&arr)[N])
{
  return N;
}

template<typename T>
inline T limit(const T p_value, const T p_lower, const T p_upper)
{
  return (p_value < p_lower) ? p_lower : (
            (p_value > p_upper) ? p_upper : p_value);
}

} // namespace base

} // namespace haluj

#endif
