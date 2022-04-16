#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <numeric>

template <size_t dimensions, typename T>
class Point
{
public:
    std::array<T, dimensions> values = {};
    Point() {};
    Point(float x, float y)
    : values { x, y }
    {}
    Point(float x, float y, float z)
    : values { x, y, z}
    {}
    template<typename... Args>
    explicit Point(Args&&... args)
    : values { std::forward<T>(static_cast<T>(args))... }
    {
        assert(sizeof...(args) == dimensions);
    }
    T x() const
    {
        return values[0];
    }
    T& x()
    {
        return values[0];
    }
    T y() const
    {
        static_assert(dimensions > 1);
        return values[1];
    }
    T& y()
    {
        static_assert(dimensions > 1);
        return values[1];
    }
    T z() const
    {
        static_assert(dimensions > 2);
        return values[2];
    }
    T& z()
    {
        static_assert(dimensions > 2);
        return values[2];
    }
    Point<dimensions, T>& operator+=(const Point<dimensions, T>& other)
    {
        std::transform(values.begin(), values.end(), other.values.begin(), values.begin(), std::plus<>());
        return *this;
    }
    Point<dimensions, T>& operator-=(const Point<dimensions, T>& other)
    {
        std::transform(values.begin(), values.end(), other.values.begin(), values.begin(), std::minus<>());
        return *this;
    }
    Point<dimensions, T>& operator*=(const Point<dimensions, T>& other)
    {
        std::transform(values.begin(), values.end(), other.values.begin(), values.begin(), std::multiplies<>());
        return *this;
    }
    Point<dimensions, T>& operator*=(const T& scalar)
    {
        std::transform(values.begin(), values.end(), values.begin(), [&scalar](const T& value) { return scalar * value; });
        return *this;
    }
    Point<dimensions, T> operator+(const Point<dimensions, T>& other) const
    {
        Point<dimensions, T> result = *this;
        result += other;
        return result;
    }
    Point<dimensions, T> operator-(const Point<dimensions, T>& other) const
    {
        Point<dimensions, T> result = *this;
        result -= other;
        return result;
    }
    Point<dimensions, T> operator*(const Point<dimensions, T>& other) const
    {
        Point<dimensions, T> result = *this;
        result *= other;
        return result;
    }
    Point<dimensions, T> operator*(const T& scalar) const
    {
        Point<dimensions, T> result = *this;
        result *= scalar;
        return result;
    }
    Point<dimensions, T> operator-() const {
        Point<dimensions, T> result = *this;
        std::transform(result.values.begin(), result.values.end(), result.values.begin(), [](const T& value){ return -value; });
        return result;
    }

    float length() const {
        return std::sqrt(std::accumulate(values.begin(), values.end(), 0., [](const auto& acc, const auto& current) { return acc + current * current; }));
    }

    float distance_to(const Point<dimensions, T>& other) const {
        return (*this - other).length();
    }

    Point<dimensions, T>& normalize(const float target_len = 1.0f)
    {
        const float current_len = length();
        if (current_len == 0)
        {
            throw std::logic_error("cannot normalize vector of length 0");
        }
        *this *= (target_len / current_len);
        return *this;
    }
    Point<dimensions, T>& cap_length(const float max_len)
    {
        assert(max_len > 0);
        const float current_len = length();
        if (current_len > max_len)
        {
            *this *= (max_len / current_len);
        }
        return *this;
    }
};

using Point2D = Point<2, float>;
using Point3D = Point<3, float>;

// our 3D-coordinate system will be tied to the airport: the runway is parallel to the x-axis, the z-axis
// points towards the sky, and y is perpendicular to both thus,
// {1,0,0} --> {.5,.5}   {0,1,0} --> {-.5,.5}   {0,0,1} --> {0,1}
inline Point2D project_2D(const Point3D& p)
{
    return { .5f * p.x() - .5f * p.y(), .5f * p.x() + .5f * p.y() + p.z() };
}

//void test_generic_points()
//{
//
//    Point<2, int> p1;
//    Point<2, int> p2;
//    auto p3 = p1 + p2;
//    p1 += p2;
//    p1 *= 3; // ou 3.f, ou 3.0 en fonction du type de Point
//}
