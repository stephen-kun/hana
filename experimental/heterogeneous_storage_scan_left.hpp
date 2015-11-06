/*
@copyright Louis Dionne 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_EXPERIMENTAL_HETEROGENEOUS_STORAGE_SCAN_LEFT_HPP
#define BOOST_HANA_EXPERIMENTAL_HETEROGENEOUS_STORAGE_SCAN_LEFT_HPP

#include "heterogeneous_storage.hpp"

#include <cstddef>
#include <new>
#include <utility>


namespace boost { namespace hana {
    template <std::size_t n, typename State, typename F, typename ...T>
    struct fold_left_nth;


    template <typename State, typename F, typename T, typename ...U>
    struct fold_left_nth<0, State, F, T, U...> {
        using type = State;
    };

    template <typename State, typename F>
    struct fold_left_nth<0, State, F> {
        using type = State;
    };

    template <std::size_t n, typename State, typename F, typename T, typename ...U>
    struct fold_left_nth<n, State, F, T, U...> {
        using NewState = decltype(std::declval<F>()(std::declval<State>(), std::declval<T>()));
        using type = typename fold_left_nth<n - 1, NewState, F, U...>::type;
    };


    template <typename T, typename ...U, typename State, typename F, std::size_t ...i,
              typename Result = hana::heterogeneous_storage<State, typename fold_left_nth<i, State, F, T, U...>::type...>>
    Result scan_left_impl(hana::heterogeneous_storage<T, U...> const& ts, State const& state, F const& f, std::index_sequence<0, i...>) {
        Result result{hana::uninitialized{}};

        void* expand[] = {
            ::new (result.raw_nth(0)) State(state),
            ::new (result.raw_nth(i)) typename fold_left_nth<i, State, F, T, U...>::type(
                f(*static_cast<typename fold_left_nth<i-1, State, F, T, U...>::type*>(result.raw_nth(i-1)),
                  *ts.template nth<i-1>())
            )...
        };
        (void)expand;

        return result;
    }

    template <typename ...T, typename State, typename F>
    auto scan_left(hana::heterogeneous_storage<T...> const& ts, State const& state, F const& f) {
        return scan_left_impl(ts, state, f, std::make_index_sequence<sizeof...(T)+1>{});
    }
}} // end namespace boost::hana

#endif // !BOOST_HANA_EXPERIMENTAL_HETEROGENEOUS_STORAGE_SCAN_LEFT_HPP
