#pragma once

/*
Wraps move constructible value, enabling move assignments as well.
Used for holding function object, when implementing views
(as lambdas are not move assignable, but the view containing them is required to be)
*/
template<class T> requires std::is_move_constructible_v<T>
class movable_box
{
public:
    constexpr movable_box() requires std::default_initializable<T>
        : value()
        , engaged(true)
    {}

    constexpr explicit movable_box(T&& val)
        : value(std::forward<T>(val))
        , engaged(true)
    {}

    constexpr ~movable_box() {
        if (engaged) {
            std::destroy_at(std::addressof(value));
        }
    }

    constexpr movable_box(movable_box&& rhs)
        : engaged(rhs.engaged)
    {
        if (engaged) {
            std::construct_at(std::addressof(value), std::forward<T>(rhs.value));
        }
    };

    constexpr movable_box& operator=(movable_box&& rhs) {
        if (std::addressof(rhs) != this) {
            if (engaged) {
                std::destroy_at(std::addressof(value));
                engaged = false;
            }
            if (rhs.engaged) {
                std::construct_at(std::addressof(value), std::forward<T>(rhs.value));
                engaged = true;
            }
        }
        return *this;
    }

    [[nodiscard]] constexpr bool has_value() const noexcept {
        return engaged;
    }

    [[nodiscard]] constexpr T& operator*() noexcept {
        return value;
    }

    [[nodiscard]] constexpr const T& operator*() const noexcept {
        return value;
    }

private:
    union {
        std::remove_cv_t<T> value;
    };
    bool engaged;
};