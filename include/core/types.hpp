#pragma once

namespace core {

template <typename T>
class Vec2 {
public:
    Vec2(T x, T y) : _x(x), _y(y)
    {
    }

    ~Vec2() = default;

    // copy constructor
    Vec2(Vec2 const& other)
        : _x(other._x), _y(other._y)
    {
    }

    // move constructor
    Vec2(Vec2&& other) noexcept
        : _x(other._x), _y(other._y)
    {
    }

    // copy assignment
    Vec2& operator=(Vec2 const& other)
    {
        if (this == &other) return *this;
        _x = other._x;
        _y = other._y;
        return *this;
    }

    Vec2& operator=(Vec2&& other) noexcept
    {
        _x = other._x;
        _y = other._y;
        return *this;
    }

    Vec2 operator-(Vec2 const& other) const
    {
        return Vec2(_x - other._x, _y - other._y);
    }

    Vec2 operator+(Vec2 const& other) const
    {
        return Vec2(_x + other._x, _y + other._y);
    }

    Vec2& operator+=(Vec2 const& other)
    {
        _x += other._x;
        _y += other._y;
        return *this;
    }

    Vec2& operator-=(Vec2 const& other)
    {
        _x -= other._x;
        _y -= other._y;
    }

private:
    T _x, _y;
};

typedef Vec2<int> vec2i;
typedef Vec2<float> vec2f;

} // namespace core