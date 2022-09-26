#pragma once

#include <SFML/System/Vector2.hpp>

template<typename T>
T DistanceSquared(const sf::Vector2<T>& vec1, const sf::Vector2<T>& vec2);

template<typename T>
T Length(const sf::Vector2<T>& vector);

template<typename T>
T SquaredLength(const sf::Vector2<T>& vector);

template<typename T>
void SetLength(sf::Vector2<T>& vector, T newLength);

template<typename T>
sf::Vector2<T> UnitVector(const sf::Vector2<T>& vector);

template<typename T>
T PolarAngle(const sf::Vector2<T>& vector);

template<typename T>
void SetPolarAngle(sf::Vector2<T>& vector, T newAngle);

template<typename T>
void Rotate(sf::Vector2<T>& vector, T angle);

template<typename T>
sf::Vector2<T> RotatedVector(const sf::Vector2<T>& vector, T angle);

template<typename T>
sf::Vector2<T> PerpendicularVector(const sf::Vector2<T>& vector);

template<typename T>
T SignedAngle(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs);

template<typename T>
T DotProduct(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs);

template<typename T>
T CrossProduct(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs);

template<typename T>
sf::Vector2<T> CwiseProduct(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs);

template<typename T>
sf::Vector2<T> CwiseQuotient(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs);

template<typename T>
sf::Vector2<T> ProjectedVector(const sf::Vector2<T>& vector, const sf::Vector2<T>& axis);

#include "VectorMath.inl"