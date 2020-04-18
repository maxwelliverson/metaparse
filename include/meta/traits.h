//
// Created by maxwell on 2020-04-17.
//

#ifndef METAPARSE_TRAITS_H
#define METAPARSE_TRAITS_H

#include <type_traits>

namespace pram::meta{
  namespace detail {
    template <typename T>
    struct strip_type{
      using type = T;
    };
    template <typename T>
    struct strip_type<const T>{
      using type = typename strip_type<T>::type;
    };
    template <typename T>
    struct strip_type<volatile T>{
      using type = typename strip_type<T>::type;
    };
    template <typename T>
    struct strip_type<T&>{
      using type = typename strip_type<T>::type;
    };
    template <typename T>
    struct strip_type<T&&>{
      using type = typename strip_type<T>::type;
    };

    template <typename T>
    struct strip_type_and_pointers{
      using type = T;
    };
    template <typename T>
    struct strip_type_and_pointers<const T>{
      using type = typename strip_type_and_pointers<T>::type;
    };
    template <typename T>
    struct strip_type_and_pointers<volatile T>{
      using type = typename strip_type_and_pointers<T>::type;
    };
    template <typename T>
    struct strip_type_and_pointers<T&>{
      using type = typename strip_type_and_pointers<T>::type;
    };
    template <typename T>
    struct strip_type_and_pointers<T&&>{
      using type = typename strip_type_and_pointers<T>::type;
    };
    template <typename T>
    struct strip_type_and_pointers<T*>{
      using type = typename strip_type_and_pointers<T>::type;
    };


    template<typename T, typename U>
    struct match_qualifiers {
      using type = U;
    };
    template<typename T, typename U>
    struct match_qualifiers<T &, U> {
      using type = typename match_qualifiers<T, U>::type &;
    };
    template<typename T, typename U>
    struct match_qualifiers<T &&, U> {
      using type = typename match_qualifiers<T, U>::type &&;
    };
    template<typename T, typename U>
    struct match_qualifiers<T *, U> {
      using type = typename match_qualifiers<T, U>::type *;
    };
    template<typename T, typename U>
    struct match_qualifiers<const T, U> {
      using type = typename match_qualifiers<T, const U>::type;
    };
    template<typename T, typename U>
    struct match_qualifiers<volatile T, U> {
      using type = typename match_qualifiers<T, volatile U>::type;
    };

    template <typename T, auto V>
    struct nttp_is_type : std::false_type {};
    template <typename T, T V>
    struct nttp_is_type<T, V> : std::true_type {};
  }

  template <typename T, typename U>
  using match_qualifiers = typename detail::match_qualifiers<U, T>::type;
  template <typename T>
  using strip_type = typename detail::strip_type<T>::type;
  template <typename T>
  using strip_type_and_pointers = typename detail::strip_type_and_pointers<T>::type;

  template <typename T, auto V>
  constexpr static bool nttp_is_type = detail::nttp_is_type<T, V>::value;
}

#endif//METAPARSE_TRAITS_H
