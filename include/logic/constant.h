#pragma once

#include "value.h"

namespace pram {

  inline namespace logic {
    class IntegralConstant;
    class LogicalConstant;
  }

  ENABLE_RTTI(
    Constant,
      RTTI_ENUM(Integral, Logical);
      RTTI_DYNAMIC_TYPE(Integral, IntegralConstant);
      RTTI_DYNAMIC_TYPE(Logical, LogicalConstant);
  );

  inline namespace logic {

    class Constant : public Value
    {
    protected:
      using ConstantKind = RTTIKind<Constant>;

    private:
      const ConstantKind constantKind;

    public:
      explicit constexpr Constant(ConstantKind kind) : Value(ValueKind::Constant), constantKind(kind) {}

      [[nodiscard]] constexpr ConstantKind getKind() const noexcept {
        return constantKind;
      }
    };

    class IntegralConstant : public Constant {
    public:
      explicit constexpr IntegralConstant() : Constant(ConstantKind::Integral){}
    };
    class LogicalConstant : public Constant {
    public:
      explicit constexpr LogicalConstant() : Constant(ConstantKind::Logical){}
    };
  }

}
