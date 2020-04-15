//
// Created by maxwe on 2020-04-08.
//

#ifndef METAPARSE_ATTRIBUTE_H
#define METAPARSE_ATTRIBUTE_H

#define RESULT_TYPE(...)
#define DECL_ATTRIBUTE(name, ...) struct name : ::pram::meta::attribute_tag< name > , ##__VA_ARGS__


#include "meta/concepts.h"

namespace pram::meta {


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
