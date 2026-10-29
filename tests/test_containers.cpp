/// \file test_containers.cpp
/// Tests of bounded::vector, ring_buffer, cyclic_index and fragment.
/// build mode, so the tests also run in release (NDEBUG) builds.
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

/// \author Claude Opus 5.5 <noreply@anthropic.com>
/// \date   2026

#include <array>
#include <cstdint>
#include <iterator> // fragment.hpp does not include it yet
#include <numeric>
#include <vector>

#include "haluj/bounded/vector.hpp"
#include "haluj/cyclic_index.hpp"
#include "haluj/fragment.hpp"
#include "haluj/ring_buffer.hpp"

#include "check.hpp"

static void test_bounded_vector()
{
  haluj::bounded::vector<int, 4> v;

  HALUJ_CHECK(v.empty());
  HALUJ_CHECK(v.capacity() == 4);

  HALUJ_CHECK(haluj::bounded::push_back(v, 1));
  HALUJ_CHECK(haluj::bounded::push_back(v, 2));
  HALUJ_CHECK(v.size() == 2);
  HALUJ_CHECK(v.front() == 1);
  HALUJ_CHECK(v.back() == 2);
  HALUJ_CHECK(v[1] == 2);
  HALUJ_CHECK(*v.data() == 1);

  HALUJ_CHECK(haluj::bounded::resize(v, 4));
  HALUJ_CHECK(v.size() == 4);
  HALUJ_CHECK(v[3] == 0);
  HALUJ_CHECK(!haluj::bounded::resize(v, 5));
  HALUJ_CHECK(v.size() == 4);

  HALUJ_CHECK(std::accumulate(v.begin(), v.end(), 0) == 3);
  HALUJ_CHECK(*v.rbegin() == 0);

  haluj::bounded::vector<int, 4> copy(v);
  HALUJ_CHECK(copy.size() == 4);
  HALUJ_CHECK(copy[1] == 2);

  const int source[] = {7, 8};
  HALUJ_CHECK(haluj::bounded::assign(v, std::begin(source), std::end(source)));
  HALUJ_CHECK(v.size() == 2);
  HALUJ_CHECK(v[0] == 7 && v[1] == 8);

  v.clear();
  HALUJ_CHECK(v.empty());
}

static void test_ring_buffer()
{
  haluj::ring_buffer<std::array<int, 4>> r;

  HALUJ_CHECK(r.empty());
  HALUJ_CHECK(!r.full());
  HALUJ_CHECK(r.capacity() == 4);
  HALUJ_CHECK(r.size() == 0);

  r.push(1);
  r.push(2);
  r.push(3);
  HALUJ_CHECK(r.size() == 3);
  HALUJ_CHECK(r.remaining() == 1);
  HALUJ_CHECK(r.front() == 1);
  HALUJ_CHECK(r.back() == 3);

  r.pop();
  HALUJ_CHECK(r.front() == 2);
  HALUJ_CHECK(r.size() == 2);

  // Wrap around the end of the storage
  r.push(4);
  r.push(5);
  HALUJ_CHECK(r.full());
  HALUJ_CHECK(r.size() == 4);

  for (int expected = 2; expected <= 5; ++expected)
  {
    HALUJ_CHECK(r.front() == expected);
    r.pop();
  }
  HALUJ_CHECK(r.empty());

  r.push(9);
  r.clear();
  HALUJ_CHECK(r.empty());
  HALUJ_CHECK(r.size() == 0);
}

static void test_cyclic_index()
{
  HALUJ_CHECK(haluj::cyclic_increment(2u, 3u) == 0u);
  HALUJ_CHECK(haluj::cyclic_increment(1u, 3u) == 2u);
  HALUJ_CHECK(haluj::cyclic_decrement(0u, 3u) == 2u);
  HALUJ_CHECK(haluj::cyclic_increment(2u, 3u, 8u) == 5u);
  HALUJ_CHECK(haluj::cyclic_decrement(1u, 3u, 8u) == 6u);

  haluj::cyclic_index<std::uint8_t, 3> i;
  HALUJ_CHECK(*i == 0);
  ++i;
  ++i;
  HALUJ_CHECK(*i == 2);
  ++i;
  HALUJ_CHECK(*i == 0);
  --i;
  HALUJ_CHECK(*i == 2);

  auto before = i++;
  HALUJ_CHECK(*before == 2);
  HALUJ_CHECK(*i == 0);
  HALUJ_CHECK(before > i);
  HALUJ_CHECK(before != i);

  i.reset();
  HALUJ_CHECK(*i == 0);
}

static void test_fragment()
{
  std::vector<int> c(10);
  std::iota(c.begin(), c.end(), 0);

  std::size_t sizes[4] = {};
  std::size_t count    = 0;

  for (auto f = haluj::make_fragment(c, 4);
       f && count < 4;
       f = haluj::make_fragment(c, 4, f))
  {
    sizes[count++] = f.size();
  }

  HALUJ_CHECK(count == 3);
  HALUJ_CHECK(sizes[0] == 4 && sizes[1] == 4 && sizes[2] == 2);
}

int main()
{
  test_bounded_vector();
  test_ring_buffer();
  test_cyclic_index();
  test_fragment();
  return haluj_test::report("test_containers");
}
