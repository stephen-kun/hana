/*
@copyright Louis Dionne 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#include "heterogeneous_storage.hpp"
#include "heterogeneous_storage_unpack.hpp"

#include <boost/hana/assert.hpp>
#include <boost/hana/equal.hpp>
#include <boost/hana/tuple.hpp>

#include <string>
namespace hana = boost::hana;


int main() {
    hana::heterogeneous_storage<int, std::string, char> storage{1, "abc", 'y'};
    auto result = unpack_(storage, [](auto ...x) {
        return hana::make_tuple(x...);
    });

    BOOST_HANA_RUNTIME_CHECK(result == hana::make_tuple(1, "abc", 'y'));
}
