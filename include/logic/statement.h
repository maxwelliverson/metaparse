//
// Created by maxwell on 2020-04-11.
//

#ifndef METAPARSE_STATEMENT_H
#define METAPARSE_STATEMENT_H

#include "expression.h"

namespace pram
{
  inline namespace logic {
    class Statement
    {


    public:
      virtual Boolean operator()() const noexcept{

      }
    };
  }
}

#endif//METAPARSE_STATEMENT_H
