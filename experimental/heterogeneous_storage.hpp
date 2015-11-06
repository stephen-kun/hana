/*
@copyright Louis Dionne 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_EXPERIMENTAL_HETEROGENEOUS_STORAGE_HPP
#define BOOST_HANA_EXPERIMENTAL_HETEROGENEOUS_STORAGE_HPP

#include <cstddef>
#include <new>
#include <type_traits>

#include <boost/hana/detail/array.hpp>


namespace boost { namespace hana {
    struct uninitialized { };

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
    struct heterogeneous_storage {
    private:
        static constexpr detail::array<std::size_t, sizeof...(T)> sizes = {{sizeof(T)...}};
        static constexpr detail::array<std::size_t, sizeof...(T)> alignments = {{alignof(T)...}};

        static constexpr detail::array<std::size_t, sizeof...(T)> offsets_impl() {
            detail::array<std::size_t, sizeof...(T)> offsets{};
            offsets[0] = 0;
            for (std::size_t i = 1; i < sizeof...(T); ++i) {
                // Padd the current member so it is placed at an offset
                // which is a multiple of its alignment.
                offsets[i] = offsets[i-1] + sizes[i-1] +
                             ((offsets[i-1] + sizes[i-1]) % alignments[i]);
            }

            return offsets;
        }

        static constexpr detail::array<std::size_t, sizeof...(T)> offsets = offsets_impl();

        static constexpr std::size_t total_size = offsets[sizeof...(T)-1] + sizes[sizeof...(T)-1];

        // TODO:
        // We could compute an alignment requirement that might be smaller
        // than the default one for std::aligned_storage.
        typename std::aligned_storage<total_size>::type storage_;

    public:
        constexpr void* raw_nth(std::size_t n) {
            if (n >= sizeof...(T))
                throw "out of bounds access in a heterogeneous_storage";
            return ((char*)&storage_) + offsets[n];
        }

        constexpr void const* raw_nth(std::size_t n) const {
            if (n >= sizeof...(T))
                throw "out of bounds access in a heterogeneous_storage";
            return ((char const*)&storage_) + offsets[n];
        }

        template <std::size_t n>
        constexpr typename nth_type<n, T...>::type* nth() {
            using Nth = typename nth_type<n, T...>::type;
            return static_cast<Nth*>(this->raw_nth(n));
        }

        template <std::size_t n>
        constexpr typename nth_type<n, T...>::type const* nth() const {
            using Nth = typename nth_type<n, T...>::type;
            return static_cast<Nth const*>(this->raw_nth(n));
        }

        explicit constexpr heterogeneous_storage(hana::uninitialized&&) { }
        explicit constexpr heterogeneous_storage(hana::uninitialized const&) { }

        /* constexpr */ heterogeneous_storage() {
            std::size_t i = 0;
            void* expand[] = {::new (this->raw_nth(i++)) T()...};
            (void)expand;
        }

        // Note: Placement-new can't be constexpr
        explicit /* constexpr */ heterogeneous_storage(T const& ...args) {
            std::size_t i = 0;
            void* expand[] = {::new (this->raw_nth(i++)) T(args)...};
            (void)expand;
        }

        /* constexpr */ heterogeneous_storage(heterogeneous_storage const& other) {
            std::size_t i = 0;
            std::size_t expand[] = {
                (::new (this->raw_nth(i)) T(*static_cast<T const*>(other.raw_nth(i))), ++i)...
            };
            (void)expand;
        }

        /* constexpr */ heterogeneous_storage(heterogeneous_storage&& other) {
            std::size_t i = 0;
            std::size_t expand[] = {
                (::new (this->raw_nth(i)) T(static_cast<T&&>(*static_cast<T*>(other.raw_nth(i)))), ++i)...
            };
            (void)expand;
        }

        ~heterogeneous_storage() {
            std::size_t i = 0;
            int expand[] = {(static_cast<T*>(this->raw_nth(i++))->~T(), int{})...};
            (void)expand;
        }
    };

    template <typename ...T>
    constexpr detail::array<std::size_t, sizeof...(T)> heterogeneous_storage<T...>::offsets;

    template <>
    struct heterogeneous_storage<> {
        constexpr heterogeneous_storage() { }

        template <bool always_false = false>
        constexpr auto raw_nth(std::size_t) {
            static_assert(always_false,
            "hana::heterogeneous_storage::raw_nth must not be called on an "
            "empty heterogeneous_storage, because that would be an "
            "out-of-bounds access");
        }

        template <std::size_t n, bool always_false = false>
        constexpr auto nth() {
            static_assert(always_false,
            "hana::heterogeneous_storage::nth must not be called on an "
            "empty heterogeneous_storage, because that would be an "
            "out-of-bounds access");
        }
    };
}} // end namespace boost::hana

#endif // !BOOST_HANA_EXPERIMENTAL_HETEROGENEOUS_STORAGE_HPP
