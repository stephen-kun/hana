/*
@copyright Louis Dionne 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#include "heterogeneous_storage.hpp"
#include "heterogeneous_storage_zip.hpp"

#include <boost/hana/assert.hpp>
#include <boost/hana/equal.hpp>
#include <boost/hana/pair.hpp>

#include <string>
namespace hana = boost::hana;


int main() {
    hana::heterogeneous_storage<int, std::string, char> storage1{1, "abc", 'y'};
    hana::heterogeneous_storage<std::string, char, double> storage2{"xyz", 'v', 3.3};

    auto result = hana::zip(storage1, storage2);

    BOOST_HANA_RUNTIME_CHECK(*result.nth<0>() == hana::make_pair(1, "xyz"));
    BOOST_HANA_RUNTIME_CHECK(*result.nth<1>() == hana::make_pair("abc", 'v'));
    BOOST_HANA_RUNTIME_CHECK(*result.nth<2>() == hana::make_pair('y', 3.3));
}
