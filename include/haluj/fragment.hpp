/// \file fragment.hpp
/// Fragments a large sized block to smaller chunks. 
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

/// A fragment is not a container, just an iterator range over part of one.

#ifndef HALUJ_FRAGMENT_HPP
#define HALUJ_FRAGMENT_HPP

#include <cstddef>
#include <iterator>
#include <ranges>

namespace haluj
{

/// [first_, last_): a part of a container, as a pair of iterators. It is a
/// range view (std::ranges::view_interface), so besides begin(), end() and
/// size() it has empty(), front(), an explicit conversion to bool (true when
/// not empty) and, over a random access container, back() and operator[];
/// and it works with the std::ranges algorithms and views.
///
/// It is a plain value: copied and assigned by the compiler.
template<std::forward_iterator Iterator>
struct fragment : std::ranges::view_interface<fragment<Iterator>>
{
  typedef Iterator iterator;

  constexpr fragment() = default;

  constexpr fragment(iterator p_first,
                     iterator p_last)
  : first_(p_first),
    last_(p_last)
  {}

  constexpr std::size_t size() const
  {
    return static_cast<std::size_t>(std::ranges::distance(first_, last_));
  }

  constexpr iterator begin() const
  {
    return first_;
  }

  constexpr iterator end() const
  {
    return last_;
  }

  iterator first_{};
  iterator last_{};
};

/// The end of a fragment of at most p_fragment_size elements starting at
/// p_current, stopping at the end of the container
template<std::ranges::forward_range Container,
         typename Iterator = std::ranges::iterator_t<const Container>>
  requires std::ranges::common_range<const Container>
constexpr Iterator
get_fragment_end_(const Container&   p_container,
                  Iterator           p_current,
                  const std::size_t  p_fragment_size)
{
  return std::ranges::next(p_current,
                           static_cast<std::iter_difference_t<Iterator>>(p_fragment_size),
                           std::ranges::end(p_container));
}

/// The first fragment of a container: its first p_fragment_size elements,
/// or all of them if there are fewer
template<std::ranges::forward_range Container,
         typename Iterator = std::ranges::iterator_t<const Container>>
  requires std::ranges::common_range<const Container>
constexpr fragment<Iterator>
make_fragment(const Container&  p_container,
              const std::size_t p_fragment_size)
{
  const Iterator first = std::ranges::begin(p_container);
  return fragment<Iterator>(first,
                            get_fragment_end_(p_container, first, p_fragment_size));
}

/// The fragment after p_f: empty once the container is used up
///
/// \code {.cpp}
/// for (auto f = haluj::make_fragment(data, 64); f; f = haluj::make_fragment(data, 64, f))
///   send(f.begin(), f.end());
/// \endcode
template<std::ranges::forward_range Container,
         typename Iterator = std::ranges::iterator_t<const Container>>
  requires std::ranges::common_range<const Container>
constexpr fragment<Iterator>
make_fragment(const Container&            p_container,
              const std::size_t           p_fragment_size,
              const fragment<Iterator>&   p_f)
{
  return fragment<Iterator>(p_f.end(),
                            get_fragment_end_(p_container, p_f.end(), p_fragment_size));
}

} // namespace haluj

// A fragment refers to the elements of the container, not to itself: an
// iterator taken from a temporary fragment stays valid
template<std::forward_iterator Iterator>
inline constexpr bool std::ranges::enable_borrowed_range<haluj::fragment<Iterator>> = true;

// HALUJ_FRAGMENT_HPP
#endif
