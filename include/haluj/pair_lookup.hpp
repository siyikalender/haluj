/// \file pair_lookup.hpp
/// Lookups in both directions over sequences of std::pair, for example a
/// constant table that maps codes to names and names back to codes
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
/// \author Selcuk Iyikalender
/// \date   2018
#ifndef HALUJ_PAIR_LOOKUP_HPP
#define HALUJ_PAIR_LOOKUP_HPP

#include <algorithm>
#include <concepts>
#include <functional>
#include <iterator>
#include <optional>
#include <ranges>
#include <string_view>
#include <utility>

namespace haluj
{

/// Compares two C strings by content
struct c_str_equal_to
{
  constexpr bool operator ()(const char* a, const char* b) const
  {
    return std::string_view(a) == std::string_view(b);
  }
};

/// Matches a value against a half open range [first, second): use it to
/// look a value up in a table whose first elements are ranges
struct in_range
{
  template<typename T>
  constexpr bool operator ()(const std::pair<T, T>& a, const T& b) const
  {
    return (b >= a.first) && (b < a.second);
  }
};

namespace pair_lookup_detail
{

// A range is anything std::ranges::begin and end apply to: a C array,
// std::array, bounded::vector, a standard container...
template<typename Range>
using iterator_t = std::ranges::iterator_t<const Range>;

template<typename Iterator>
using pair_t = typename std::iterator_traits<Iterator>::value_type;

template<typename Iterator>
using first_t = typename pair_t<Iterator>::first_type;

template<typename Iterator>
using second_t = typename pair_t<Iterator>::second_type;

} // namespace pair_lookup_detail

/// An iterator over pairs, such as std::pair or anything with first and
/// second members and first_type and second_type
template<typename Iterator>
concept pair_iterator =
  std::input_iterator<Iterator> &&
  requires(Iterator it)
  {
    typename pair_lookup_detail::first_t<Iterator>;
    typename pair_lookup_detail::second_t<Iterator>;
    it->first;
    it->second;
  };

/// A comparator called as comparator(element, key)
template<typename CompFunc, typename Element, typename Key>
concept pair_comparator = std::predicate<const CompFunc&, const Element&, const Key&>;

// The key may be of another type than the elements it is compared with:
// the default comparator, std::equal_to<>, compares any two types that
// have ==, so an int key finds a uint8_t element. A comparator of one's
// own is called as comparator(element, key).

/// The first pair in [first, last) whose first element matches p_key
template<typename     Key,
         pair_iterator Iterator,
         typename     CompFunc = std::equal_to<> >
  requires pair_comparator<CompFunc, pair_lookup_detail::first_t<Iterator>, Key>
constexpr Iterator
find_in_first(const Key&      p_key,
              Iterator        p_first_it,
              Iterator        p_last_it,
              const CompFunc  p_comparator = CompFunc())
{
  return
    std::find_if(p_first_it, p_last_it,
                 [&](const auto& v){
                   return p_comparator(v.first, p_key);
                 });
}

/// The first pair in [first, last) whose second element matches p_key
template<typename     Key,
         pair_iterator Iterator,
         typename     CompFunc = std::equal_to<> >
  requires pair_comparator<CompFunc, pair_lookup_detail::second_t<Iterator>, Key>
constexpr Iterator
find_in_second(const Key&      p_key,
               Iterator        p_first_it,
               Iterator        p_last_it,
               const CompFunc  p_comparator = CompFunc())
{
  return
    std::find_if(p_first_it, p_last_it,
                 [&](const auto& v){
                   return p_comparator(v.second, p_key);
                 });
}

/// The second element of the first pair whose first element matches p_key,
/// or std::nullopt when there is none
template<typename     Key,
         pair_iterator Iterator,
         typename     CompFunc = std::equal_to<> >
  requires pair_comparator<CompFunc, pair_lookup_detail::first_t<Iterator>, Key>
constexpr std::optional<pair_lookup_detail::second_t<Iterator>>
to_second(const Key&      p_key,
          Iterator        p_first_it,
          Iterator        p_last_it,
          const CompFunc  p_comparator = CompFunc())
{
  std::optional<pair_lookup_detail::second_t<Iterator>> result;

  auto it = find_in_first(p_key, p_first_it, p_last_it, p_comparator);

  if (it != p_last_it)
  {
    result = it->second;
  }

  return result;
}

/// to_second over a whole range
template<typename     Key,
         std::ranges::common_range Range,
         typename     CompFunc = std::equal_to<> >
  requires pair_iterator<pair_lookup_detail::iterator_t<Range>>
constexpr std::optional<pair_lookup_detail::second_t<pair_lookup_detail::iterator_t<Range>>>
to_second(const Key&      p_key,
          const Range&    p_range,
          const CompFunc  p_comparator = CompFunc())
{
  return to_second(p_key,
                   std::ranges::begin(p_range),
                   std::ranges::end(p_range),
                   p_comparator);
}

/// The first element of the first pair whose second element matches p_key,
/// or std::nullopt when there is none
template<typename     Key,
         pair_iterator Iterator,
         typename     CompFunc = std::equal_to<> >
  requires pair_comparator<CompFunc, pair_lookup_detail::second_t<Iterator>, Key>
constexpr std::optional<pair_lookup_detail::first_t<Iterator>>
to_first(const Key&      p_key,
         Iterator        p_first_it,
         Iterator        p_last_it,
         const CompFunc  p_comparator = CompFunc())
{
  std::optional<pair_lookup_detail::first_t<Iterator>> result;

  auto it = find_in_second(p_key, p_first_it, p_last_it, p_comparator);

  if (it != p_last_it)
  {
    result = it->first;
  }

  return result;
}

/// to_first over a whole range
template<typename     Key,
         std::ranges::common_range Range,
         typename     CompFunc = std::equal_to<> >
  requires pair_iterator<pair_lookup_detail::iterator_t<Range>>
constexpr std::optional<pair_lookup_detail::first_t<pair_lookup_detail::iterator_t<Range>>>
to_first(const Key&      p_key,
         const Range&    p_range,
         const CompFunc  p_comparator = CompFunc())
{
  return to_first(p_key,
                  std::ranges::begin(p_range),
                  std::ranges::end(p_range),
                  p_comparator);
}

} // namespace haluj

#endif // HALUJ_PAIR_LOOKUP_HPP
