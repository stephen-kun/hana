/*
@copyright Louis Dionne 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_EXPERIMENTAL_HETEROGENEOUS_STORAGE_ZIP_HPP
#define BOOST_HANA_EXPERIMENTAL_HETEROGENEOUS_STORAGE_ZIP_HPP

#include "heterogeneous_storage.hpp"

#include <boost/hana/pair.hpp>

#include <cstddef>
#include <new>


namespace boost { namespace hana {
    template <typename ...T, typename ...U,
              typename Result = hana::heterogeneous_storage<hana::pair<T, U>...>>
    Result zip(hana::heterogeneous_storage<T...> const& ts,
               hana::heterogeneous_storage<U...> const& us)
    {
        Result result{hana::uninitialized{}};

        std::size_t i = 0;
        std::size_t expand[] = {
            (::new (result.raw_nth(i)) hana::pair<T, U>(
                *static_cast<T const*>(ts.raw_nth(i)),
                *static_cast<U const*>(us.raw_nth(i))
            ), ++i)...
        };
        (void)expand;

        return result;
    }
}} // end namespace boost::hana

#endif // !BOOST_HANA_EXPERIMENTAL_HETEROGENEOUS_STORAGE_ZIP_HPP
