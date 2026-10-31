/// \file test_ring_buffer_spsc.cpp
/// ring_buffer with the spsc policy driven by two threads, standing in for an
/// interrupt and the main loop. Run it under ThreadSanitizer too.
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
#include <atomic>
#include <cstdint>
#include <thread>

#include "haluj/ring_buffer.hpp"

#include "check.hpp"

// A side that has to wait yields, so the test also finishes when the two
// threads share one core with other tests or run slowly under a sanitizer
namespace
{

constexpr std::uint32_t count = 1000000;

// A capacity that is not a power of two, so the positions wrap at 14
haluj::ring_buffer<std::array<std::uint32_t, 7>, haluj::spsc> buffer;

std::atomic<bool> producer_done{false};

void producer()
{
  for (std::uint32_t value = 0; value < count; )
  {
    if (buffer.try_push(value))
      ++value;
    else
      std::this_thread::yield();
  }
  producer_done.store(true);
}

} // namespace

// Every value arrives once and in order, whatever the interleaving
static void test_try_push_try_pop()
{
  std::thread p(producer);

  std::uint32_t expected = 0;
  std::uint32_t received = 0;
  std::uint32_t errors   = 0;
  std::uint32_t value    = 0;

  while (expected < count)
  {
    if (buffer.try_pop(value))
    {
      if (value != expected)
        ++errors;
      ++expected;
      ++received;
    }
    else
      std::this_thread::yield();
  }

  p.join();

  HALUJ_CHECK(errors == 0);
  HALUJ_CHECK(received == count);
  HALUJ_CHECK(producer_done.load());
  HALUJ_CHECK(buffer.empty());
}

// The same with the unchecked functions behind the documented checks: the
// producer looks at full(), the consumer at empty() and front()
static void test_contract()
{
  std::thread p([]
  {
    for (std::uint32_t value = 0; value < count; )
    {
      if (!buffer.full())
      {
        buffer.push(value);
        ++value;
      }
      else
        std::this_thread::yield();
    }
  });

  std::uint32_t expected = 0;
  std::uint32_t errors   = 0;

  while (expected < count)
  {
    if (!buffer.empty())
    {
      if (buffer.front() != expected)
        ++errors;
      buffer.pop();
      ++expected;
    }
    else
      std::this_thread::yield();
  }

  p.join();

  HALUJ_CHECK(errors == 0);
  HALUJ_CHECK(buffer.empty());
}

int main()
{
  test_try_push_try_pop();
  test_contract();
  return haluj_test::report("test_ring_buffer_spsc");
}
