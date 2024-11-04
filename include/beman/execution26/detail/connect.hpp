// include/beman/execution26/detail/connect.hpp                       -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_CONNECT
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_CONNECT

#include <beman/execution26/detail/transform_sender.hpp>
#include <beman/execution26/detail/get_domain_late.hpp>
#include <beman/execution26/detail/get_env.hpp>
#include <beman/execution26/detail/operation_state.hpp>
#include <beman/execution26/detail/connect_awaitable.hpp>
#include <type_traits>
#include <concepts>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail {
struct connect_t {
    template <typename Sender, typename Receiver>
    auto operator()(Sender&& sender, Receiver&& receiver) const noexcept(true /*-dk:TODO*/) {
        auto new_sender = [&sender, &receiver]() -> decltype(auto) {
            return ::beman::execution26::transform_sender(
                decltype(::beman::execution26::detail::get_domain_late(::std::forward<Sender>(sender),
                                                                       ::beman::execution26::get_env(receiver))){},
                ::std::forward<Sender>(sender),
                ::beman::execution26::get_env(receiver));
        };

        if constexpr (requires { new_sender().connect(::std::forward<Receiver>(receiver)); }) {
            using state_type = decltype(new_sender().connect(::std::forward<Receiver>(receiver)));
            static_assert(::beman::execution26::operation_state<state_type>);
            return new_sender().connect(::std::forward<Receiver>(receiver));
        } else if constexpr (requires {
                                 ::beman::execution26::detail::connect_awaitable(new_sender(),
                                                                                 ::std::forward<Receiver>(receiver));
                             }) {
            return ::beman::execution26::detail::connect_awaitable(new_sender(), ::std::forward<Receiver>(receiver));
        } else {
            enum bad_sender {};
            static_assert(::std::same_as<bad_sender, decltype(new_sender())>,
                          "result from transform_sender has no suitable connect()");
        }
    }
};
} // namespace beman::execution26::detail

namespace beman::execution26 {
using beman::execution26::detail::connect_t;
/*!
 * \brief Customization point object used to connect a sender and a receiver.
 */
inline constexpr connect_t connect{};
} // namespace beman::execution26

// ----------------------------------------------------------------------------

#endif
