#pragma once

#include "meta/rtti.h"

namespace pram
{
  inline namespace logic
  {
    class Value;
    class Variable;
    class Constant;
  }

  ENABLE_RTTI(
    Value,
      RTTI_ENUM(Constant, Variable);
      RTTI_DYNAMIC_TYPE(Constant, logic::Constant);
      RTTI_DYNAMIC_TYPE(Variable, logic::Variable);
  );

  inline namespace logic
  {
    class Value
    {
    protected:
      using ValueKind = RTTIKind<Value>;

    private:
      const ValueKind valueKind;

    public:

      explicit constexpr Value(ValueKind kind) : valueKind(kind){}

      [[nodiscard]] constexpr ValueKind getKind() const noexcept
      {
        return valueKind;
      }
    };
  }
}
