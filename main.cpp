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
#include <llvm/Support/GenericDomTree.h>
//#include "metafunction.h"





using namespace pram::meta;


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
    auto str = initialze_array<>("Hello, World!");

    for (auto c : str)
      if(c)
        std::cout << c;
}
