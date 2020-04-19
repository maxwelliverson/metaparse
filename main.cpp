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
//#include <llvm/IR/BasicBlock.h>
//#include <llvm/IR/Value.h>
//#include <llvm/IR/Type.h>
//#include <llvm/IR/Metadata.h>
//#include <llvm/Support/GenericDomTree.h>
//#include "metafunction.h"
//
#include "util/atomic_ref.h"





using namespace pram::meta;
using namespace pram::logic;


struct ValueVisitor
{
  std::string operator()(const Value* var) const{
    return "Unknown Value";
  }
  std::string operator()(const BoundVariable* var) const{
    return std::string("Bound Variable with ID = ") + var->getIdentifier();
  }
  std::string operator()(const FreeVariable* var) const{
    return std::string("Free Variable with ID = ") + var->getIdentifier();
  }
};

constexpr static ValueVisitor print_var = {};

struct SizeTestA {
  const ExprNode *ptr;
  const unsigned char n;
};
struct SizeTestB {
  const unsigned char n;
  const ExprNode *ptr;
};



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
constexpr static array_initializer<ArrType> initialize_array{};

#define print_size(...) std::cout << "Size of " << #__VA_ARGS__ << " Hierarchy: " << pram::util::hierarchy_max_size<__VA_ARGS__>() << std::endl

int main()
{
    using Type = RTTIDynamic<RTTIDynamic<Value, 1>, 1>;
    constexpr bool val = RTTIHierarchyBase<Variable>;

    print_size(ExprNode);
    print_size(RootExprNode);
    print_size(IntermediateExprNode);
    print_size(TerminalExprNode);
    print_size(ValueExprNode);
    print_size(NullExprNode);
    print_size(Operation);

    print_size(int);

    print_size(Value);
    print_size(Constant);
    print_size(Variable);
    print_size(BoundVariable);
    print_size(FreeVariable); 

    print_size(SizeTestA);
    print_size(SizeTestB);

    printf("Size of NodeExpr: %llu\n", sizeof(ExprNode));
    printf("Size of Value: %llu\n", sizeof(Value));

    Variable* var1 = new BoundVariable("var1");
    Variable* var2 = new FreeVariable("var2");

    std::cout << dispatch(var1, print_var) << std::endl;
    std::cout << dispatch(var2, print_var) << std::endl;


    delete var1;
    delete var2;

    /*auto str = initialize_array<llvm::SmallVector>("Hello, World!");

    for (auto c : str)
      if(c)
        std::cout << c;*/
}
