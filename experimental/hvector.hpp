/*
@copyright Louis Dionne 2016
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_EXPERIMENTAL_HVECTOR_HPP
#define BOOST_HANA_EXPERIMENTAL_HVECTOR_HPP

#include <boost/hana/detail/array.hpp>

#include <cstddef>
#include <type_traits>


namespace boost { namespace hana {
    template <std::size_t n, typename ...T>
    struct nth_type;

    template <typename T, typename ...U>
    struct nth_type<0, T, U...> {
        using type = T;
    };

    template <std::size_t n, typename T, typename ...U>
    struct nth_type<n, T, U...> {
        using type = typename nth_type<n-1, U...>::type;
    };

    template <typename ...T>
    struct normal_struct_layout {
        static constexpr detail::array<std::size_t, sizeof...(T)> sizes = {{sizeof(T)...}};
        static constexpr detail::array<std::size_t, sizeof...(T)> alignments = {{alignof(T)...}};

        static constexpr detail::array<std::size_t, sizeof...(T)> offsets_impl() {
            detail::array<std::size_t, sizeof...(T)> offsets{};
            offsets[0] = 0;
            for (std::size_t i = 1; i < sizeof...(T); ++i) {
                // Pad the current member so it is placed at an offset
                // which is a multiple of its alignment.
                offsets[i] = offsets[i-1] + sizes[i-1] +
                             ((offsets[i-1] + sizes[i-1]) % alignments[i]);
            }

            return offsets;
        }

        static constexpr detail::array<std::size_t, sizeof...(T)> offsets = offsets_impl();
        static constexpr std::size_t total_size = offsets[sizeof...(T)-1] + sizes[sizeof...(T)-1];


        static constexpr void* raw_nth(void* storage, std::size_t n) {
            if (n >= sizeof...(T))
                throw "out of bounds access in a heterogeneous_storage";
            return ((char*)storage) + offsets[n];
        }
    };

    template <typename ...T>
    constexpr detail::array<std::size_t, sizeof...(T)> normal_struct_layout<T...>::offsets;


    //////////////////////////////////////////////////////////////////////////
    // raw_storage
    //  Holds uninitialized storage suitable for holding a pack of types.
    //  Objects __must__ be constructed in-place in the storage, for the
    //  destructor calls the destructor of each type in the pack.
    //
    //  This is meant to be used as follows:
    //      raw_storage<int, char, std::string> storage{};
    //      hvector<> empty{storage};
    //      hvector<int, char, float> full = empty.append(1).append('x').append(2.3f);
    //      // No copy of the partial vectors are done, even though we chain
    //      // calls to `append`, which returns by value. Now, we can use `full`
    //      // as a regular tuple.
    //////////////////////////////////////////////////////////////////////////
    template <typename ...T>
    struct raw_storage {
        using Layout = normal_struct_layout<T...>;
        std::aligned_storage_t<Layout::total_size> actual_;

        raw_storage() { }
        raw_storage(raw_storage const&) = delete;
        raw_storage(raw_storage&&) = delete;
        raw_storage& operator=(raw_storage const&) = delete;
        raw_storage& operator=(raw_storage&&) = delete;

        ~raw_storage() {
            std::size_t i = 0;
            int expand[] = {
                (static_cast<T*>(Layout::raw_nth(&actual_, i++))->~T(), int{})...
            };
            (void)expand;
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // hvector
    //////////////////////////////////////////////////////////////////////////
    template <typename ...T>
    struct hvector;

    template <>
    struct hvector<> {
        void* storage_;

        template <typename ...U>
        explicit hvector(raw_storage<U...>& storage)
            : storage_(&storage.actual_)
        { }

        // only used by hvector itself
        explicit hvector(void* storage)
            : storage_(storage)
        { }

        template <typename U>
        hvector<U> append(U const& u) {
            using Layout = normal_struct_layout<U>;
            ::new (Layout::raw_nth(storage_, 0)) U(u);
            return hvector<U>{storage_};
        }
    };

    template <typename ...T>
    struct hvector {
        void* storage_;

        template <typename ...U>
        explicit hvector(raw_storage<U...>& storage)
            : storage_(&storage.actual_)
        { }

        // only used by hvector itself
        explicit hvector(void* storage)
            : storage_(storage)
        { }

        template <typename U>
        hvector<T..., U> append(U const& u) {
            using Layout = normal_struct_layout<T..., U>;
            ::new (Layout::raw_nth(storage_, sizeof...(T))) U(u);
            return hvector<T..., U>{storage_};
        }

        template <std::size_t n>
        typename nth_type<n, T...>::type& nth() {
            using Layout = normal_struct_layout<T...>;
            using Nth = typename nth_type<n, T...>::type;
            return *static_cast<Nth*>(Layout::raw_nth(storage_, n));
        }
    };
}} // end namespace boost::hana

#endif // !BOOST_HANA_EXPERIMENTAL_HVECTOR_HPP
