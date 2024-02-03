#pragma once

#include <ranges>

#include "MovableBox.h"

template<class Func>
class generator_view : public std::ranges::view_interface<generator_view<Func>>
{
public:
	class iterator
	{
	public:
		using iterator_concept = std::input_iterator_tag;
		using value_type = std::invoke_result_t<Func>;
		using difference_type = std::ptrdiff_t;

		constexpr explicit iterator(generator_view* p) : parent(p) {}

		constexpr const value_type& operator* () const noexcept { return parent->value; }

		constexpr iterator& operator++() {
			parent->update_value();
			return *this;
		}
		constexpr iterator operator++(int) {
			parent->update_value();
			return *this;
		}

	private:
		generator_view* parent;
	};

	template<class F>
	constexpr generator_view(F&& f) : func(std::move(f)) {
		update_value();
	}

	constexpr auto begin() { return iterator(this); }
	constexpr auto end() { return std::unreachable_sentinel; }

private:
	movable_box<Func> func{};
	std::invoke_result_t<Func> value{};

	void update_value() { value = std::invoke(*func); }
};

namespace views
{
	template<class Func>
	auto generate(Func&& fn)
	{
		return generator_view<std::decay_t<Func>>(std::forward<Func>(fn));
	}
}