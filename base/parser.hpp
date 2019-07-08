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

#ifndef HALUJ_BASE_PARSER_HPP
#define HALUJ_BASE_PARSER_HPP

namespace haluj
{

namespace base
{

struct rule
{};

/// Specific character parser rule
/// parses a single alphabethic character defined in C locale

struct ch_p : rule
{
  ch_p(char c)
  : c_(c) {}

  template<typename Iterator>
  bool accept(Iterator &first, Iterator last) const
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

inline ch_p ch(const char c)
{
  return ch_p(c);
}

/// Alphabetic parser rule
/// parses a single alphabethic character defined in C locale

struct alpha_p : rule
{
  alpha_p()
  {}

  template<typename Iterator>
  bool accept(Iterator &first, Iterator last) const
  {
    bool result = (first != last && (isalpha(*first) != 0));
    if (result) first++;
    return result;
  }

};

inline alpha_p alpha()
{
  return alpha_p();
}

/// Alphabetic parser rule
/// parses a single alphabethic character defined in C locale

struct alnum_p : rule
{
  alnum_p()
  {}

  template<typename Iterator>
  bool accept(Iterator &first, Iterator last) const
  {
    bool result = (first != last && (isalnum(*first) != 0));
    if (result) first++;
    return result;
  }

};

inline alnum_p alnum()
{
  return alnum_p();
}

/// Whitespace parser rule (including eol)

struct space_p : rule
{
  space_p()
  {}

  template<typename Iterator>
  bool accept(Iterator &first, Iterator last) const
  {
    bool result = (first != last && (isspace(*first) != 0));
    if (result)
    {
      first++;
    }
    return result;
  }

};

inline space_p space()
{
  return space_p();
}

/// Single digit parser rule
/// parses a single digit

struct digit_p : rule
{
  digit_p()
  {}

  template<typename Iterator>
  bool accept(Iterator &first, Iterator last) const
  {
    bool result = (first != last && (isdigit(*first) != 0));
    if (result)
    {
      first++;
    }
    return result;
  }

};

inline digit_p digit()
{
  return digit_p();
}

/// Single hex digit parser rule
/// parses a single digit

struct hex_digit_p : rule
{
  hex_digit_p()
  {}

  template<typename Iterator>
  bool accept(Iterator &first, Iterator last) const
  {
    bool result = (first != last && (isxdigit(*first) != 0));
    if (result)
    {
      first++;
    }
    return result;
  }

};

inline hex_digit_p hex_digit()
{
  return hex_digit_p();
}

/// Upper case parser rule
/// parses a single upper case alpha character defined in C locale

struct upper_p : rule
{
  upper_p()
  {}

  template<typename Iterator>
  bool accept(Iterator &first, Iterator last) const
  {
    bool result = (first != last && (isupper(*first) != 0));
    if (result) first++;
    return result;
  }

};

inline upper_p upper()
{
  return upper_p();
}

/// rest parser
/// consumes all remaining characters to end of the buffer
/// typical this rule is useful for parsing line comments

struct rest_p : rule
{
  rest_p()
  {}

  template<typename Iterator>
  bool accept(Iterator &first, Iterator last) const
  {
    while(first != last) first++;
    return true;
  }

};

inline rest_p rest()
{
  return rest_p();
}


template<typename ExprType>
struct opt_p : rule
{
  opt_p(const ExprType& expr)
  : expr_(expr)
  {}

  template<typename Iterator>
  bool accept(Iterator &first, Iterator last) const
  {
    expr_.accept(first, last);
    return true;
  }

  ExprType    expr_;
};

template<typename ExprType>
opt_p<ExprType> opt(const ExprType& expr)
{
  return opt_p<ExprType>(expr);
}

template<typename ExprType>
struct om_p : rule
{
  om_p(const ExprType& expr)
  : expr_(expr)
  {}

  template<typename Iterator>
  bool accept(Iterator &first, Iterator last) const
  {
    bool result = false;
    if (expr_.accept(first, last))
    {
      result = true;
      while(expr_.accept(first, last));
    }
    return result;
  }

  ExprType expr_;
};

template<typename ExprType>
om_p<ExprType> om(const ExprType& p_expr)
{
  return om_p<ExprType>(p_expr);
}

template<typename ExprType>
struct zm_p : rule
{
  zm_p(const ExprType& expr)
  : expr_(expr)
  {}

  template<typename Iterator>
  bool accept(Iterator &first, Iterator last) const
  {
    bool result = true;
    while(expr_.accept(first, last));
    return result;
  }

  ExprType expr_;
};

template<typename ExprType>
zm_p<ExprType> zm(const ExprType& expr)
{
  return zm_p<ExprType>(expr);
}

template<typename ... Types>
struct seq_p : rule
{
  template<typename Iterator>
  bool accept(Iterator &first, Iterator last) const
  {
    return true;
  }
};

template<typename ExprType, typename ... Types>
struct seq_p<ExprType, Types...> : rule
{
  typedef seq_p<Types...> next;

  seq_p(const ExprType& p_expr_, Types... args)
  : expr_(p_expr_), next_(args...)
  {}

  template<typename Iterator>
  bool accept(Iterator &first, Iterator last) const
  {
    Iterator initial = first;
    bool result = (expr_.accept(first, last) && next_.accept(first, last));
    if (!result)
      first = initial;
    return result;
  }

  ExprType  expr_;
  next      next_;
};

template<typename ExprType>
seq_p<ExprType>
seq(const ExprType& p_a)
{
  return seq_p<ExprType>(p_a);
}

template<typename ExprType, typename ... Types>
seq_p<ExprType, Types...>
seq(const ExprType& p_a, Types... args)
{
  return seq_p<ExprType, Types...>(p_a, args...);
}


template<typename ... Types>
struct any_p : rule
{
  template<typename Iterator>
  bool accept(Iterator &first, Iterator last) const
  {
    return false;
  }
};

template<typename ExprType, typename ... Types>
struct any_p<ExprType, Types...> : rule
{
  typedef any_p<Types...> next;

  any_p(const ExprType& p_expr_, Types... args)
  : expr_(p_expr_), next_(args...)
  {}

  template<typename Iterator>
  bool accept(Iterator &first, Iterator last) const
  {
    // Iterator  initial = first;
    bool result = (expr_.accept(first, last) || next_.accept(first, last));
    return result;
  }

  ExprType  expr_;
  next      next_;
};

template<typename ExprType>
any_p<ExprType>
any(const ExprType& p_a)
{
  return any_p<ExprType>(p_a);
}

template<typename ExprType, typename ... Types>
any_p<ExprType, Types...>
any(const ExprType& p_a, Types... args)
{
  return any_p<ExprType, Types...>(p_a, args...);
}

//
template<typename ExprType>
struct except_p : rule
{
  except_p(const ExprType& expr)
  : expr_(expr)
  {}

  template<typename Iterator>
  bool accept(Iterator &first, Iterator last) const
  {
    Iterator initial = first;
    while((first != last) && !expr_.accept(first, last)) first++;
    return (first != last) && (first != initial);
  }

  ExprType    expr_;
};

template<typename ExprType>
except_p<ExprType>
except(const ExprType& p_expr)
{
  return except_p<ExprType>(p_expr);
}

template<typename ExprType, typename EscapeExprType>
struct except_2_p : rule
{
  except_2_p(const ExprType&        p_expr, 
             const EscapeExprType&  p_esc_expr)
  : expr_(p_expr),
    esc_expr_(p_esc_expr)
  {}

  template<typename Iterator>
  bool accept(Iterator &first, Iterator last) const
  {
    Iterator  initial = first;
    bool      success = false;
    while((esc_expr_.accept(first, last) && expr_.accept(first, last) ) || 
          (!(success = expr_.accept(first, last)) && (first != last) && (first++ != 0) ) );
    if (!success)
      first = initial;
    return success;
  }

  ExprType          expr_;
  EscapeExprType    esc_expr_;
};

template<typename ExprType, typename EscapeExprType>
except_2_p<ExprType, EscapeExprType>
except_2(const ExprType& p_expr, const EscapeExprType& p_esc_expr)
{
  return except_2_p<ExprType, EscapeExprType>(p_expr, p_esc_expr);
}


template<typename ExprType>
struct accept_adaptor_p : rule
{
  accept_adaptor_p(const ExprType &expr)
  : expr_(expr)
  {}

  template<typename Iterator>
  bool accept(Iterator &first, Iterator last) const
  {
    return expr_(first, last);
  }

  ExprType      expr_;
};

template<typename ExprType>
accept_adaptor_p<ExprType>
accept_adaptor(const ExprType& p_a)
{
  return accept_adaptor_p<ExprType>(p_a);
}

struct default_action
{
  template<typename Iterator>
  void operator()(Iterator first, Iterator last)
  {}

  template<typename Iterator>
  void operator()(Iterator first, Iterator last) const
  {}
};

template <typename ExprType, typename ActionType = default_action>
struct action_p : rule
{
  action_p(const ExprType   &p_expr,
           const ActionType &p_action)
  : expr_(p_expr),
    a_(p_action) {}

  template<typename Iterator>
  bool accept(Iterator &first, Iterator last) const
  {
    Iterator  initial = first;
    bool result = expr_.accept(first, last);
    if (result)
    {
      a_(initial, first);
    }
    return result;
  }

  ExprType          expr_;
  const ActionType  a_;
};

template<typename ExprType, typename ActionType = default_action>
inline action_p<ExprType, ActionType>
action(const ExprType   &p_expr,
       const ActionType &p_action = ActionType())
{
  return action_p<ExprType, ActionType>(p_expr, p_action);
}
/*
This rule can be used for Debug purposes.
Due to its iostream and std::string dependencies, it is left commented

template <typename ExprType>
struct info_p : rule
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

} // namespace base

} // namespace haluj

// HALUJ_BASE_PARSER_HPP
#endif
