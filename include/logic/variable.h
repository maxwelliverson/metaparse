//
// Created by maxwell on 2020-04-11.
//

#ifndef METAPARSE_VARIABLE_H
#define METAPARSE_VARIABLE_H

#include "boolean.h"
#include "meta/rtti.h"
#include "value.h"

#include <string>

namespace pram
{
  inline namespace logic{
    class Variable;
    class BoundVariable;
    class FreeVariable;
  }

  ENABLE_RTTI(
    Variable,
      RTTI_ENUM(Bound, Free);
      RTTI_DYNAMIC_TYPE(Bound, BoundVariable);
      RTTI_DYNAMIC_TYPE(Free, FreeVariable);
  );

  inline namespace logic {
    class Variable : public Value
    {
    protected:
      using VariableKind = RTTIKind<Variable>;

    private:
      const VariableKind Kind;
      const std::string identifier;

    protected:
      explicit Variable(VariableKind kind, std::string id) : Value(ValueKind::Variable), Kind(kind), identifier(std::move(id)) {}

    public:
      [[nodiscard]] const std::string& getIdentifier() const {
        return identifier;
      }
      [[nodiscard]] VariableKind getKind() const {
        return Kind;
      };
    };


    class FreeVariable : public Variable
    {
    public:
      explicit FreeVariable(std::string id) : Variable(VariableKind::Free, std::move(id)) {}
    };
    class BoundVariable : public Variable
    {
    public:
      explicit BoundVariable(std::string id) : Variable(VariableKind::Bound, std::move(id)) {}
    };
  }
}

#endif//METAPARSE_VARIABLE_H
