#ifndef CTEP_PACK_HPP
#define CTEP_PACK_HPP

#include <type_traits>
#include <utility>

#define DISABLE_EXPLICIT int &...

namespace ctep {

template <typename... Ts> struct pack {};

template <DISABLE_EXPLICIT, typename... Us>
consteval auto make_pack(Us...) noexcept {
  return pack<Us...>{};
}

template <DISABLE_EXPLICIT, typename U, typename... Us>
consteval auto append(pack<Us...>, U) noexcept {
  return pack<Us..., U>{};
}

namespace detail {

// TODO(samuel): Non-recursive method

template <DISABLE_EXPLICIT, typename U, typename... Us>
consteval auto get(pack<U, Us...>, std::integral_constant<size_t, 0>) noexcept {
  return U{};
}

template <DISABLE_EXPLICIT, size_t N, typename U, typename... Us>
consteval auto get(pack<U, Us...>, std::integral_constant<size_t, N>) noexcept {
  return get(pack<Us...>{}, std::integral_constant<size_t, N - 1>{});
}

} // namespace detail

template <size_t N, DISABLE_EXPLICIT, typename U, typename... Us>
consteval auto get(pack<U, Us...>) noexcept {
  return detail::get(pack<U, Us...>{}, std::integral_constant<size_t, N>{});
}

template <size_t N> consteval auto get(pack<>) noexcept = delete;

namespace detail {

template <DISABLE_EXPLICIT, size_t... Is, typename... Us>
consteval auto pop_back(pack<Us...> pack, std::index_sequence<Is...>) noexcept {
  return make_pack(get<Is>(pack)...);
}

} // namespace detail

template <DISABLE_EXPLICIT, typename... Us>
consteval auto pop_back(pack<Us...> pack) noexcept {
  return detail::pop_back(pack, std::make_index_sequence<sizeof...(Us) - 1>{});
}

template <DISABLE_EXPLICIT, typename U, typename... Us>
consteval auto pop_front(pack<U, Us...>) noexcept {
  return pack<Us...>{};
}

} // namespace ctep

#endif