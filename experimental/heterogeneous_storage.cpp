/*
@copyright Louis Dionne 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#include "heterogeneous_storage.hpp"

#include <boost/hana/assert.hpp>

#include <string>
namespace hana = boost::hana;


int main() {
    hana::heterogeneous_storage<int, char, std::string, double> storage{1, 'x', "abcdef", 3.4};
    int* a = storage.nth<0>();
    char* b = storage.nth<1>();
    std::string* c = storage.nth<2>();
    double* d = storage.nth<3>();

    BOOST_HANA_RUNTIME_CHECK(*a == 1);
    BOOST_HANA_RUNTIME_CHECK(*b == 'x');
    BOOST_HANA_RUNTIME_CHECK(*c == "abcdef");
    BOOST_HANA_RUNTIME_CHECK(*d == 3.4);
}
