//
// Created by maxwell on 2020-04-14.
//

#ifndef METAPARSE_RTTI_H
#define METAPARSE_RTTI_H

#include "meta/attribute.h"
#include <compare>

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
    template<typename T, RTTIKind<T> I>
    concept RTTIEnumEnd = I == RTTIKind<T>::EnumCount;
    template<typename T, RTTIKind<T> I>
    concept RTTIEnumNotEnd = !RTTIEnumEnd<T, I>;
  }
  inline namespace rtti {
    struct LastEnum{};

    namespace rtti_detail {
      template<typename Base, RTTIKind<Base> E>
      struct GetDynamicType;

      template<typename Base, RTTIKind<Base> E>
      requires(RTTIEnumEnd<Base, E>)
      struct GetDynamicType<Base, E> {
        using type = LastEnum;
      };

      template <typename Base, RTTIKind<Base> E>
      requires(RTTIEnumNotEnd<Base, E>) struct GetDynamicType<Base, E> {
        using type = typename RTTIEnum<Base>::template TypeMap<E>::type;
      };


    }

    template<typename Base, auto E>
    using RTTIDynamic = typename rtti_detail::GetDynamicType<Base, static_cast<RTTIKind<Base>>(E)>::type;

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
      inline constexpr decltype(auto) [[maybe_unused]] dispatch(Constant<EnumType::EnumCount> &&, Base &&base, FuncObj &&func, Args &&... args) {
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

      struct dispatch_fn {
        template<typename Base,
                 typename... Args,
                 typename FuncObj,
                 typename StrippedBase = strip_type_and_pointers<Base>>
                 requires(RTTIHierarchyBase<StrippedBase>)
        constexpr decltype(auto) operator()(Base &&base, FuncObj &&func, Args &&... args) const noexcept {
          return dispatch<RTTIKind<StrippedBase>>(rtti_detail::Constant<0>{}, std::forward<Base>(base), std::forward<FuncObj>(func), std::forward<Args>(args)...);
        }
      };
    }

    inline constexpr static rtti_detail::dispatch_fn dispatch{};

    namespace rtti_detail {
      template <RTTIHierarchyBase Base>
      class rtti_range {
        using RTTIEnumKind = RTTIKind<Base>;

        class rtti_iterator {
          int pos;
        public:
          explicit constexpr rtti_iterator(int pos) : pos(pos){}

          constexpr RTTIEnumKind operator*() const noexcept {
            return pos;
          }
          constexpr rtti_iterator & operator++() {
            ++pos;
            return *this;
          }
          constexpr std::strong_ordering operator<=>(const rtti_iterator& other) const noexcept{
            if (pos == other.pos)
              return std::strong_ordering::equal;
            if (pos < other.pos)
              return std::strong_ordering::less;

            return std::strong_ordering::greater;
          }

        };

        friend constexpr auto begin(const rtti_range& range) {
          return rtti_iterator(0);
        }
        friend constexpr auto end(const rtti_range& range) {
          return rtti_iterator(RTTIEnumKind::EnumCount);
        }

      public:
        explicit constexpr rtti_range() = default;
      };
      template<RTTIHierarchyBase Base>
      struct hierarchy_iterator_fn {
        constexpr auto operator()() const noexcept {
          return rtti_range<Base>();
        }
      };
      struct recursive_dispatch_fn {
        template<typename Base,
                 typename... Args,
                 typename FuncObj,
                 typename StrippedBase = strip_type_and_pointers<Base>>
                 requires(RTTIHierarchyBase<StrippedBase>)
        constexpr decltype(auto) operator()(Base &&base, FuncObj &&func, Args &&... args) const{
          return dispatch(std::forward<Base>(base), *this, std::forward<FuncObj>(func), std::forward<Args>(args)...);
        }

        template<typename Derived,
                 typename... Args,
                 typename FuncObj>
        constexpr decltype(auto) operator()(Derived &&derived, FuncObj &&func, Args &&... args) const{
          return std::forward<FuncObj>(func)(std::forward<Derived>(derived), std::forward<Args>(args)...);
        }


      };
    }

    template<typename Base>
    constexpr static rtti_detail::hierarchy_iterator_fn<strip_type<Base>> hierarchy_iterator{};
    constexpr static rtti_detail::recursive_dispatch_fn recursive_dispatch{};

    namespace rtti_detail {
      struct double_dispatch_fn {
        struct double_dispatch_second_fn {
          template<typename BaseA,
                   typename BaseB,
                   typename... Args,
                   typename FuncObj>
          constexpr decltype(auto) operator()(BaseB &&base_b, FuncObj &&func, BaseA &&base_a, Args &&... args) const noexcept {
            return recursive_dispatch(std::forward<BaseA>(base_a), std::forward<FuncObj>(func), std::forward<BaseB>(base_b), std::forward<Args>(args)...);
          }
        };
        constexpr static double_dispatch_second_fn double_dispatch_second{}; 


        template<typename BaseA,
                 typename BaseB,
                 typename... Args,
                 typename FuncObj>
        constexpr decltype(auto) operator()(BaseA &&base_a, BaseB &&base_b, FuncObj &&func, Args &&... args) const noexcept {
          return recursive_dispatch(std::forward<BaseB>(base_b), double_dispatch_second, std::forward<BaseB>(base_b), std::forward<FuncObj>(func), std::forward<Args>(args)...);
        }
      };
    }

    constexpr static rtti_detail::double_dispatch_fn double_dispatch{};

  }
}

namespace pram
{
  using meta::RTTIKind;
  using meta::dyn_cast;
  using meta::dispatch;
}

#define ENABLE_RTTI(name, ...) template <> struct pram::meta::rtti::RTTIEnum< name >{ __VA_ARGS__ }
#define RTTI_ENUM(...) enum Kind : unsigned char {__VA_ARGS__, EnumCount }; template <Kind, typename = void> struct TypeMap
#define RTTI_DYNAMIC_TYPE(e, t) template <typename Void> struct TypeMap< e , Void>{using type = t;}

#endif//METAPARSE_RTTI_H
