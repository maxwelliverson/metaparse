//
// Created by maxwell on 2020-04-09.
//

#ifndef METAPARSE_BOOLEAN_H
#define METAPARSE_BOOLEAN_H

#include "meta/metavalue.h"
#include <type_traits>
#include <variant>
#include <optional>
#include <memory>

namespace pram::inline logic
{
  class Boolean
  {
    const bool value;
  public:
    explicit constexpr Boolean(bool B) : value(B){}

    explicit constexpr operator bool() const noexcept
    {
      return value;
    }

    constexpr Boolean operator==(const Boolean& b) const noexcept{return Boolean(value == b.value);}
    constexpr Boolean operator!=(const Boolean& b) const noexcept{return Boolean(value != b.value);}

    constexpr Boolean operator&&(const Boolean& b) const noexcept{return Boolean(value && b.value);}
    constexpr Boolean operator||(const Boolean& b) const noexcept{return Boolean(value || b.value);}
    constexpr Boolean operator !()                 const noexcept{return Boolean(!value);}
  };

  inline constexpr static Boolean True{true};
  inline constexpr static Boolean False{false};

  using TrueType = meta::MetaValue<True>;
  using FalseType = meta::MetaValue<False>;
}

#endif // METAPARSE_BOOLEAN_H
