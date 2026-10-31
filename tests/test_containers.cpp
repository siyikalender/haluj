/// \file test_containers.cpp
/// Tests of bounded::vector, ring_buffer, cyclic_index and fragment.
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
#include <type_traits>
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

// The checked push_back refuses an element once the vector is full and
// leaves the storage around it alone
// A vector is a plain value: copied and assigned by the compiler, trivially
// copyable for a trivially copyable T, and not an aggregate
static_assert(std::is_trivially_copyable<haluj::bounded::vector<int, 4>>::value,
              "trivially copyable");
static_assert(!std::is_aggregate<haluj::bounded::vector<int, 4>>::value,
              "not an aggregate");

static void test_bounded_vector_value()
{
  haluj::bounded::vector<int, 4> a;
  a.push_back(1);
  a.push_back(2);

  haluj::bounded::vector<int, 4> b;
  b.push_back(9);

  b = a;
  HALUJ_CHECK(b.size() == 2);
  HALUJ_CHECK(b[0] == 1 && b[1] == 2);

  a[0] = 5;               // b is a copy, not a view
  HALUJ_CHECK(b[0] == 1);

  const haluj::bounded::vector<int, 4> c(b);
  HALUJ_CHECK(c.size() == 2 && c[1] == 2);
}

// Reverse iteration over a const vector, and the iterators of a full vector
static void test_bounded_vector_iterators()
{
  haluj::bounded::vector<int, 4> v;
  for (int i = 1; i <= 4; ++i)
    v.push_back(i);

  const auto& cv = v;
  int expected = 4;
  for (auto it = cv.rbegin(); it != cv.rend(); ++it)
    HALUJ_CHECK(*it == expected--);
  HALUJ_CHECK(expected == 0);

  HALUJ_CHECK(cv.end() - cv.begin() == 4);
  HALUJ_CHECK(cv.end() == cv.data() + 4);
  HALUJ_CHECK(std::accumulate(cv.begin(), cv.end(), 0) == 10);
}

// The checked assign takes any range that fits, including one that fills the
// vector and an empty one, and refuses a longer one without changing the
// vector
static void test_bounded_vector_assign()
{
  const int four[] = {1, 2, 3, 4};
  const int five[] = {1, 2, 3, 4, 5};

  haluj::bounded::vector<int, 4> v;

  HALUJ_CHECK(haluj::bounded::assign(v, std::begin(four), std::end(four)));
  HALUJ_CHECK(v.size() == 4);
  HALUJ_CHECK(v[0] == 1 && v[3] == 4);

  HALUJ_CHECK(!haluj::bounded::assign(v, std::begin(five), std::end(five)));
  HALUJ_CHECK(v.size() == 4);
  HALUJ_CHECK(v[3] == 4);

  HALUJ_CHECK(haluj::bounded::assign(v, std::begin(four), std::begin(four)));
  HALUJ_CHECK(v.empty());
}

static void test_bounded_vector_push_back_full()
{
  struct guarded
  {
    haluj::bounded::vector<int, 2> v;
    int                            guard = 42;
  } g;

  HALUJ_CHECK(haluj::bounded::push_back(g.v, 1));
  HALUJ_CHECK(haluj::bounded::push_back(g.v, 2));
  HALUJ_CHECK(!haluj::bounded::push_back(g.v, 3));
  HALUJ_CHECK(g.v.size() == 2);
  HALUJ_CHECK(g.v[0] == 1 && g.v[1] == 2);
  HALUJ_CHECK(g.guard == 42);
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

// back() is the newest element at every position of the head, including
// the moment it wraps to the start of the storage
static void test_ring_buffer_back()
{
  haluj::ring_buffer<std::array<int, 4>> r;

  for (int i = 1; i <= 4; ++i)
  {
    r.push(i);
    HALUJ_CHECK(r.back() == i);
  }
  HALUJ_CHECK(r.full());
  HALUJ_CHECK(r.back() == 4); // the head has wrapped to 0

  for (int i = 5; i <= 12; ++i)
  {
    r.pop();
    r.push(i);
    HALUJ_CHECK(r.back() == i);
    HALUJ_CHECK(r.front() == i - 3);
  }
}

// The documented contract: push() only when !full(), pop() and front() only
// when !empty(). A caller that keeps to it never loses or reorders elements.
static void test_ring_buffer_contract()
{
  haluj::ring_buffer<std::array<int, 4>> r;

  // A producer offers more than fits: the guard turns away the excess
  int accepted = 0;
  for (int i = 0; i < 10; ++i)
  {
    if (!r.full())
    {
      r.push(i);
      ++accepted;
    }
    HALUJ_CHECK(r.size() + r.remaining() == r.capacity());
  }
  HALUJ_CHECK(accepted == 4);
  HALUJ_CHECK(r.full());
  HALUJ_CHECK(r.remaining() == 0);

  // A consumer drains it in order and stops at empty
  int expected = 0;
  while (!r.empty())
  {
    HALUJ_CHECK(r.front() == expected);
    r.pop();
    ++expected;
  }
  HALUJ_CHECK(expected == 4);
  HALUJ_CHECK(r.size() == 0);
  HALUJ_CHECK(r.remaining() == 4);

  // Producer and consumer at different rates, many times around the storage:
  // every accepted value comes out once, in order
  int next_in  = 0;
  int next_out = 0;
  for (int round = 0; round < 50; ++round)
  {
    for (int k = 0; k < 3; ++k)
    {
      if (r.remaining() > 0)
        r.push(next_in++);
    }

    for (int k = 0; k < 2 && r.size() > 0; ++k)
    {
      HALUJ_CHECK(r.front() == next_out);
      r.pop();
      ++next_out;
    }

    HALUJ_CHECK(r.size() == static_cast<std::size_t>(next_in - next_out));
  }

  while (!r.empty())
  {
    HALUJ_CHECK(r.front() == next_out++);
    r.pop();
  }
  HALUJ_CHECK(next_out == next_in);
}

// Every position and step count of small rings and of rings at the top of
// uint8_t, against arithmetic done in a wider type
template<typename T>
static void check_cyclic_steps(unsigned n)
{
  const T N = static_cast<T>(n);

  for (unsigned v = 0; v < n; ++v)
  {
    for (unsigned size = 0; size <= n; ++size)
    {
      const T up   = haluj::cyclic_increment(T(v), T(size), N);
      const T down = haluj::cyclic_decrement(T(v), T(size), N);

      HALUJ_CHECK(up   == T((v + size) % n));
      HALUJ_CHECK(down == T((v + n - size) % n));
    }

    // size steps at once equals size single steps
    T step = T(v);
    for (unsigned size = 0; size <= n; ++size)
    {
      HALUJ_CHECK(haluj::cyclic_increment(T(v), T(size), N) == step);
      step = haluj::cyclic_increment(step, N);
    }
  }
}

static void test_cyclic_steps()
{
  for (unsigned n = 1; n <= 12; ++n)
  {
    check_cyclic_steps<unsigned>(n);
    check_cyclic_steps<std::uint8_t>(n);
  }

  for (unsigned n = 250; n <= 255; ++n)
    check_cyclic_steps<std::uint8_t>(n);

  // The case that used to return N instead of wrapping to 0
  HALUJ_CHECK(haluj::cyclic_increment(6u, 2u, 8u) == 0u);
}

template<typename T>
static void check_cyclic_distance(unsigned n)
{
  const T N = static_cast<T>(n);

  for (unsigned a = 0; a < n; ++a)
  {
    for (unsigned b = 0; b < n; ++b)
    {
      const T forward  = haluj::cyclic_distance_forward(T(a), T(b), N);
      const T backward = haluj::cyclic_distance_backward(T(a), T(b), N);

      HALUJ_CHECK(forward  == T((b + n - a) % n));
      HALUJ_CHECK(backward == T((a + n - b) % n));

      // Walking the distance gets there
      HALUJ_CHECK(haluj::cyclic_increment(T(a), forward,  N) == T(b));
      HALUJ_CHECK(haluj::cyclic_decrement(T(a), backward, N) == T(b));

      // The two directions are mirror images and go round the ring together
      HALUJ_CHECK(backward == haluj::cyclic_distance_forward(T(b), T(a), N));
      HALUJ_CHECK(unsigned(forward) + unsigned(backward) == (a == b ? 0u : n));
    }
  }
}

static void test_cyclic_distance()
{
  for (unsigned n = 1; n <= 12; ++n)
  {
    check_cyclic_distance<unsigned>(n);
    check_cyclic_distance<std::uint8_t>(n);
  }

  for (unsigned n = 250; n <= 255; ++n)
    check_cyclic_distance<std::uint8_t>(n);

  HALUJ_CHECK(haluj::cyclic_distance_forward(2u, 5u, 8u) == 3u);
  HALUJ_CHECK(haluj::cyclic_distance_forward(5u, 2u, 8u) == 5u);
  HALUJ_CHECK(haluj::cyclic_distance_backward(2u, 5u, 8u) == 5u);
  HALUJ_CHECK(haluj::cyclic_distance_backward(5u, 2u, 8u) == 3u);
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

// cyclic_index is a plain value: copyable and assignable in every way, and
// usable at compile time
namespace
{

using index3 = haluj::cyclic_index<std::uint8_t, 3>;

static_assert(std::is_trivially_copyable<index3>::value, "trivially copyable");
static_assert(sizeof(index3) == sizeof(std::uint8_t), "no overhead");

constexpr index3 advanced(int steps)
{
  index3 i;
  for (int k = 0; k < steps; ++k)
    ++i;
  return i;
}

static_assert(*advanced(0) == 0, "starts at 0");
static_assert(*advanced(4) == 1, "wraps at compile time");
static_assert(haluj::cyclic_increment(6u, 2u, 8u) == 0u, "constexpr steps");
static_assert(haluj::cyclic_distance_forward(6u, 1u, 8u) == 3u, "constexpr distance");

} // namespace

static void test_cyclic_index_value()
{
  index3       i;
  index3       j;
  const index3 k = advanced(2);

  i = j++;                // from a temporary
  HALUJ_CHECK(*i == 0 && *j == 1);

  i = k;                  // from a const object
  HALUJ_CHECK(*i == 2);

  i = index3();           // from a value initialized temporary
  HALUJ_CHECK(*i == 0);

  index3 copy(k);
  HALUJ_CHECK(copy == k);

  i.assign(k);
  HALUJ_CHECK(*i == 2);
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
  test_bounded_vector_push_back_full();
  test_bounded_vector_assign();
  test_bounded_vector_value();
  test_bounded_vector_iterators();
  test_ring_buffer();
  test_ring_buffer_back();
  test_ring_buffer_contract();
  test_cyclic_steps();
  test_cyclic_distance();
  test_cyclic_index();
  test_cyclic_index_value();
  test_fragment();
  return haluj_test::report("test_containers");
}
