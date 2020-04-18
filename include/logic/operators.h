//
// Created by maxwell on 2020-04-09.
//

#ifndef METAPARSE_OPERATORS_H
#define METAPARSE_OPERATORS_H

#include <memory>
#include "expression.h"

namespace pram
{
  inline namespace logic
  {
    class Operator;
    class UnaryOperator;
    class BinaryOperator;
    class VariadicOperator;
  }

  ENABLE_RTTI(
    Operator,
      RTTI_ENUM(Unary, Binary, Variadic);
      RTTI_DYNAMIC_TYPE(Unary, UnaryOperator);
      RTTI_DYNAMIC_TYPE(Binary, BinaryOperator);
      RTTI_DYNAMIC_TYPE(Variadic, VariadicOperator);
  );

  inline namespace logic {
    class Operator : public IntermediateExprNode {
    protected:
      using OperatorKind = RTTIKind<Operator>;
    private:
      const OperatorKind operatorKind;

    public:
      explicit constexpr Operator(OperatorKind opKind) : IntermediateExprNode(IntermediateExprKind::Operator), operatorKind(opKind){}
    };


    class UnaryOperator;
    class BinaryOperator;
    class VariadicOperator;
  }
  

}

#endif // METAPARSE_OPERATORS_H
