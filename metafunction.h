//
// Created by maxwe on 2020-04-08.
//

#ifndef METAPARSE_METAFUNCTION_H
#define METAPARSE_METAFUNCTION_H

#define CONCATX(x, y) x ## y
#define CONCATX2 CONCATX
#define CONCAT CONCATX2
#define ADD_COMMA(...) (__VA_ARGS__) ,
//#define ADD_COMMA ADD_COMMA_I
#define IGNORE(...)
#define REMOVE_PEREN(...) __VA_ARGS__
#define SEQ_HEAD_I3(x, ...) REMOVE_PEREN x
#define SEQ_HEAD_I2(x, ...) SEQ_HEAD_I3 (x)
#define SEQ_HEAD_I SEQ_HEAD_I2
#define SEQ_HEAD(...) SEQ_HEAD_I(ADD_COMMA __VA_ARGS__)



#define SEQ_TAIL_I(...)
#define SEQ_TAIL(...) SEQ_TAIL_I __VA_ARGS__

#define META_FN
#define METAFUNCTION_PROTOTYPE(...) template <SEQ_HEAD(SEQ_TAIL(__VA_ARGS__))> struct SEQ_HEAD(__VA_ARGS__)
#define METAFUNCTION_NAME(name) struct name
#define METAFUNCTION_PARAMS(...) <__VA_ARGS__>
#define METAFUNCTION_I METAFUNCTION_NAME
#define METAFUNCTION(...) METAFUNCTION_PROTOTYPE(__VA_ARGS__) SEQ_TAIL(SEQ_TAIL(__VA_ARGS__))
#define REPLACE_BRACKETS(...) < __VA_ARGS__ > ::result
#define CALL(name) name REPLACE_BRACKETS


#define END_PEREN_I )
#define END_PEREN END_PEREN_I
#define START_PEREN START_PEREN_I
#define START_PEREN_I (

#define GIVEN(...) template <__VA_ARGS__>
#define MATCH_PARAMS(...) <__VA_ARGS__>
#define IF_ARGS_OF struct


#define RETURN_I2(...) meta::metafunction::metafunction_return<decltype(meta::metafunction::metafunction_recurse< __VA_ARGS__ >())>\
{using meta::metafunction::metafunction_return<decltype(meta::metafunction::metafunction_recurse< __VA_ARGS__ >())>::result;}
#define RETURN_I RETURN_I2
#define RETURN RETURN_I
#define THROW_ERROR(e) ::meta::metafunction::error<::meta::metafunction::error_type:: e>\
{using ::meta::metafunction::error<::meta::metafunction::error_type:: e>::result;}

#include "meta.h"

namespace meta{


    namespace metafunction {

        template <decltype(auto) V>
        struct value
        {
            constexpr static decltype(auto) result = V;
        };

        template <typename T>
        T metafunction_recurse();
        template <decltype(auto) V>
        value<V> metafunction_recurse();

        enum struct error_type
        {
            no_error,
            out_of_range,
            invalid_args,
            constraints_not_satisfied
        };
        template <error_type E>
        struct error
        {
            static_assert(E != error_type::out_of_range, "Metafunction Error: Out of Range");
            static_assert(E != error_type::constraints_not_satisfied, "Metafunction Error: Constraints not Satisfied");
            static_assert(E != error_type::invalid_args, "Metafunction Error: Invalid Arguments");

            constexpr static error_type result = E;
        };

        template <typename T>
        struct metafunction_return : uninstantiable
        {
            using result = T;
        };
        template <decltype(auto) V>
        struct metafunction_return<value<V>> : uninstantiable
        {
            constexpr static decltype(auto) result = V;
        };

        template <typename T>
        concept metafunction_end = !derived_from_template<T, metafunction_return>;

        template <typename T>
        struct metafunction_body;
        template <metafunction_end T>
        struct metafunction_body<T> :
                metafunction_return<T>
        {
            using metafunction_return<T>::result;
        };
        template <typename T>
        struct metafunction_body : T
        {
            using T::result;
        };
    }

    /*template <typename T>
    constexpr static bool is_uninstantiable = false;*/
    template <std::derived_from<uninstantiable> T>
    constexpr static bool is_uninstantiable = true;
}

#define STRINGIZE_I(...) #__VA_ARGS__
#define STRINGIZE STRINGIZE_I

using namespace meta::concepts;
using namespace meta::metafunction;


METAFUNCTION_PROTOTYPE((get_second_type_if_not_void)(typename, typename));

METAFUNCTION((get_second_type_if_not_void)(typename T, typename T2) : RETURN(T2));

GIVEN(typename T) IF_ARGS_OF get_second_type_if_not_void MATCH_PARAMS(T, void): RETURN(T);

METAFUNCTION((remove_const)(typename T) : RETURN(T));
GIVEN(typename T) IF_ARGS_OF remove_const MATCH_PARAMS(const T): RETURN(T);

METAFUNCTION((type_at_index)(size_t N, typename ...T));
GIVEN(typename H, typename ...T) IF_ARGS_OF type_at_index MATCH_PARAMS(0, H, T...): RETURN(H);
GIVEN(size_t N, typename H, typename ...T) IF_ARGS_OF type_at_index MATCH_PARAMS(N, H, T...): RETURN(type_at_index<N-1, T...>);
GIVEN(size_t N) IF_ARGS_OF type_at_index MATCH_PARAMS(N): THROW_ERROR(out_of_range);

#endif //METAPARSE_METAFUNCTION_H
