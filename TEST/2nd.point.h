#pragma once
template<typename T>
class Point
{
public:
    T x;
    T y;
    constexpr Point<T>(T X = 0, T Y = 0) noexcept : x(X), y(Y) {}
    constexpr Point<T>& operator+=(const Point<T>& other) noexcept
    {
        x += other.x;
        y += other.y;
        return *this;
    }
    constexpr Point<T>& operator/=(T t) noexcept
    {
        x /= t;
        y /= t;
        return *this;
    }
};

template<typename T>
constexpr Point<T> operator+(Point<T> lhs, const Point<T>& rhs) noexcept
{
    lhs += rhs;
    return lhs;
}

template<typename T>
constexpr Point<T> operator/(Point<T> vec, T t) noexcept
{
    vec /= t;
    return vec;
}