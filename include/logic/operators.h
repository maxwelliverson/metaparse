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
    class Operation;


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
    class Operator{
    protected:
      using OperatorKind = RTTIKind<Operator>;
    private:
      const OperatorKind operatorKind;

    public:
      explicit constexpr Operator(OperatorKind opKind) : operatorKind(opKind){}
    };


    class UnaryOperator : public Operator {
    public:
      explicit constexpr UnaryOperator() : Operator(OperatorKind::Unary){}
    };
    class BinaryOperator : public Operator
    {
    public:
      explicit constexpr BinaryOperator() : Operator(OperatorKind::Binary) {}
    };
    class VariadicOperator : public Operator
    {
    public:
      explicit constexpr VariadicOperator() : Operator(OperatorKind::Variadic) {}
    };

    class Operation : public IntermediateExprNode {
      
    };
  }
  

}

#endif // METAPARSE_OPERATORS_H
