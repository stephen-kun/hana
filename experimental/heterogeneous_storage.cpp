/*
@copyright Louis Dionne 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#include "heterogeneous_storage.hpp"

#include <boost/hana/assert.hpp>

#include <string>
#include <utility>
namespace hana = boost::hana;


int main() {
    // Variadic construction
    {
        using Storage = hana::heterogeneous_storage<int, char, std::string, double>;
        Storage storage{1, 'x', "abcdef", 3.4};
        BOOST_HANA_RUNTIME_CHECK(*storage.nth<0>() == 1);
        BOOST_HANA_RUNTIME_CHECK(*storage.nth<1>() == 'x');
        BOOST_HANA_RUNTIME_CHECK(*storage.nth<2>() == "abcdef");
        BOOST_HANA_RUNTIME_CHECK(*storage.nth<3>() == 3.4);
    }

    // Copy construction
    {
        using Storage = hana::heterogeneous_storage<int, char, std::string, double>;
        Storage storage{1, 'x', "abcdef", 3.4};

        Storage copy1(storage);
        BOOST_HANA_RUNTIME_CHECK(*copy1.nth<0>() == 1);
        BOOST_HANA_RUNTIME_CHECK(*copy1.nth<1>() == 'x');
        BOOST_HANA_RUNTIME_CHECK(*copy1.nth<2>() == "abcdef");
        BOOST_HANA_RUNTIME_CHECK(*copy1.nth<3>() == 3.4);

        Storage copy2{storage};
        BOOST_HANA_RUNTIME_CHECK(*copy2.nth<0>() == 1);
        BOOST_HANA_RUNTIME_CHECK(*copy2.nth<1>() == 'x');
        BOOST_HANA_RUNTIME_CHECK(*copy2.nth<2>() == "abcdef");
        BOOST_HANA_RUNTIME_CHECK(*copy2.nth<3>() == 3.4);

        Storage copy3 = storage;
        BOOST_HANA_RUNTIME_CHECK(*copy3.nth<0>() == 1);
        BOOST_HANA_RUNTIME_CHECK(*copy3.nth<1>() == 'x');
        BOOST_HANA_RUNTIME_CHECK(*copy3.nth<2>() == "abcdef");
        BOOST_HANA_RUNTIME_CHECK(*copy3.nth<3>() == 3.4);
    }

    // Move construction
    {
        using Storage = hana::heterogeneous_storage<int, char, std::string, double>;
        Storage storage{1, 'x', "abcdef", 3.4};

        Storage move1(std::move(storage));
        BOOST_HANA_RUNTIME_CHECK(*move1.nth<0>() == 1);
        BOOST_HANA_RUNTIME_CHECK(*move1.nth<1>() == 'x');
        BOOST_HANA_RUNTIME_CHECK(*move1.nth<2>() == "abcdef");
        BOOST_HANA_RUNTIME_CHECK(*move1.nth<3>() == 3.4);

        Storage move2{std::move(move1)};
        BOOST_HANA_RUNTIME_CHECK(*move2.nth<0>() == 1);
        BOOST_HANA_RUNTIME_CHECK(*move2.nth<1>() == 'x');
        BOOST_HANA_RUNTIME_CHECK(*move2.nth<2>() == "abcdef");
        BOOST_HANA_RUNTIME_CHECK(*move2.nth<3>() == 3.4);

        Storage move3 = std::move(move2);
        BOOST_HANA_RUNTIME_CHECK(*move3.nth<0>() == 1);
        BOOST_HANA_RUNTIME_CHECK(*move3.nth<1>() == 'x');
        BOOST_HANA_RUNTIME_CHECK(*move3.nth<2>() == "abcdef");
        BOOST_HANA_RUNTIME_CHECK(*move3.nth<3>() == 3.4);
    }
}
