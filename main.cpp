/**
 * \file		main.cpp
 * \author		Maxwell Iverson
 * \date		April 7th, 2020
 *
 *
 *
 */

#include "meta/attribute.h"
#include "logic/statement.h"

#include <initializer_list>
#include <iostream>
#include <llvm/ADT/SmallVector.h>
//#include <llvm/Support/GenericDomTree.h>
//#include "metafunction.h"





using namespace pram::meta;
using namespace pram::logic;


struct VariableVisitor
{
  std::string operator()(const Variable* var) const{
    std::string ret_string = std::string("Bound Variable with ID = ") + var->getIdentifier();
    return ret_string;
  }
  std::string operator()(const BoundVariable* var) const{
    std::string ret_string = std::string("Bound Variable with ID = ") + var->getIdentifier();
    return ret_string;
  }
  std::string operator()(const FreeVariable* var) const{
    std::string ret_string = std::string("Free Variable with ID = ") + var->getIdentifier();
    return ret_string;
  }
};

constexpr static VariableVisitor print_var = {};



template <template <typename, auto> typename ArrType>
struct array_initializer
{
public:
    template <typename T, size_t N>
    ArrType<T, N> operator()(const T (&arr)[N]) const noexcept
    {
        constexpr auto seq = std::make_index_sequence<N>();
        constexpr auto helper = []<size_t ...I>(const T (&arr)[N], const std::index_sequence<I...>&)
        {
          return std::move(ArrType<T, N>{arr[I]...});
        };
        return helper(arr, seq);
    }
};

template <template <typename, auto> typename ArrType = std::array>
constexpr static array_initializer<ArrType> initialze_array{};



int main()
{
    Variable* var1 = new BoundVariable("var1");
    Variable* var2 = new FreeVariable("var2");

    std::cout << dispatch(var1, print_var) << std::endl;
    std::cout << dispatch(var2, print_var) << std::endl;

    delete var1;
    delete var2;

 /*   auto str = initialze_array<llvm::SmallVector>("Hello, World!");

    for (auto c : str)
      if(c)
        std::cout << c;*/
}
