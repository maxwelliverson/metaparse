/**
 * \file		main.cpp
 * \author		Maxwell Iverson
 * \date		April 7th, 2020
 *
 *
 *
 */

#include <initializer_list>
#include <iostream>
#include <llvm/ADT/SmallVector.h>
#include <llvm/Support/GenericDomTree.h>
#include "meta.h"
//#include "metafunction.h"





using namespace meta::concepts;
template <typename T, template <typename> typename TT>
constexpr static bool test = base_class_of<TT<T>, T>;

struct array_initializer
{
    template <typename T, size_t N>
    constexpr std::initializer_list<char> operator()(const T (& arr)[N]) const noexcept
    {
        return std::initializer_list<char>(arr + 0, arr + N);
    }
};

constexpr static array_initializer initialze{};



int main()
{
    llvm::SmallVector<char, 16> str = initialze("Hello, World!");

    std::cout << std::boolalpha;

#define print_trait(...) std::cout << #__VA_ARGS__ << ": " << __VA_ARGS__ << std::endl

    struct lmao : meta::uninstantiable{};


    static_assert(std::is_same_v<meta::extract_crtp_base_t<lmao, meta::attribute_tag>, lmao>);
    static_assert(std::is_same_v<meta::extract_crtp_base_t<meta::uninstantiable, meta::attribute_tag>, meta::attribute_tag<meta::uninstantiable>>);
    static_assert(test<meta::uninstantiable, meta::attribute_tag>);
    static_assert(!test<lmao, meta::attribute_tag>);

    static_assert(crtp_inherits<meta::uninstantiable, meta::attribute_tag>);
    //static_assert(!crtp_inherits<lmao, meta::attribute_tag>);

#undef print_trait

    for (auto c : str)
        std::cout << c;
}
