/*!
@file
Defines `boost::hana::sort`.

@copyright Louis Dionne 2013-2016
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_SORT_HPP
#define BOOST_HANA_SORT_HPP

#include <boost/hana/fwd/sort.hpp>

#include <boost/hana/at.hpp>
#include <boost/hana/concept/sequence.hpp>
#include <boost/hana/config.hpp>
#include <boost/hana/core/dispatch.hpp>
#include <boost/hana/core/make.hpp>
#include <boost/hana/detail/nested_by.hpp> // required by fwd decl
#include <boost/hana/length.hpp>
#include <boost/hana/less.hpp>

#include <utility> // std::declval, std::index_sequence


BOOST_HANA_NAMESPACE_BEGIN
    //! @cond
    template <typename Xs, typename Predicate>
    constexpr auto sort_t::operator()(Xs&& xs, Predicate&& pred) const {
        using S = typename hana::tag_of<Xs>::type;
        using Sort = BOOST_HANA_DISPATCH_IF(sort_impl<S>,
            hana::Sequence<S>::value
        );

    #ifndef BOOST_HANA_CONFIG_DISABLE_CONCEPT_CHECKS
        static_assert(hana::Sequence<S>::value,
        "hana::sort(xs, predicate) requires 'xs' to be a Sequence");
    #endif

        return Sort::apply(static_cast<Xs&&>(xs),
                           static_cast<Predicate&&>(pred));
    }

    template <typename Xs>
    constexpr auto sort_t::operator()(Xs&& xs) const {
        using S = typename hana::tag_of<Xs>::type;
        using Sort = BOOST_HANA_DISPATCH_IF(sort_impl<S>,
            hana::Sequence<S>::value
        );

    #ifndef BOOST_HANA_CONFIG_DISABLE_CONCEPT_CHECKS
        static_assert(hana::Sequence<S>::value,
        "hana::sort(xs) requires 'xs' to be a Sequence");
    #endif

        return Sort::apply(static_cast<Xs&&>(xs));
    }
    //! @endcond

    namespace detail {
        template <typename Xs, typename Pred>
        struct sort_predicate {
            template <std::size_t I, std::size_t J>
            using apply = decltype(std::declval<Pred>()(
                hana::at_c<I>(std::declval<Xs>()),
                hana::at_c<J>(std::declval<Xs>())
            ));
        };

        template <typename Pred, std::size_t Insert, bool IsInsertionPoint,
                  typename Left,
                  std::size_t ...Right>
        struct insert;

        // We did not find the insertion point; continue processing elements
        // recursively.
        template <
            typename Pred, std::size_t Insert,
            std::size_t ...Left,
            std::size_t Right1, std::size_t Right2, std::size_t ...Right
        >
        struct insert<Pred, Insert, false,
                      std::index_sequence<Left...>,
                      Right1, Right2, Right...
        > {
            using type = typename insert<
                Pred, Insert, (bool)Pred::template apply<Insert, Right2>::value,
                std::index_sequence<Left..., Right1>,
                Right2, Right...
            >::type;
        };

        // We did not find the insertion point, but there is only one element
        // left. We insert at the end of the list, and we're done.
        template <typename Pred, std::size_t Insert, std::size_t ...Left, std::size_t Last>
        struct insert<Pred, Insert, false, std::index_sequence<Left...>, Last> {
            using type = std::index_sequence<Left..., Last, Insert>;
        };

        // We found the insertion point, we're done.
        template <typename Pred, std::size_t Insert, std::size_t ...Left, std::size_t ...Right>
        struct insert<Pred, Insert, true, std::index_sequence<Left...>, Right...> {
            using type = std::index_sequence<Left..., Insert, Right...>;
        };


        template <typename Pred, typename Result, std::size_t ...T>
        struct insertion_sort_impl;

        template <typename Pred,
                  std::size_t Result1, std::size_t ...Result,
                  std::size_t T, std::size_t ...Ts>
        struct insertion_sort_impl<Pred, std::index_sequence<Result1, Result...>, T, Ts...> {
            using type = typename insertion_sort_impl<
                Pred,
                typename insert<
                    Pred, T, (bool)Pred::template apply<T, Result1>::value,
                    std::index_sequence<>,
                    Result1, Result...
                >::type,
                Ts...
            >::type;
        };

        template <typename Pred, std::size_t T, std::size_t ...Ts>
        struct insertion_sort_impl<Pred, std::index_sequence<>, T, Ts...> {
            using type = typename insertion_sort_impl<
                Pred, std::index_sequence<T>, Ts...
            >::type;
        };

        template <typename Pred, typename Result>
        struct insertion_sort_impl<Pred, Result> {
            using type = Result;
        };

        template <typename Pred, typename Indices>
        struct sort_helper;

        template <typename Pred, std::size_t ...i>
        struct sort_helper<Pred, std::index_sequence<i...>> {
            using type = typename insertion_sort_impl<
                Pred, std::index_sequence<>, i...
            >::type;
        };
    } // end namespace detail

    namespace detail2 {
        namespace Detail {
            template <template <std::size_t,std::size_t> class Pred>
            struct S {
                template<typename Out, typename In,
                    bool Tag, bool FTag, std::size_t ... Ts>
                struct SortInsert;

                //next is not less than insert, no more ins, terminate
                template<std::size_t ... Os, std::size_t In, std::size_t ... Ts>
                struct SortInsert<std::index_sequence<Os...>, std::index_sequence<In>, false, false, Ts...> {
                    using type = std::index_sequence<Os..., In, Ts...>;
                };
                //next is less than insert, next is end, terminate
                template<std::size_t ... Os, std::size_t... Ins, std::size_t T>
                struct SortInsert<std::index_sequence<Os...>, std::index_sequence<Ins...>, true, false, T> {
                    using type = std::index_sequence<Os..., T, Ins...>;
                };
                //next is not less than insert, have more next and have more ins, cant fast track
                template<std::size_t ... Os, std::size_t In1, std::size_t In2, std::size_t...Ins, std::size_t T, std::size_t ... Ts>
                struct SortInsert<std::index_sequence<Os...>, std::index_sequence<In1, In2, Ins...>, false, false, T, Ts...> :
                    SortInsert<std::index_sequence<Os..., In1>,
                    std::index_sequence<In2, Ins...>,
                    Pred<T, In2>::value,
                    false,
                    T, Ts...> {};
                //next is not less than insert, have more next and have more ins, can fast track
                template<std::size_t ... Os, std::size_t In1, std::size_t In2, std::size_t...Ins, std::size_t T1, std::size_t T2, std::size_t T3, std::size_t T4, std::size_t T5, std::size_t T6, std::size_t T7, std::size_t T8, std::size_t ... Ts>
                struct SortInsert<std::index_sequence<Os...>, std::index_sequence<In1, In2, Ins...>, false, false, T1, T2, T3, T4, T5, T6, T7, T8, Ts...> :
                    SortInsert<std::index_sequence<Os..., In1>,
                    std::index_sequence<In2, Ins...>,
                    Pred<T1, In2>::value,
                    Pred<T8, In2>::value,
                    T1, T2, T3, T4, T5, T6, T7, T8, Ts...> {};
                //next is less than insert, next is not end
                template<std::size_t ... Os, std::size_t In, std::size_t... Ins,
                    std::size_t T1, std::size_t T2, std::size_t ... Ts>
                struct SortInsert<std::index_sequence<Os...>, std::index_sequence<In,Ins...>, true, false, T1, T2, Ts...> :
                    SortInsert<std::index_sequence<Os..., T1>,
                    std::index_sequence<In,Ins...>,
                    Pred<T2, In>::value,
                    false,
                    T2, Ts...> {};
                //fast track is less than insert, no more
                template<std::size_t ... Os, std::size_t In, std::size_t...Ins, std::size_t T1, std::size_t T2, std::size_t T3, std::size_t T4, std::size_t T5, std::size_t T6, std::size_t T7, std::size_t T8>
                struct SortInsert<std::index_sequence<Os...>, std::index_sequence<In,Ins...>, true, true, T1, T2, T3, T4, T5, T6, T7, T8> {
                    using type = std::index_sequence<Os..., T1, T2, T3, T4, T5, T6, T7, T8, In, Ins...>;
                };
                //fast track is less than insert, not enough to fast track again
                template<std::size_t ... Os, std::size_t In, std::size_t...Ins, std::size_t T1, std::size_t T2, std::size_t T3, std::size_t T4, std::size_t T5, std::size_t T6, std::size_t T7, std::size_t T8, std::size_t ... Ts>
                struct SortInsert<std::index_sequence<Os...>, std::index_sequence<In, Ins...>, true, true, T1, T2, T3, T4, T5, T6, T7, T8, Ts...> :
                    SortInsert<std::index_sequence<Os..., T1, T2, T3, T4, T5, T6, T7, T8>,
                    std::index_sequence<In, Ins...>,
                    Pred<T2, In>::value,
                    false,
                    Ts...> {};
                //fast track is less than insert, can fast track again
                template<std::size_t ... Os, std::size_t In, std::size_t...Ins, std::size_t T1, std::size_t T2, std::size_t T3, std::size_t T4, std::size_t T5, std::size_t T6, std::size_t T7, std::size_t T8, std::size_t T9, std::size_t T10, std::size_t T11, std::size_t T12, std::size_t T13, std::size_t T14, std::size_t T15, std::size_t T16, std::size_t ... Ts>
                struct SortInsert<std::index_sequence<Os...>, std::index_sequence<In, Ins...>, true, true, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, Ts...> :
                    SortInsert<std::index_sequence<Os..., T1, T2, T3, T4, T5, T6, T7, T8>,
                    std::index_sequence<In, Ins...>,
                    Pred<T9, In>::value,
                    Pred<T16, In>::value,
                    T9, T10, T11, T12, T13, T14, T15, T16,
                    Ts...> {};

                template<typename Out, typename In>
                struct CallSortInsert;

                //out cant fast track
                template<std::size_t O, std::size_t ... Os, std::size_t In, std::size_t... Ins>
                struct CallSortInsert<std::index_sequence<O, Os...>, std::index_sequence<In,Ins...>> :
                    SortInsert<std::index_sequence<>,
                    std::index_sequence<In,Ins...>,
                    Pred<O, In>::value,
                    false,
                    O,
                    Os...> {};
                //out can fast track
                template<std::size_t O1, std::size_t O2, std::size_t O3, std::size_t O4, std::size_t O5, std::size_t O6, std::size_t O7, std::size_t O8, std::size_t ... Os, std::size_t In, std::size_t... Ins>
                struct CallSortInsert<std::index_sequence<O1, O2, O3, O4, O5, O6, O7, O8, Os...>, std::index_sequence<In,Ins...>> :
                    SortInsert<std::index_sequence<>,
                    std::index_sequence<In,Ins...>,
                    Pred<O1, In>::value,
                    Pred<O8, In>::value,
                    O1, O2, O3, O4, O5, O6, O7, O8,
                    Os...> {};


                template<typename Out, std::size_t ... Ts>
                struct Sort {
                    using type = Out;  //in is empty
                };
                //out cant fast track
                template<typename Out, std::size_t In, std::size_t ... Ts>
                struct Sort<Out, In, Ts...> :
                    Sort< typename CallSortInsert<Out, std::index_sequence<In>>::type,
                    Ts...> {};

                // out can fast track
                template <typename Out, std::size_t T1, std::size_t T2, std::size_t T3, std::size_t T4, std::size_t T5, std::size_t T6, std::size_t T7, std::size_t T8, std::size_t T9, std::size_t T10, std::size_t T11, std::size_t T12, std::size_t T13, std::size_t T14, std::size_t T15, std::size_t T16, std::size_t ... Ts>
                struct Sort<Out, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, Ts...> :
                    Sort<typename CallSortInsert<Out, typename Sort<std::index_sequence<T1>,T2,T3,T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16>::type>::type,
                    Ts...> {};
            };
        }

        //Sort
        template <typename TList, typename TPred>
        struct sort_impl;
        //empty input case
        template <typename TPred>
        struct sort_impl<std::index_sequence<>, TPred> {
            using type = std::index_sequence<>;
        };
        //one element case
        template <std::size_t T, typename TPred>
        struct sort_impl<std::index_sequence<T>, TPred> {
            using type = std::index_sequence<T>;
        };
        //two or more elements case
        template <std::size_t T, std::size_t U, std::size_t ... Ts, typename TPred>
        struct sort_impl<std::index_sequence<T, U, Ts...>, TPred> : Detail::S<TPred::template apply>::template Sort<std::index_sequence<T>, U, Ts...> {};
        //alias
        template <typename TList, typename TPred>
        using sort = typename sort_impl<TList, TPred>::type;


        template <typename Xs, typename Pred>
        struct sort_predicate {
            template <std::size_t I, std::size_t J>
            using apply = decltype(std::declval<Pred>()(
                hana::at_c<I>(std::declval<Xs>()),
                hana::at_c<J>(std::declval<Xs>())
            ));
        };
    }

    template <typename S, bool condition>
    struct sort_impl<S, when<condition>> : default_ {
        template <typename Xs, std::size_t ...i>
        static constexpr auto apply_impl(Xs&& xs, std::index_sequence<i...>) {
            return hana::make<S>(hana::at_c<i>(static_cast<Xs&&>(xs))...);
        }

#if defined(ORIGINAL_SORT)
        template <typename Xs, typename Pred>
        static constexpr auto apply(Xs&& xs, Pred const&) {
            constexpr std::size_t Len = decltype(hana::length(xs))::value;
            using Indices = typename detail::sort_helper<
                detail::sort_predicate<Xs&&, Pred>,
                std::make_index_sequence<Len>
            >::type;

            // return apply_impl(static_cast<Xs&&>(xs), Indices{});
        }
#elif defined(BRIGAND_SORT)
        template <typename Xs, typename Pred>
        static constexpr auto apply(Xs&& xs, Pred const&) {
            constexpr std::size_t Len = decltype(hana::length(xs))::value;
            using Indices = detail2::sort<
                std::make_index_sequence<Len>,
                detail2::sort_predicate<Xs&&, Pred>
            >;
            // return apply_impl(static_cast<Xs&&>(xs), Indices{});
        }
#endif

        template <typename Xs>
        static constexpr auto apply(Xs&& xs)
        { return sort_impl::apply(static_cast<Xs&&>(xs), hana::less); }
    };
BOOST_HANA_NAMESPACE_END

#endif // !BOOST_HANA_SORT_HPP
