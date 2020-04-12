//
// Created by maxwell on 2020-04-11.
//

#ifndef METAPARSE_VARIABLE_H
#define METAPARSE_VARIABLE_H

#include "boolean.h"

#include <llvm/ADT/StringSwitch.h>
#include <llvm/Support/Casting.h>

namespace pram::inline logic
{
  class Variable
  {
  public:
    enum VariableKind : int
    {
      Free,
      Bound
    };
  private:
    const VariableKind Kind;
  protected:
    constexpr explicit Variable(VariableKind kind) : Kind(kind){}
  public:

    [[nodiscard]] VariableKind GetKind() const{
      return Kind;
    };
  };


  class FreeVariable : public Variable
  {
  public:
    FreeVariable() : Variable(Free){}

    static bool isclass(const Variable* Var) {
      return Var->GetKind() == Free;
    }
  };
  class BoundVariable : public Variable
  {
  public:
    BoundVariable() : Variable(Bound){}

    static bool isclass(const Variable* Var) {
      return Var->GetKind() == Bound;
    }

  };

}

#endif//METAPARSE_VARIABLE_H
