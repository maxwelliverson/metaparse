//
// Created by maxwell on 2020-04-11.
//

#ifndef METAPARSE_METAVALUE_H
#define METAPARSE_METAVALUE_H

#include <type_traits>

namespace pram::meta
{
  template <auto& V>
  struct MetaValue
  {
    using type = std::remove_reference_t<decltype(V)>;
    constexpr static decltype(auto) value = V;

    constexpr MetaValue() = default;

    constexpr operator const type&() const & noexcept
    {
      return V;
    }
    constexpr operator type&&() const && noexcept(noexcept(type(V)))
    {
      return V;
    }
  };
}

#endif//METAPARSE_METAVALUE_H
