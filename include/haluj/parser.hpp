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
/// \date   2018

#ifndef HALUJ_PARSER_HPP
#define HALUJ_PARSER_HPP

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

} // namespace parser_detail

/// An empty tag that rules of one's own may derive from. The rules of the
/// library do not: a rule that holds rules all derived from the same empty
/// base cannot place them at the same address, so the empty base
/// optimisation does not apply and every level of a grammar grows.
struct rule
{};

/// Specific character parser rule
/// parses the specified character

struct ch_p
{
  constexpr ch_p(char c)
  : c_(c) {}

  template<typename Iterator>
  constexpr bool accept(Iterator &first, Iterator last) const
  {
    bool result = (first != last && c_ == *first);
    if (result)
    {
      first++;
    }
    return result;
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
template<typename ExprType>
constexpr opt_p<ExprType> opt(const ExprType& expr)
{
  return opt_p<ExprType>(expr);
}

/// Accepts expr as long as it succeeds and consumes input. Used by om and
/// zm: a rule that succeeds without consuming anything (opt, zm, rest at the
/// end of the input) would otherwise be repeated forever.
template<typename ExprType, typename Iterator>
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

template<typename ExprType>
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

template<typename ExprType>
constexpr zm_p<ExprType> zm(const ExprType& expr)
{
  return zm_p<ExprType>(expr);
}

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

template<typename ExprType>
constexpr seq_p<ExprType>
seq(const ExprType& p_a)
{
  return seq_p<ExprType>(p_a);
}

template<typename ExprType, typename ... Types>
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

template<typename ExprType>
constexpr any_p<ExprType>
any(const ExprType& p_a)
{
  return any_p<ExprType>(p_a);
}

template<typename ExprType, typename ... Types>
constexpr any_p<ExprType, Types...>
any(const ExprType& p_a, Types... args)
{
  return any_p<ExprType, Types...>(p_a, args...);
}

//
template<typename ExprType>
struct except_p
{
  constexpr except_p(const ExprType& expr)
  : expr_(expr)
  {}

  template<typename Iterator>
  constexpr bool accept(Iterator &first, Iterator last) const
  {
    while(first != last) 
    {
      Iterator tmp = first;
      if (expr_.accept(tmp, last)) 
      {
        break;
      }
      else
      {
        ++first;
      }
    }
    return true; // except is not a strict parsing rule, so it always returns true;
  }

  [[no_unique_address]] ExprType    expr_;
};

template<typename ExprType>
constexpr except_p<ExprType>
except(const ExprType& p_expr)
{
  return except_p<ExprType>(p_expr);
}

template<typename ExprType, typename EscapeExprType>
struct except_2_p
{
  constexpr except_2_p(const ExprType&        p_expr,
             const EscapeExprType&  p_esc_expr)
  : expr_(p_expr),
    esc_expr_(p_esc_expr)
  {}

  template<typename Iterator>
  constexpr bool accept(Iterator &first, Iterator last) const
  {
    Iterator  initial = first;
    bool      success = false;
    while((esc_expr_.accept(first, last) && expr_.accept(first, last) ) || 
          (!(success = expr_.accept(first, last)) && (first != last) && (++first, true) ) );
    if (!success)
      first = initial;
    return success;
  }

  [[no_unique_address]] ExprType          expr_;
  [[no_unique_address]] EscapeExprType    esc_expr_;
};

template<typename ExprType, typename EscapeExprType>
constexpr except_2_p<ExprType, EscapeExprType>
except_2(const ExprType& p_expr, const EscapeExprType& p_esc_expr)
{
  return except_2_p<ExprType, EscapeExprType>(p_expr, p_esc_expr);
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

template<typename ExprType, typename ActionType = default_action>
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
///   itself, but 120 bytes at -Os, where a level goes through three
///   functions; a 32 character line can then nest 3.8 KB deep. Measure a
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
