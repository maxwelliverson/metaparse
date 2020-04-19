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

    class IdentityOperator;
    class NotOperator;

    class AndOperator;
    class OrOperator;
    class XorOperator;
    class NorOperator;
    class XnorOperator;
    class NandOperator;
    class ImpliesOperator;

    class AllOperator;
    class AnyOperator;
  }

  ENABLE_RTTI(
    Operator,
      RTTI_ENUM(Unary, Binary, Variadic);
      RTTI_DYNAMIC_TYPE(Unary, UnaryOperator);
      RTTI_DYNAMIC_TYPE(Binary, BinaryOperator);
      RTTI_DYNAMIC_TYPE(Variadic, VariadicOperator);
  );

  ENABLE_RTTI(
    UnaryOperator,
      RTTI_ENUM(Identity, Not);
      RTTI_DYNAMIC_TYPE(Identity, IdentityOperator);
      RTTI_DYNAMIC_TYPE(Not, NotOperator);
  );

  ENABLE_RTTI(
    BinaryOperator,
      RTTI_ENUM(And, Or, Xor, Nor, Xnor, Nand, Implies);
      RTTI_DYNAMIC_TYPE(And, AndOperator);
      RTTI_DYNAMIC_TYPE(Or, OrOperator);
      RTTI_DYNAMIC_TYPE(Xor, XorOperator);
      RTTI_DYNAMIC_TYPE(Nor, NorOperator);
      RTTI_DYNAMIC_TYPE(Xnor, XnorOperator);
      RTTI_DYNAMIC_TYPE(Nand, NandOperator);
      RTTI_DYNAMIC_TYPE(Implies, ImpliesOperator);
  );

  ENABLE_RTTI(
    VariadicOperator,
      RTTI_ENUM(All, Any);
      RTTI_DYNAMIC_TYPE(All, AllOperator);
      RTTI_DYNAMIC_TYPE(Any, AnyOperator);
  );


  inline namespace logic {
    class Operator{
    protected:
      using OperatorKind = RTTIKind<Operator>;
    private:
      const OperatorKind operatorKind;

    public:
      explicit constexpr Operator(OperatorKind opKind) : operatorKind(opKind){}

      [[nodiscard]] OperatorKind getKind() const noexcept {
        return operatorKind;
      }
    };


    class UnaryOperator : public Operator {
    protected:
      using UnaryKind = RTTIKind<UnaryOperator>;
    private:
      const UnaryKind unaryKind;

    public:
      explicit constexpr UnaryOperator(UnaryKind kind) :
        Operator(OperatorKind::Unary), unaryKind(kind){}
    };
    class BinaryOperator : public Operator
    {
    protected:
      using BinaryKind = RTTIKind<BinaryOperator>;
      using BinaryFunction = const ExprNode*(const ExprNode*, const ExprNode*);
    private:
      const BinaryKind binaryKind;
      BinaryFunction * const op;
    public:
      explicit constexpr BinaryOperator(BinaryFunction* op, BinaryKind kind) :
        Operator(OperatorKind::Binary), binaryKind(kind), op(op) {}

      const ExprNode *operator()(const ExprNode *A, const ExprNode *B) const noexcept(noexcept(op(A, B))) {
        return meta::double_dispatch(A, B, op);
      }
    };
    class VariadicOperator : public Operator
    {
    protected:
      using VariadicKind = RTTIKind<VariadicOperator>;
    private:
      const VariadicKind variadicKind;
    public:
      explicit constexpr VariadicOperator(VariadicKind kind) : Operator(OperatorKind::Variadic), variadicKind(kind){}
    };

    class Operation : public IntermediateExprNode {
      
    };
  }
  

}

#endif // METAPARSE_OPERATORS_H
