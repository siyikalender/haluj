/// \file test_blackboard_spsc.cpp
/// blackboard with the spsc policy driven by two threads, standing in for an
/// interrupt that raises events and the main loop that takes them. Run it
/// under ThreadSanitizer too.
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

#include <cstdint>
#include <thread>

#include "haluj/event_strategy.hpp"

#include "check.hpp"

namespace
{

constexpr std::uint32_t count = 200000;

haluj::strategy::blackboard<std::uint32_t, haluj::spsc> board;

} // namespace

// The producer raises 1, 2, 3... each as soon as the board is free; the
// consumer waits for each one in turn. A side that has to wait yields, so
// the test finishes on a busy machine too. An event lost or seen out of order
// would leave the consumer waiting, and the test would time out.
static void test_handoff()
{
  std::thread producer([]
  {
    for (std::uint32_t event = 1; event <= count; )
    {
      if (board.raise(event))
        ++event;
      else
        std::this_thread::yield();
    }
  });

  std::uint32_t taken = 0;

  for (std::uint32_t expected = 1; expected <= count; ++expected)
  {
    while (!board.test_and_clear(expected))
      std::this_thread::yield();
    ++taken;
  }

  producer.join();

  HALUJ_CHECK(taken == count);
  HALUJ_CHECK(!board.test(count));
}

int main()
{
  test_handoff();
  return haluj_test::report("test_blackboard_spsc");
}
