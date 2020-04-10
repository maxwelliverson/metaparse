//
// Created by maxwe on 2020-04-08.
//

#ifndef METAPARSE_ATTRIBUTE_H
#define METAPARSE_ATTRIBUTE_H

#define RESULT_TYPE(...)
#define DECL_ATTRIBUTE(name, ...) struct name : ::pram::meta::attribute_tag< name > , ##__VA_ARGS__

#include <type_traits>

namespace pram::meta {
  namespace detail {

    template<typename T>
    struct recurse_result : T { using T::result; };

    template<typename Derived, template<typename...> typename Base>
    Derived extract_crtp_base_helper(...);

    template<typename Derived, template<typename...> typename Base>
    Base<Derived> extract_crtp_base_helper(const Base<Derived> &);

    template<typename Derived, template<typename...> typename Base,
             typename... OtherTypes>
    Base<Derived, OtherTypes...>
    extract_crtp_base_helper(const Base<Derived, OtherTypes...> &);

    template<typename Derived, template<typename...> typename Base>
    Derived extract_templated_base_helper(...);

    template<typename Derived, template<typename...> typename Base,
             typename... OtherTypes>
    Base<OtherTypes...> extract_templated_base_helper(const Base<OtherTypes...> &);
  }// namespace detail

  template<typename Derived, template<typename...> typename Base>
  using extract_templated_base_t =
          decltype(detail::extract_templated_base_helper<Derived, Base>(
                  std::declval<const Derived &>()));
  template<typename Derived, template<typename...> typename Base>
  using extract_crtp_base_t =
          decltype(detail::extract_crtp_base_helper<Derived, Base>(
                  std::declval<const Derived &>()));

  template<typename T>
  struct remove_cv_past_ref { using result = T; };
  template<typename T>
  struct remove_cv_past_ref<const T &> {};

  inline namespace concepts {
    template<typename T>
    concept tag_type = std::is_empty_v<T>;

    template<typename Base, typename Derived>
    concept base_class_of = requires(std::remove_reference_t<Base> *base,
                                     std::remove_reference_t<Derived> *der) {
      {base = der};
      noexcept(dynamic_cast<std::remove_reference_t<Base> *>(der));
    }
    &&!requires(std::remove_reference_t<Base> * base,
                std::remove_reference_t<Derived> *der) {
      {der = base};
      noexcept(dynamic_cast<std::remove_reference_t<Derived> *>(base));
    };

    template<typename T, template<typename...> typename TT>
    concept derived_from_template =
            base_class_of<extract_templated_base_t<T, TT>, T>;
    template<typename T, template<typename...> typename TT>
    concept crtp_inherits = requires {
      base_class_of<extract_crtp_base_t<T, TT>, T>;
    };
    template<typename T, template<typename...> typename Tag>
    concept tagged_as =
            crtp_inherits<T, Tag> &&std::is_empty_v<extract_crtp_base_t<T, Tag>>;

    template<typename T>
    concept l_ref = std::is_lvalue_reference_v<T>;
    template<typename T>
    concept r_ref = std::is_rvalue_reference_v<T>;
    template<typename T>
    concept const_qualified = std::is_const_v<std::remove_reference_t<T>>;
    template<typename T>
    concept volatile_qualified = std::is_volatile_v<std::remove_reference_t<T>>;
    template<typename T>
    concept cv_qualified = const_qualified<T> &&volatile_qualified<T>;

    template<typename T, typename U>
    concept any_cv_qualifiers = std::is_same_v<std::remove_cv_t<T>, U>;
    template<typename T, typename U>
    concept any_ref = std::is_same_v<std::remove_reference_t<T>, U>;
    template<typename T, typename U>
    concept any_qualifiers = std::is_same_v<std::remove_cvref_t<T>, U>;
  }// namespace concepts


  /********************************************************************
 * \brief A metadata tag from which all other metadata tags derive.
 *
 * \class		attribute_tag
 * \code		template <typename> attribute_tag {}; \endcode
 *
 *
 * \param T A metadata tag
 *
 * \pre Template parameter \code T \endcode must CRTP inherit from
 *		attribute_tag.
 ********************************************************************/
  template<typename T>
  class attribute_tag;

  inline namespace concepts {
    template<typename T>
    concept metadata = tagged_as<T, attribute_tag>;
    template<typename T>
    concept attribute = base_class_of<attribute_tag<T>, T>;
    template<typename T, typename Attr>
    concept has_attribute = attribute<Attr> &&base_class_of<Attr, T>;
  }// namespace concepts

  template<typename T>
  class attribute_tag
  {};

  DECL_ATTRIBUTE(uninstantiable) {
    uninstantiable() = delete;
    uninstantiable(const uninstantiable &) = delete;
    uninstantiable(uninstantiable &&) = delete;
    uninstantiable &operator=(const uninstantiable &) = delete;
    uninstantiable &operator=(uninstantiable &&) = delete;
    ~uninstantiable() = delete;
  };

}// namespace meta

#endif// METAPARSE_ATTRIBUTE_H
