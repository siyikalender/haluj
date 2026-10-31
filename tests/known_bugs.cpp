/// \file known_bugs.cpp
/// Regression checks for bugs that are known but not fixed yet.
///
/// Each case checks the CORRECT behaviour and is registered in CMake with
/// WILL_FAIL, so today it fails and ctest reports it as passing. When a bug
/// is fixed its case starts to pass, ctest reports it as failing, and the
/// case should move to the ordinary tests.
///
/// Bugs with undefined behaviour, an endless loop or a build error are not
/// run here, because they may pass silently, hang or break the build. They
/// are listed for tracking (headers that do not build on their own are
/// listed in tests/CMakeLists.txt):
///   - format(int) negates INT_MIN
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

#include <cstdio>
#include <cstring>
#include <string>

#include "haluj/format.hpp"

#include "check.hpp"

// A number that does not fit must not be written as a different number
static void format_int_truncation()
{
  char  text[8] = {};
  char* end     = haluj::format(12345, text, text + 3);
  HALUJ_CHECK(std::string(text, end) != "345");
}

struct known_bug
{
  const char* name;
  void      (*run)();
};

static const known_bug bugs[] =
{
  {"format_int_truncation",         format_int_truncation},
};

int main(int argc, char** argv)
{
  if (argc != 2)
  {
    std::fprintf(stderr, "usage: %s <case>\n", argv[0]);
    // Exit code 0, so that a case misspelled in CMake fails under WILL_FAIL
    return 0;
  }

  for (const known_bug& bug : bugs)
  {
    if (std::strcmp(bug.name, argv[1]) == 0)
    {
      bug.run();
      return haluj_test::report(bug.name);
    }
  }

  std::fprintf(stderr, "unknown case: %s\n", argv[1]);
  return 0;
}
