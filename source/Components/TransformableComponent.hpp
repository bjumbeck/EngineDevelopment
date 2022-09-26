#pragma once

#include <SFML/Graphics/Transformable.hpp>
#include "SFML/System/Vector2.hpp"

struct TransformableComponent : public sf::Transformable
{
    TransformableComponent() = default;
    TransformableComponent(const sf::Vector2f& position,
                           const float rotation = 0.0f,
                           const sf::Vector2f& scale = sf::Vector2f(1.0f, 1.0f),
                           const sf::Vector2f& origin = sf::Vector2f())
    {
        setPosition(position);
        setRotation(rotation);
        setScale(scale);
        setOrigin(origin);
    }
};
