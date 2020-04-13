//
// Created by maxwell on 2020-04-11.
//

#ifndef METAPARSE_EXPRESSION_H
#define METAPARSE_EXPRESSION_H

#include "boolean.h"
#include "set.h"
#include "variable.h"

namespace pram::inline logic
{
  class Expression
  {
    using Value = std::variant<std::shared_ptr<Expression>>;

    enum
    {
      IDENTITY,
      AND,
      OR,
      NOT,
      NAND,
      NOR,
      XOR,
      IMPLIES
    } operation;



  };

}

#endif//METAPARSE_EXPRESSION_H
