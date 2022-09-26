#pragma once

#include "Trigonometry.hpp"
#include <cassert>

template<typename T>
T DistanceSquared(const sf::Vector2<T>& vec1, const sf::Vector2<T>& vec2)
{
    T xSeparation = vec2.x - vec1.x;
    T ySeparation = vec2.y - vec1.y;

    return ySeparation * ySeparation + xSeparation * xSeparation;
}

template<typename T>
T Length(const sf::Vector2 <T>& vector)
{
    return TrigonometricTraits<T>::Sqrt(SquaredLength(vector));
}

template<typename T>
T SquaredLength(const sf::Vector2 <T>& vector)
{
    return DotProduct(vector, vector);
}

template<typename T>
void SetLength(sf::Vector2 <T>& vector, T newLength)
{
    assert(vector != sf::Vector2<T>());
    vector *= newLength / Length(vector);
}

template<typename T>
sf::Vector2 <T> UnitVector(const sf::Vector2 <T>& vector)
{
    assert(vector != sf::Vector2<T>());
    return vector / Length(vector);
}

template<typename T>
T PolarAngle(const sf::Vector2 <T>& vector)
{
    assert(vector != sf::Vector2<T>());
    return TrigonometricTraits<T>::ArcTan2(vector.y, vector.x);
}

template<typename T>
void SetPolarAngle(sf::Vector2 <T>& vector, T newAngle)
{
    // No assert here, because turning a zero vector is well-defined (yields always zero vector)

    T vecLength = Length(vector);

    vector.x = vecLength * TrigonometricTraits<T>::Cos(newAngle);
    vector.y = vecLength * TrigonometricTraits<T>::Sin(newAngle);
}

template<typename T>
void Rotate(sf::Vector2 <T>& vector, T angle)
{
    // No assert here, because turning a zero vector is well-defined (yields always zero vector)

    T cos = TrigonometricTraits<T>::Cos(angle);
    T sin = TrigonometricTraits<T>::Sin(angle);

    // Don't manipulate x and y separately, otherwise they're overwritten too early
    vector = sf::Vector2<T>(
            cos * vector.x - sin * vector.y,
            sin * vector.x + cos * vector.y);
}

template<typename T>
sf::Vector2 <T> RotatedVector(const sf::Vector2 <T>& vector, T angle)
{
    // No assert here, because turning a zero vector is well-defined (yields always zero vector)

    sf::Vector2 <T> copy = vector;
    Rotate(copy, angle);
    return copy;
}

template<typename T>
sf::Vector2 <T> PerpendicularVector(const sf::Vector2 <T>& vector)
{
    return sf::Vector2<T>(-vector.y, vector.x);
}

// ---------------------------------------------------------------------------------------------------------------------------


template<typename T>
T SignedAngle(const sf::Vector2 <T>& lhs, const sf::Vector2 <T>& rhs)
{
    assert(lhs != sf::Vector2<T>() && rhs != sf::Vector2<T>());
    return TrigonometricTraits<T>::ArcTan2(CrossProduct(lhs, rhs), DotProduct(lhs, rhs));
}

template<typename T>
T DotProduct(const sf::Vector2 <T>& lhs, const sf::Vector2 <T>& rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

template<typename T>
T CrossProduct(const sf::Vector2 <T>& lhs, const sf::Vector2 <T>& rhs)
{
    return lhs.x * rhs.y - lhs.y * rhs.x;
}

template<typename T>
sf::Vector2 <T> CwiseProduct(const sf::Vector2 <T>& lhs, const sf::Vector2 <T>& rhs)
{
    return sf::Vector2<T>(lhs.x * rhs.x, lhs.y * rhs.y);
}

template<typename T>
sf::Vector2 <T> CwiseQuotient(const sf::Vector2 <T>& lhs, const sf::Vector2 <T>& rhs)
{
    assert(rhs.x != 0 && rhs.y != 0);
    return sf::Vector2<T>(lhs.x / rhs.x, lhs.y / rhs.y);
}

template<typename T>
sf::Vector2 <T> ProjectedVector(const sf::Vector2 <T>& vector, const sf::Vector2 <T>& axis)
{
    assert(axis != sf::Vector2<T>());
    return DotProduct(vector, axis) / SquaredLength(axis) * axis;
}