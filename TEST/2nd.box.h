#pragma once
#include "2nd.point.h"

template<typename T>
class Box
{
public:
    T left;
    T top;
    T width; // Must be positive
    T height; // Must be positive

    constexpr Box(T Left = 0, T Top = 0, T Width = 0, T Height = 0) noexcept :
        left(Left), top(Top), width(Width), height(Height) {}

    constexpr Box(const Point<T>& position, const Point<T>& size) noexcept :
        left(position.x), top(position.y), width(size.x), height(size.y) {}

    constexpr T getRight() const noexcept
    {
        return left + width;
    }

    constexpr T getBottom() const noexcept
    {
        return top + height;
    }

    constexpr Point<T> getTopLeft() const noexcept
    {
        return Point<T>(left, top);
    }

    constexpr Point<T> getCenter() const noexcept
    {
        return Point<T>(left + width / 2, top + height / 2);
    }

    constexpr Point<T> getSize() const noexcept
    {
        return Point<T>(width, height);
    }

    constexpr bool contains(const Box<T>& box) const noexcept
    {
        return left <= box.left && box.getRight() <= getRight() &&
            top <= box.top && box.getBottom() <= getBottom();
    }

    constexpr bool intersects(const Box<T>& box) const noexcept
    {
        return !(left >= box.getRight() || getRight() <= box.left ||
            top >= box.getBottom() || getBottom() <= box.top);
    }
};