/*
@copyright Louis Dionne 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#include "../experimental/heterogeneous_storage.hpp"
namespace hana = boost::hana;


template <int i>
struct x { };

int main() {
    hana::heterogeneous_storage<
        <%= (1..input_size).map { |n| "x<#{n}>" }.join(', ') %>
    > storage{
        <%= (1..input_size).map { |n| "x<#{n}>{}" }.join(', ') %>
    };
    (void)storage;
}
