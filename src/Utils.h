#pragma once

#include <ranges>
#include <algorithm>

namespace stdr = std::ranges;
namespace stdv = std::views;

template<class R, class V>
concept range_of = stdr::range<R> && std::convertible_to<stdr::range_value_t<R>, V>;