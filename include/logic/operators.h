//
// Created by maxwell on 2020-04-09.
//

#ifndef METAPARSE_OPERATORS_H
#define METAPARSE_OPERATORS_H

#include <memory>

namespace pram::inline logic
{
  class LogicStatement;

  class LogicalOperator{
    virtual std::unique_ptr<LogicStatement> eval() = 0;
  };

}

#endif // METAPARSE_OPERATORS_H
