/*
@copyright Louis Dionne 2016
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#include <boost/hana/assert.hpp>

#include "hvector.hpp"

#include <string>
namespace hana = boost::hana;


int main() {
    {
        hana::raw_storage<int, char, std::string> storage{};
        hana::hvector<> empty{storage};
        hana::hvector<int, char, std::string> full = empty.append(1).append('x').append(std::string{"abcdef"});

        BOOST_HANA_RUNTIME_CHECK(full.nth<0>() == 1);
        BOOST_HANA_RUNTIME_CHECK(full.nth<1>() == 'x');
        BOOST_HANA_RUNTIME_CHECK(full.nth<2>() == "abcdef");
    }
}
