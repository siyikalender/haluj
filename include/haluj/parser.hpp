/// \file parser.hpp
/// A lightweight recursive descent parser, that does not paralyze the compiler
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
/// \author Claude Opus 5.5 <noreply@anthropic.com>
/// \date   2018

#ifndef HALUJ_PARSER_HPP
#define HALUJ_PARSER_HPP

#include <algorithm>
#include <bit>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <type_traits>
#include <utility>

namespace haluj
{

namespace parser_detail
{

// Character classes of the "C" locale, as in <cctype>, but constexpr and
// defined for every char value: <cctype> takes an int that must be an
// unsigned char value or EOF, so a char above 127 on a target where char is
// signed, a byte received from a serial line for example, was undefined.

constexpr bool is_upper(char c)  { return c >= 'A' && c <= 'Z'; }
constexpr bool is_lower(char c)  { return c >= 'a' && c <= 'z'; }
constexpr bool is_digit(char c)  { return c >= '0' && c <= '9'; }
constexpr bool is_alpha(char c)  { return is_upper(c) || is_lower(c); }
constexpr bool is_alnum(char c)  { return is_alpha(c) || is_digit(c); }

constexpr bool is_xdigit(char c)
{
  return is_digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

/// space, \t, \n, \v, \f and \r
constexpr bool is_space(char c)  { return c == ' ' || (c >= '\t' && c <= '\r'); }

/// An element as an unsigned byte: char, signed char, unsigned char,
/// std::uint8_t or std::byte
template<typename Element>
constexpr std::uint8_t to_byte(const Element& e)
{
  return static_cast<std::uint8_t>(e);
}

} // namespace parser_detail

/// An empty tag that rules of one's own may derive from. The rules of the
/// library do not: a rule that holds rules all derived from the same empty
/// base cannot place them at the same address, so the empty base
/// optimisation does not apply and every level of a grammar grows.
struct rule
{};

/// What every rule provides: accept(first, last) tries the rule at first,
/// moves first past what it matched and tells whether it matched. A rule is
/// a template over the iterator; it is checked here with const char*, so a
/// rule must at least work on plain character arrays. The combinators take
/// parser_rule arguments, so a wrong argument is reported where the rule is
/// built.
template<typename Rule>
concept parser_rule = requires(const Rule rule, const char*& first, const char* last)
{
  { rule.accept(first, last) } -> std::same_as<bool>;
};

/// Specific character parser rule
/// parses the specified character

struct ch_p
{
  constexpr ch_p(char c)
  : c_(c) {}

  template<typename Iterator>
  constexpr bool accept(Iterator &first, Iterator last) const
  {
    bool result = (first != last && equal(*first));
    if (result)
    {
      first++;
    }
    return result;
  }

  // One byte elements compare as unsigned bytes: with a signed char, char
  // 0x81 is -127 and would never equal a std::uint8_t 0x81 of binary data
  template<typename Element>
  constexpr bool equal(const Element& e) const
  {
    if constexpr (sizeof(Element) == 1)
      return parser_detail::to_byte(e) == parser_detail::to_byte(c_);
    else
      return e == c_;
  }

  char c_;
};

/// Creates a specific character parser rule
constexpr ch_p ch(const char c)
{
  return ch_p(c);
}

/// Alphabetic parser rule
/// parses a single alphabethic character defined in C locale
struct alpha_p
{
  constexpr alpha_p()
  {}

  template<typename Iterator>
  constexpr bool accept(Iterator &first, Iterator last) const
  {
    bool result = (first != last && parser_detail::is_alpha(*first));
    if (result) first++;
    return result;
  }

};

/// Creates an alphabetic parser rule
constexpr alpha_p alpha()
{
  return alpha_p();
}

/// Alphanumeric parser rule
/// parses a single alphanumeric character defined in C locale
struct alnum_p
{
  constexpr alnum_p()
  {}

  template<typename Iterator>
  constexpr bool accept(Iterator &first, Iterator last) const
  {
    bool result = (first != last && parser_detail::is_alnum(*first));
    if (result) first++;
    return result;
  }

};

/// Creates an alphanumeric parser rule
constexpr alnum_p alnum()
{
  return alnum_p();
}

/// Whitespace parser rule (including eol)
struct space_p
{
  constexpr space_p()
  {}

  template<typename Iterator>
  constexpr bool accept(Iterator &first, Iterator last) const
  {
    bool result = (first != last && parser_detail::is_space(*first));
    if (result)
    {
      first++;
    }
    return result;
  }

};

/// Creates a whitespace parser rule
constexpr space_p space()
{
  return space_p();
}

/// Single digit parser rule
/// parses a single digit
struct digit_p
{
  constexpr digit_p()
  {}

  template<typename Iterator>
  constexpr bool accept(Iterator &first, Iterator last) const
  {
    bool result = (first != last && parser_detail::is_digit(*first));
    if (result)
    {
      first++;
    }
    return result;
  }

};

/// Creates a single digit parser rule
constexpr digit_p digit()
{
  return digit_p();
}

/// Single hexadecimal digit parser rule
/// parses a single digit
struct hex_digit_p
{
  constexpr hex_digit_p()
  {}

  template<typename Iterator>
  constexpr bool accept(Iterator &first, Iterator last) const
  {
    bool result = (first != last && parser_detail::is_xdigit(*first));
    if (result)
    {
      first++;
    }
    return result;
  }

};

/// Creates a single hexadecimal digit parser rule
constexpr hex_digit_p hex_digit()
{
  return hex_digit_p();
}

/// Single octal digit parser rule
/// parses a single octal digit

struct oct_digit_p
{
  constexpr oct_digit_p()
  {}

  template<typename Iterator>
  constexpr bool accept(Iterator &first, Iterator last) const
  {
    bool result = (first != last && (*first) >= '0' && (*first) <='7');
    if (result)
    {
      first++;
    }
    return result;
  }

};

/// Creates a single octal parser rule
constexpr oct_digit_p oct_digit()
{
  return oct_digit_p();
}

/// Single binary digit parser rule
/// parses a single binary digit

struct bin_digit_p
{
  constexpr bin_digit_p()
  {}

  template<typename Iterator>
  constexpr bool accept(Iterator &first, Iterator last) const
  {
    bool result = (first != last && (*first) >= '0' && (*first) <='1');
    if (result)
    {
      first++;
    }
    return result;
  }

};

/// Creates a single binary parser rule
constexpr bin_digit_p bin_digit()
{
  return bin_digit_p();
}

/// Upper case parser rule
/// parses a single upper case alpha character defined in C locale
struct upper_p
{
  constexpr upper_p()
  {}

  template<typename Iterator>
  constexpr bool accept(Iterator &first, Iterator last) const
  {
    bool result = (first != last && parser_detail::is_upper(*first));
    if (result) first++;
    return result;
  }

};

/// Creates an upper case parser rule
constexpr upper_p upper()
{
  return upper_p();
}

/// rest parser rule
/// consumes all remaining characters to end of the range
/// typical this rule is useful for parsing line comments
struct rest_p
{
  constexpr rest_p()
  {}

  template<typename Iterator>
  constexpr bool accept(Iterator &first, Iterator last) const
  {
    while(first != last) first++;
    return true;
  }

};

/// Creates a rest parser rule
constexpr rest_p rest()
{
  return rest_p();
}

/// Any single element: a character of text or a byte of binary data. With
/// nt() and vt() it reads a field of a fixed or a given length.
struct anychar_p
{
  template<typename Iterator>
  constexpr bool accept(Iterator &first, Iterator last) const
  {
    bool result = (first != last);
    if (result)
    {
      first++;
    }
    return result;
  }
};

/// Creates a rule that accepts any single element
constexpr anychar_p anychar()
{
  return anychar_p();
}

// ---- Binary data ------------------------------------------------------------
//
// Rules for binary formats such as protocol headers. The input is a range of
// bytes (std::uint8_t, unsigned char, char, std::byte), each compared as an
// unsigned byte. Fields of a given length are nt(n, anychar()); the actions
// below take their values out:
//
//   std::uint16_t      port;
//   std::array<std::uint8_t, 4> address;
//
//   seq(be16(0x0800),                                     // a word of a fixed value
//       u8(4),                                            // a byte of a fixed value
//       action(nt(2, anychar()), store_be(port)),         // any word, kept
//       action(nt(4, anychar()), store_bytes(address)),   // any 4 bytes, kept
//       nt(6, anychar()))                                 // 6 bytes skipped

/// An unsigned number of a fixed value in the given byte order
template<std::unsigned_integral ValueType, std::endian Order>
struct word_p
{
  constexpr word_p(ValueType value)
  : value_(value) {}

  template<typename Iterator>
  constexpr bool accept(Iterator &first, Iterator last) const
  {
    Iterator at = first;

    for (std::size_t i = 0; i < sizeof(ValueType); ++i, ++at)
    {
      const std::size_t byte  = (Order == std::endian::big) ? sizeof(ValueType) - 1 - i : i;
      const auto        value = static_cast<std::uint8_t>(value_ >> (8 * byte));

      if (at == last || parser_detail::to_byte(*at) != value)
        return false;
    }

    first = at;
    return true;
  }

  ValueType value_;
};

/// A byte of the given value
constexpr word_p<std::uint8_t, std::endian::big> u8(std::uint8_t value)
{
  return {value};
}

/// A 16 / 32 bit number of the given value, big endian (network order)
constexpr word_p<std::uint16_t, std::endian::big> be16(std::uint16_t value)
{
  return {value};
}

constexpr word_p<std::uint32_t, std::endian::big> be32(std::uint32_t value)
{
  return {value};
}

/// A 16 / 32 bit number of the given value, little endian
constexpr word_p<std::uint16_t, std::endian::little> le16(std::uint16_t value)
{
  return {value};
}

constexpr word_p<std::uint32_t, std::endian::little> le32(std::uint32_t value)
{
  return {value};
}

/// An action storing the matched bytes in out as a big endian number
template<std::unsigned_integral ValueType>
constexpr auto store_be(ValueType& out)
{
  return [&out](auto first, auto last)
  {
    ValueType value = 0;
    for (; first != last; ++first)
      value = static_cast<ValueType>((value << 8) | parser_detail::to_byte(*first));
    out = value;
  };
}

/// An action storing the matched bytes in out as a little endian number
template<std::unsigned_integral ValueType>
constexpr auto store_le(ValueType& out)
{
  return [&out](auto first, auto last)
  {
    ValueType   value = 0;
    std::size_t shift = 0;
    for (; first != last; ++first, shift += 8)
      value = static_cast<ValueType>(value | (ValueType(parser_detail::to_byte(*first)) << shift));
    out = value;
  };
}

/// An action copying the matched bytes to out, a container at least as long
/// as the field (a std::array for a fixed length field)
template<typename Container>
constexpr auto store_bytes(Container& out)
{
  return [&out](auto first, auto last)
  {
    std::transform(first, last, std::begin(out),
                   [](const auto& e) { return static_cast<std::remove_cvref_t<decltype(*std::begin(out))>>(parser_detail::to_byte(e)); });
  };
}

/// optional expression parser rule
/// Always true regardless the return value of accept method of the expr
template<typename ExprType>
struct opt_p
{
  constexpr opt_p(const ExprType& expr)
  : expr_(expr)
  {}

  template<typename Iterator>
  constexpr bool accept(Iterator &first, Iterator last) const
  {
    expr_.accept(first, last);
    return true;
  }

  [[no_unique_address]] ExprType    expr_;
};

/// Creates and oprional expression parser rule
template<parser_rule ExprType>
constexpr opt_p<ExprType> opt(const ExprType& expr)
{
  return opt_p<ExprType>(expr);
}

/// Accepts expr as long as it succeeds and consumes input. Used by om and
/// zm: a rule that succeeds without consuming anything (opt, zm, rest at the
/// end of the input) would otherwise be repeated forever.
// Matches expr exactly n times, or restores first and fails
template<typename ExprType, typename IntegerType, typename Iterator>
constexpr bool repeat_n_(const ExprType& expr, const IntegerType n,
                         Iterator& first, Iterator last)
{
  // n elements of anything: with a random access iterator one bounds check
  // for the whole field, not one per element (a binary field of n bytes)
  if constexpr (std::same_as<ExprType, anychar_p> && std::random_access_iterator<Iterator>)
  {
    if (!std::cmp_greater(n, 0))
      return true;
    if (std::cmp_less(last - first, n))
      return false;
    first += static_cast<std::iter_difference_t<Iterator>>(n);
    return true;
  }

  const Iterator initial = first;

  for (IntegerType i = 0; i < n; ++i)
  {
    if (!expr.accept(first, last))
    {
      first = initial;
      return false;
    }
  }

  return true;
}

template<parser_rule ExprType, typename Iterator>
constexpr void repeat_(const ExprType& expr, Iterator& first, Iterator last)
{
  for (;;)
  {
    Iterator before = first;
    if (!expr.accept(first, last) || first == before)
    {
      break;
    }
  }
}

/// One and more expression parser rule
/// At least one times the 
template<typename ExprType>
struct om_p
{
  constexpr om_p(const ExprType& expr)
  : expr_(expr)
  {}

  template<typename Iterator>
  constexpr bool accept(Iterator &first, Iterator last) const
  {
    bool result = false;
    if (expr_.accept(first, last))
    {
      result = true;
      repeat_(expr_, first, last);
    }
    return result;
  }

  [[no_unique_address]] ExprType expr_;
};

template<parser_rule ExprType>
constexpr om_p<ExprType> om(const ExprType& p_expr)
{
  return om_p<ExprType>(p_expr);
}

template<typename ExprType>
struct zm_p
{
  constexpr zm_p(const ExprType& expr)
  : expr_(expr)
  {}

  template<typename Iterator>
  constexpr bool accept(Iterator &first, Iterator last) const
  {
    bool result = true;
    repeat_(expr_, first, last);
    return result;
  }

  [[no_unique_address]] ExprType expr_;
};

template<parser_rule ExprType>
constexpr zm_p<ExprType> zm(const ExprType& expr)
{
  return zm_p<ExprType>(expr);
}

/// The rule exactly N times, N fixed when the rule is built: a field of a
/// fixed length in a binary format, for example. Fails, and consumes
/// nothing, when the rule does not match N times; a count of 0 or less
/// always matches.
///
///   nt(4, anychar())          a 4 byte field
///   nt(2, hex_digit())        two hexadecimal digits
template<typename ExprType, std::integral IntegerType>
struct nt_p
{
  constexpr nt_p(const IntegerType N, const ExprType& expr)
  : N_(N),
    expr_(expr)
  {}

  template<typename Iterator>
  constexpr bool accept(Iterator &first, Iterator last) const
  {
    return repeat_n_(expr_, N_, first, last);
  }

  IntegerType                         N_;
  [[no_unique_address]] ExprType      expr_;
};

/// Creates a rule that matches expr exactly N times
template<parser_rule ExprType, std::integral IntegerType>
constexpr nt_p<ExprType, IntegerType> nt(const IntegerType N, const ExprType& expr)
{
  return nt_p<ExprType, IntegerType>(N, expr);
}

/// The rule exactly N times, N read from a variable each time the rule
/// runs: a field whose length an earlier field gave, for example. The rule
/// keeps a reference to the variable, so the variable must outlive the
/// rule; a temporary is refused at compile time.
///
///   std::uint8_t length = 0;
///   auto packet = seq(action(anychar(), [&](auto p, auto) { length = std::uint8_t(*p); }),
///                     vt(length, anychar()));
///
/// Fails, and consumes nothing, when the rule does not match N times.
template<typename ExprType, std::integral IntegerType>
struct vt_p
{
  constexpr vt_p(const IntegerType& N, const ExprType& expr)
  : N_(N),
    expr_(expr)
  {}

  template<typename Iterator>
  constexpr bool accept(Iterator &first, Iterator last) const
  {
    return repeat_n_(expr_, N_, first, last);
  }

  const IntegerType&                  N_;
  [[no_unique_address]] ExprType      expr_;
};

/// Creates a rule that matches expr as many times as N holds when it runs
template<parser_rule ExprType, std::integral IntegerType>
constexpr vt_p<ExprType, IntegerType> vt(const IntegerType& N, const ExprType& expr)
{
  return vt_p<ExprType, IntegerType>(N, expr);
}

/// A temporary count would be destroyed before the rule runs
template<parser_rule ExprType, std::integral IntegerType>
void vt(const IntegerType&& N, const ExprType& expr) = delete;

template<typename ... Types>
struct seq_p
{
  template<typename Iterator>
  constexpr bool accept(Iterator &, Iterator) const
  {
    return true;
  }
};

template<typename ExprType, typename ... Types>
struct seq_p<ExprType, Types...>
{
  typedef seq_p<Types...> next;

  constexpr seq_p(const ExprType& p_expr_, Types... args)
  : expr_(p_expr_), next_(args...)
  {}

  template<typename Iterator>
  constexpr bool accept(Iterator &first, Iterator last) const
  {
    Iterator initial = first;
    bool result = (expr_.accept(first, last) && next_.accept(first, last));
    if (!result)
      first = initial;
    return result;
  }

  [[no_unique_address]] ExprType  expr_;
  [[no_unique_address]] next      next_;
};

// The last rule of a sequence. It holds no empty seq_p<> after it: two
// nested sequences would each end with one, and two members of the same
// empty type cannot share an address, so each would cost a byte.
template<typename ExprType>
struct seq_p<ExprType>
{
  constexpr seq_p(const ExprType& p_expr_)
  : expr_(p_expr_)
  {}

  template<typename Iterator>
  constexpr bool accept(Iterator &first, Iterator last) const
  {
    Iterator initial = first;
    bool result = expr_.accept(first, last);
    if (!result)
      first = initial;
    return result;
  }

  [[no_unique_address]] ExprType  expr_;
};

template<parser_rule ExprType>
constexpr seq_p<ExprType>
seq(const ExprType& p_a)
{
  return seq_p<ExprType>(p_a);
}

template<parser_rule ExprType, parser_rule ... Types>
constexpr seq_p<ExprType, Types...>
seq(const ExprType& p_a, Types... args)
{
  return seq_p<ExprType, Types...>(p_a, args...);
}


template<typename ... Types>
struct any_p
{
  template<typename Iterator>
  constexpr bool accept(Iterator &, Iterator) const
  {
    return false;
  }
};

template<typename ExprType, typename ... Types>
struct any_p<ExprType, Types...>
{
  typedef any_p<Types...> next;

  constexpr any_p(const ExprType& p_expr_, Types... args)
  : expr_(p_expr_), next_(args...)
  {}

  template<typename Iterator>
  constexpr bool accept(Iterator &first, Iterator last) const
  {
    // Iterator  initial = first;
    bool result = (expr_.accept(first, last) || next_.accept(first, last));
    return result;
  }

  [[no_unique_address]] ExprType  expr_;
  [[no_unique_address]] next      next_;
};

// The last alternative, without an empty any_p<> after it (see seq_p)
template<typename ExprType>
struct any_p<ExprType>
{
  constexpr any_p(const ExprType& p_expr_)
  : expr_(p_expr_)
  {}

  template<typename Iterator>
  constexpr bool accept(Iterator &first, Iterator last) const
  {
    return expr_.accept(first, last);
  }

  [[no_unique_address]] ExprType  expr_;
};

template<parser_rule ExprType>
constexpr any_p<ExprType>
any(const ExprType& p_a)
{
  return any_p<ExprType>(p_a);
}

template<parser_rule ExprType, parser_rule ... Types>
constexpr any_p<ExprType, Types...>
any(const ExprType& p_a, Types... args)
{
  return any_p<ExprType, Types...>(p_a, args...);
}

namespace parser_detail
{

// A rule that never matches: the escape of an except without one
struct never_p
{
  template<typename Iterator>
  constexpr bool accept(Iterator &, Iterator) const
  {
    return false;
  }
};

} // namespace parser_detail

/// Text up to and including a terminator, such as the rest of a quoted
/// string after its opening quote. It fails, and consumes nothing, when the
/// input ends before the terminator.
///
/// An escape followed by the terminator or by another escape is a pair that
/// stands for that character, so it does not end the text: with escape '\',
/// \" is a quote inside the text and \\ a backslash. An escape followed by
/// anything else is an ordinary character. This covers both usual styles:
///
///   except(ch('"'), ch('\\'))     "a \"quoted\" word"   C style
///   except(ch('\''), ch('\''))    'it''s'               SQL style, the
///                                                       quote doubled
///
/// The terminator may be any rule, for example seq(ch('*'), ch('/')) for the
/// end of a comment.
template<typename TerminatorType, typename EscapeType>
struct except_p
{
  constexpr except_p(const TerminatorType& p_terminator,
                     const EscapeType&     p_escape)
  : terminator_(p_terminator),
    escape_(p_escape)
  {}

  template<typename Iterator>
  constexpr bool accept(Iterator &first, Iterator last) const
  {
    const Iterator initial = first;

    for (;;)
    {
      // An escape pair: skip both. An escape that matches empty input is no
      // escape: it would take the terminator after it, or loop forever.
      const Iterator at = first;
      if (escape_.accept(first, last) && first != at)
      {
        if (terminator_.accept(first, last) || escape_.accept(first, last))
        {
          continue;
        }
      }
      first = at;

      if (terminator_.accept(first, last))
      {
        return true;
      }

      if (first == last)
      {
        first = initial;
        return false;
      }

      ++first;
    }
  }

  [[no_unique_address]] TerminatorType  terminator_;
  [[no_unique_address]] EscapeType      escape_;
};

/// Text up to and including the terminator, with an escape (see except_p)
template<parser_rule TerminatorType, parser_rule EscapeType>
constexpr except_p<TerminatorType, EscapeType>
except(const TerminatorType& p_terminator, const EscapeType& p_escape)
{
  return except_p<TerminatorType, EscapeType>(p_terminator, p_escape);
}

/// Text up to and including the terminator, without an escape
template<parser_rule TerminatorType>
constexpr except_p<TerminatorType, parser_detail::never_p>
except(const TerminatorType& p_terminator)
{
  return except_p<TerminatorType, parser_detail::never_p>(p_terminator,
                                                          parser_detail::never_p());
}

/// Everything up to the terminator or to the end of the input, without the
/// terminator: a field of a comma separated line, for example, where the
/// last field has no comma after it. It always matches, possibly nothing.
/// except() is the rule for text that must be terminated.
///
///   seq(action(until(ch(',')), field), zm(seq(ch(','), action(until(ch(',')), field))))
template<typename TerminatorType>
struct until_p
{
  constexpr until_p(const TerminatorType& p_terminator)
  : terminator_(p_terminator)
  {}

  template<typename Iterator>
  constexpr bool accept(Iterator &first, Iterator last) const
  {
    while (first != last)
    {
      Iterator at = first;
      if (terminator_.accept(at, last))
      {
        break;
      }
      ++first;
    }
    return true;
  }

  [[no_unique_address]] TerminatorType  terminator_;
};

/// Creates a rule that reads up to the terminator or to the end
template<parser_rule TerminatorType>
constexpr until_p<TerminatorType> until(const TerminatorType& p_terminator)
{
  return until_p<TerminatorType>(p_terminator);
}

template<typename ExprType>
struct accept_adaptor_p
{
  constexpr accept_adaptor_p(const ExprType &expr)
  : expr_(expr)
  {}

  template<typename Iterator>
  constexpr bool accept(Iterator &first, Iterator last) const
  {
    return expr_(first, last);
  }

  [[no_unique_address]] ExprType      expr_;
};

template<typename ExprType>
constexpr accept_adaptor_p<ExprType>
accept_adaptor(const ExprType& p_a)
{
  return accept_adaptor_p<ExprType>(p_a);
}

struct default_action
{
  template<typename Iterator>
  constexpr void operator()(Iterator, Iterator) const
  {}
};

template <typename ExprType, typename ActionType = default_action>
struct action_p
{
  constexpr action_p(const ExprType   &p_expr,
           const ActionType &p_action)
  : expr_(p_expr),
    a_(p_action) {}

  template<typename Iterator>
  constexpr bool accept(Iterator &first, Iterator last) const
  {
    Iterator  initial = first;
    bool result = expr_.accept(first, last);
    if (result)
    {
      a_(initial, first);
    }
    return result;
  }

  [[no_unique_address]] ExprType          expr_;
  [[no_unique_address]] const ActionType  a_;
};

template<parser_rule ExprType, typename ActionType = default_action>
constexpr action_p<ExprType, ActionType>
action(const ExprType   &p_expr,
       const ActionType &p_action = ActionType())
{
  return action_p<ExprType, ActionType>(p_expr, p_action);
}
/// A rule that may refer to itself, directly or through other rules.
///
/// A grammar is a nest of rule types, so a rule cannot contain itself. A
/// recursive rule is instead a named, empty type that gets its definition
/// from a static function written after the rules that use it:
///
/// \code {.cpp}
/// // expr   := term   (('+' | '-') term)*
/// // term   := factor (('*' | '/') factor)*
/// // factor := digit+ | '(' expr ')'
/// struct expr : haluj::recursive<expr>
/// {
///   static constexpr auto definition();
/// };
///
/// constexpr auto factor = any(om(digit()), seq(ch('('), expr{}, ch(')')));
/// constexpr auto term   = seq(factor, zm(seq(any(ch('*'), ch('/')), factor)));
///
/// constexpr auto expr::definition()
/// {
///   return seq(term, zm(seq(any(ch('+'), ch('-')), term)));
/// }
/// \endcode
///
/// Two things to keep in mind:
/// - No left recursion. A rule must consume input before it reaches itself
///   again: expr := expr '+' term calls itself without end and overflows the
///   stack. Write it as a repetition, as above.
/// - Every level of nesting in the input is a level of calls on the stack.
///   Without left recursion each level consumes at least one character, so
///   the depth is at most the length of the input: bound the input, for
///   example with the line buffer it is read into, to bound the stack.
///   What a level costs depends on the grammar and on the optimisation:
///   with GCC 13 on Cortex-M4 the expression grammar above takes 24 bytes
///   per level at -O2, where the rules inline into one function calling
///   itself, but 96 bytes at -Os, where a level goes through three
///   functions; a 32 character line can then nest 3 KB deep. Measure a
///   grammar of your own with -fstack-usage.
template<typename Derived>
struct recursive
{
  template<typename Iterator>
  constexpr bool accept(Iterator &first, Iterator last) const
  {
    return Derived::definition().accept(first, last);
  }
};

/*
This rule can be used for Debug purposes.
Due to its iostream and std::string dependencies, it is left commented

template <typename ExprType>
struct info_p
{
  info_p(const ExprType   &p_expr, const std::string& p_name)
  : expr_(p_expr),
    name_(p_name) {}

  template<typename Iterator>
  bool accept(Iterator &first, Iterator last) const
  {
    // Iterator  initial = first;
    std::cout << "rule:" << name_ << "->" << *first << " - remaining"  << std::size_t(last - first)<< std::endl;
    return expr_.accept(first, last);
  }

  ExprType          expr_;
  std::string       name_;
};
template<typename ExprType>
inline info_p<ExprType>
info(const ExprType   &p_expr, const std::string& p_name)
{
  return info_p<ExprType>(p_expr, p_name);
}
*/

} // namespace haluj

// HALUJ_PARSER_HPP
#endif
