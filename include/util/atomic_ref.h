//
// Created by maxwell on 2020-04-14.
//

#ifndef METAPARSE_ATOMIC_REF_H
#define METAPARSE_ATOMIC_REF_H

#include "meta/rtti.h"
#include "logic/variable.h"

namespace pram::util {

  template<int I, typename Base>
  constexpr size_t hierarchy_max_size_impl() {
    return 0;
  }

  template<int I, typename Base, typename Derived = meta::RTTIDynamic<Base, I>>
  requires(!std::is_same_v<Derived, meta::LastEnum>)
  constexpr size_t hierarchy_max_size_impl() {
    static_assert(!std::is_void_v<Derived>, "Derived Type must never be void.");

    constexpr size_t next_size = hierarchy_max_size_impl<I + 1, Base>();
    if constexpr (meta::complete_type<Derived>) {
      if constexpr (constexpr size_t this_size = sizeof(Derived); meta::RTTIHierarchyBase<Derived>)
        return std::max({this_size, next_size, hierarchy_max_size_impl<0, Derived>()});
      else 
        return std::max(this_size, next_size);
    }
    else {
      return next_size;
    }
  }

  template <meta::RTTIHierarchyBase Base>
  requires(meta::complete_type<Base>)
  constexpr size_t hierarchy_max_size() {
    return hierarchy_max_size_impl<0, Base>();
  }

  template<meta::complete_type Base>
  constexpr size_t hierarchy_max_size() {
    return sizeof(Base);
  }

  template<typename Base>
  constexpr size_t hierarchy_max_size() {
    return 1;
  }

  template <typename T>
  struct AtomicRef {





  };



}

#endif//METAPARSE_ATOMIC_REF_H
