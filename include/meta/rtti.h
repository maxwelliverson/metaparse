//
// Created by maxwell on 2020-04-14.
//

#ifndef METAPARSE_RTTI_H
#define METAPARSE_RTTI_H

#include "meta/attribute.h"

namespace pram::meta
{
  inline namespace rtti {
    template<typename HierarchyBase>
    struct RTTIEnum;
  }
  inline namespace concepts {
    template<typename T>
    concept RTTIEnumType = instantiation_of_type_template<T, rtti::RTTIEnum> &&
                           requires {
      typename T::Kind;
      std::is_enum_v<typename T::Kind>;
      //typename T::TypeMap<static_cast<typename T::Kind>(0)>::type;
      //T::Kind::EnumCount;
    };
  }
  inline namespace rtti {
    template<typename T>
    using RTTIKind = typename RTTIEnum<T>::Kind;
  }
  inline namespace concepts {
    /*template<typename T>
    concept RTTIKindType = instantiation_of_type_template<T, rtti::RTTIKind>;*/
    template <typename T>
    concept RTTIKindType = std::is_enum_v<T>;
    template<typename T>
    concept RTTIEnabled = requires(T &&t) {
      { t.getKind() }
      ->RTTIKindType;
    };
    template<typename T>
    concept RTTIHierarchyBase = RTTIEnumType<rtti::RTTIEnum<T>> &&RTTIEnabled<T>;
  }
  inline namespace rtti {
    template<typename Base, auto E>
    using RTTIDynamic = typename RTTIEnum<Base>::template TypeMap<E>::type;

    template <auto E,
              typename Base,
              typename StrippedBase = strip_type_and_pointers<Base>>
      requires (RTTIKindType<std::remove_cvref_t<decltype(E)>> &&
                RTTIHierarchyBase<StrippedBase> &&
                nttp_is_type<RTTIKind<StrippedBase>, E>)
    constexpr decltype(auto) dyn_cast(Base&& base) {
      using DynamicType = match_qualifiers<RTTIDynamic<StrippedBase, E>, Base>;

      if constexpr(std::is_pointer_v<std::remove_reference_t<Base>>)
        return static_cast<std::remove_reference_t<match_qualifiers<RTTIDynamic<StrippedBase, E>, Base>>>(std::forward<Base>(base));
      else
        return static_cast<match_qualifiers<RTTIDynamic<StrippedBase, E>, Base>>(std::forward<Base>(base));
    }

    namespace rtti_detail
    {
      template <int> struct Constant{};
      template <typename EnumType, typename Base, typename FuncObj, typename ...Args>
      inline constexpr decltype(auto) dispatch(Constant<EnumType::EnumCount>&&, Base&& base, FuncObj&& func, Args&& ...args){
        return (std::forward<FuncObj>(func))(std::forward<Base>(base), std::forward<Args>(args)...);
      }
      template <typename EnumType, int N, typename Base, typename FuncObj, typename ...Args>
      inline constexpr decltype(auto) dispatch(Constant<N>&&, Base&& base, FuncObj&& func, Args&& ...args)
      {
        constexpr auto enum_value = static_cast<EnumType>(N);

        EnumType thisKind;

        if constexpr(std::is_pointer_v<std::remove_reference_t<Base>>)
          thisKind = base->getKind();
        else
          thisKind = base.getKind();

        if(thisKind == enum_value)
          return std::forward<FuncObj>(func)(dyn_cast<enum_value>(std::forward<Base>(base)), std::forward<Args>(args)...);

        return dispatch<EnumType>(Constant<N + 1>{}, std::forward<Base>(base), std::forward<FuncObj>(func), std::forward<Args>(args)...);
      }
    }

    template<typename Base,
             typename... Args,
             typename FuncObj,
             typename StrippedBase = strip_type_and_pointers<Base>>
      requires (RTTIHierarchyBase<StrippedBase>)
    inline constexpr decltype(auto) dispatch(Base&& base, FuncObj &&func, Args &&... args) {
      return rtti_detail::dispatch<RTTIKind<StrippedBase>>(rtti_detail::Constant<0>{}, std::forward<Base>(base), std::forward<FuncObj>(func), std::forward<Args>(args)...);
    }
  }
}

namespace pram
{
  using meta::RTTIKind;
  using meta::dyn_cast;
  using meta::dispatch;
}

#define ENABLE_RTTI(name, ...) template <> struct pram::meta::rtti::RTTIEnum< name >{ __VA_ARGS__ }
#define RTTI_ENUM(...) enum Kind {__VA_ARGS__, EnumCount }; template <Kind, typename = void> struct TypeMap
#define RTTI_DYNAMIC_TYPE(e, t) template <typename Void> struct TypeMap< e , Void>{using type = t;}

#endif//METAPARSE_RTTI_H
