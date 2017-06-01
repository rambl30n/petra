#pragma once

#include "dispatch/utilities/fold.hpp"
#include <tuple>

namespace dispatch {
namespace detail {

}  // namespace detail

  template<typename Tuple>
  static constexpr decltype(auto) tuple_size(Tuple&&) {
    return std::tuple_size<std::decay_t<Tuple>>{};
  }

  // check if an element is in the tuple
  template<typename Integral, typename T>
  constexpr bool in_tuple(const Integral& i, const T& t) {
    constexpr auto x = [](auto&& i, auto&&... args) {
      return in_sequence(i, args...);
    };
    if constexpr (std::tuple_size<std::decay_t<T>>{} == 0) {
      return false;
    } else {
      return std::apply(x, std::tuple_cat(std::make_tuple(i), t));
    }
  }

  template <std::size_t Skip, typename T, std::size_t ...I>
  constexpr auto get_elements(T&& t, std::index_sequence<I...>) {
    return std::make_tuple(std::get<I + Skip>(t)...);
  }

  template <std::size_t I, typename T>
  constexpr auto get_elements_until(T&& t) {
    return get_elements<0>(t, std::make_index_sequence<I>{});
  };


  template <std::size_t I, typename T>
  constexpr auto get_elements_after(T&& t) {
    constexpr std::size_t size = std::tuple_size<std::decay_t<T>>{};
    if constexpr (I + 1 > size) {
      return std::make_tuple();
    } else {
      return get_elements<I + 1>(t, std::make_index_sequence<size - I - 1>{});
    }
  }

  template<typename... Ts>
  constexpr auto pop_front(std::tuple<Ts...>&& t) {
    return std::make_pair(std::get<0>(t), get_elements_after<0>(t));
  }

  template<typename Tuple, typename Elem>
  static constexpr auto append(Tuple&& t, Elem && e) {
    return std::tuple_cat(t, std::make_tuple(e));
  }

  template<std::size_t I, typename T, typename Elem>
  static constexpr auto insert_at(T&& t, Elem&& elem) {
    return std::tuple_cat(append(get_elements_until<I>(t), elem), get_elements_after<I>(t));
  }

  template<std::size_t I, typename... Inputs>
  static constexpr decltype(auto) split_on_index(Inputs&&... inputs) {
    constexpr auto f = [](auto&& result, auto&& p) {
      return append(result, std::get<I>(p));
    };

    return fold_left(f, std::make_tuple(), inputs...);
  }

  template<typename ...Keys, typename ...Values>
  static constexpr decltype(auto) split_pairs(
      std::pair<Keys, Values>&&... pairs) {
    return std::make_pair(
        split_on_index<0>(pairs...),
        split_on_index<1>(pairs...));
  }

}  // namespace dispatch