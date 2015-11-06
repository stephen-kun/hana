/*
@copyright Louis Dionne 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_EXPERIMENTAL_HETEROGENEOUS_STORAGE_UNPACK_HPP
#define BOOST_HANA_EXPERIMENTAL_HETEROGENEOUS_STORAGE_UNPACK_HPP

#include "heterogeneous_storage.hpp"

#include <cstddef>
#include <utility>


namespace boost { namespace hana {
    template <typename ...T, typename F, std::size_t ...i>
    auto unpack_impl_(hana::heterogeneous_storage<T...> const& ts, F const& f, std::index_sequence<i...>) {
        return f(*static_cast<T const*>(ts.raw_nth(i))...);
    }

    template <typename ...T, typename F>
    auto unpack_(hana::heterogeneous_storage<T...> const& ts, F const& f) {
        return unpack_impl_(ts, f, std::make_index_sequence<sizeof...(T)>{});
    }
}} // end namespace boost::hana

#endif // !BOOST_HANA_EXPERIMENTAL_HETEROGENEOUS_STORAGE_UNPACK_HPP
