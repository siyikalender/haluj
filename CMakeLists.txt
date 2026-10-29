# CMake build of the haluj library
#
# This is free and unencumbered software released into the public domain.
# For more information, please refer to <http://unlicense.org> and the
# LICENSE file in the root of this repository.
#
# Author: Claude Opus 5.5 <noreply@anthropic.com>
# Date:   2026

cmake_minimum_required(VERSION 3.14)

project(haluj LANGUAGES CXX)

include(GNUInstallDirs)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  set(HALUJ_IS_TOP_LEVEL ON)
else()
  set(HALUJ_IS_TOP_LEVEL OFF)
endif()

option(HALUJ_BUILD_TESTS          "Build the haluj tests"                ${HALUJ_IS_TOP_LEVEL})
option(HALUJ_WARNINGS_AS_ERRORS   "Treat warnings in tests as errors"    OFF)
option(HALUJ_FETCH_BIT            "Download the bit library if it is not found" ON)

# Dependency: the bit library (https://github.com/siyikalender/bit), used by
# ring_buffer.hpp. An existing bit::bit target or an installed package is
# preferred; otherwise a pinned commit is downloaded.
if(NOT TARGET bit::bit)
  find_package(bit CONFIG QUIET)
endif()

if(NOT TARGET bit::bit)
  if(NOT HALUJ_FETCH_BIT)
    message(FATAL_ERROR
      "haluj needs the bit library: provide a bit::bit target, install bit, "
      "or turn HALUJ_FETCH_BIT on")
  endif()

  include(FetchContent)
  FetchContent_Declare(bit
    GIT_REPOSITORY https://github.com/siyikalender/bit.git
    GIT_TAG        a6a60919b1a0d82aef380d40ca995380433de68d)

  set(BIT_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
  set(BIT_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  set(BIT_INSTALL        OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(bit)
endif()

# Header only library: haluj::haluj
add_library(haluj INTERFACE)
add_library(haluj::haluj ALIAS haluj)

target_include_directories(haluj
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# state_machine.hpp uses auto template parameters
target_compile_features(haluj INTERFACE cxx_std_17)
target_link_libraries(haluj INTERFACE bit::bit)

if(HALUJ_BUILD_TESTS)
  # Warning flags for the code in this repository (tests only)
  add_library(haluj_warnings INTERFACE)

  if(MSVC)
    target_compile_options(haluj_warnings INTERFACE /W4 /permissive-)
    if(HALUJ_WARNINGS_AS_ERRORS)
      target_compile_options(haluj_warnings INTERFACE /WX)
    endif()
  else()
    target_compile_options(haluj_warnings INTERFACE -Wall -Wextra -Wpedantic)
    if(HALUJ_WARNINGS_AS_ERRORS)
      target_compile_options(haluj_warnings INTERFACE -Werror)
    endif()
  endif()

  enable_testing()
  add_subdirectory(tests)
endif()
