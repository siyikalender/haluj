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

template<typename ValueType>
struct async_loop
{
  typedef ValueType value_t;

  explicit async_loop(const value_t p_initial = value_t(0))
  {
    initialize(p_initial);
  }
  
  async_loop(const async_loop& p_other) = delete;

  async_loop(async_loop&& p_other) = delete;

  void initialize(const value_t p_initial)
  {
    value_ = p_initial;
  }

  template<typename Condition, 
           typename Update>
  bool operator()(Condition p_condition, 
                  Update    p_update)
  {
    bool result = p_condition(value_);
    if (result)
      value_ = p_update(value_);

    return result;
  }

  value_t   value_;
};

} // namespace base

} // namespace haluj

#endif
