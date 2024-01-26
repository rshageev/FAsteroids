#pragma once

#include <ranges>
#include <algorithm>

namespace stdr = std::ranges;
namespace stdv = std::views;

template<class R, class V>
concept range_of = stdr::range<R> && std::convertible_to<stdr::range_value_t<R>, V>;

struct translate_rotate
{
	sf::Vector2f offset;
	sf::Angle rotation;

	sf::Vector2f operator() (sf::Vector2f pt) const {
		return offset + pt.rotatedBy(rotation);
	}
};