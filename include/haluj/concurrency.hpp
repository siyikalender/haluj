/// \file concurrency.hpp
/// Concurrency policies for the types that one context writes and another
/// reads, such as an interrupt and the main loop
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

#ifndef HALUJ_CONCURRENCY_HPP
#define HALUJ_CONCURRENCY_HPP

#include <atomic>

namespace haluj
{

/// The object is used from one context only, for example only from the main
/// loop. Its shared state is plain variables.
struct single_context
{
  static constexpr bool is_atomic = false;
};

/// Single producer, single consumer: one context produces and another
/// consumes, for example an interrupt that receives data and the main loop
/// that processes it. The shared state is atomics that each side changes
/// in its own way only, so no lock and no interrupt masking is needed.
struct spsc
{
  static constexpr bool is_atomic = true;
};

namespace concurrency_detail
{

template<typename T, bool Atomic>
struct shared
{
  constexpr T load() const  { return value; }
  constexpr void store(T p) { value = p; }

  T value = T();
};

// A store publishes what was written before it (release), and a load sees
// everything written before the store it reads (acquire): data written
// before a store is there for the side that loads what was stored.
//
// Only loads and stores are offered, no read-modify-write, so no lock is
// needed even on cores without atomic read-modify-write instructions such
// as Cortex-M0, where std::atomic is not "always lock free" but an aligned
// word is still loaded and stored in one instruction.
template<typename T>
struct shared<T, true>
{
  T load() const  { return value.load(std::memory_order_acquire); }
  void store(T p) { value.store(p, std::memory_order_release); }

  std::atomic<T> value{T()};
};

} // namespace concurrency_detail

/// A value shared under a concurrency policy: a plain T for single_context,
/// an atomic with acquire loads and release stores for spsc
template<typename T, typename Concurrency>
using shared = concurrency_detail::shared<T, Concurrency::is_atomic>;

} // namespace haluj

#endif // HALUJ_CONCURRENCY_HPP
