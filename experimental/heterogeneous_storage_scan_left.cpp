/*
@copyright Louis Dionne 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#include "heterogeneous_storage.hpp"
#include "heterogeneous_storage_scan_left.hpp"

#include <boost/hana/assert.hpp>

#include <sstream>
#include <string>
namespace hana = boost::hana;


int main() {
    hana::heterogeneous_storage<int, std::string, char> storage{1, "abc", 'y'};
    auto result = scan_left(storage, std::string{"xx"}, [](auto const& a, auto const& b) {
        std::ostringstream ss{};
        ss << a << b;
        return ss.str();
    });

    BOOST_HANA_RUNTIME_CHECK(*result.nth<0>() == "xx");
    BOOST_HANA_RUNTIME_CHECK(*result.nth<1>() == "xx1");
    BOOST_HANA_RUNTIME_CHECK(*result.nth<2>() == "xx1abc");
    BOOST_HANA_RUNTIME_CHECK(*result.nth<3>() == "xx1abcy");
}
