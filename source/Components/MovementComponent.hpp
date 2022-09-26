#pragma once

#include "SFML/System/Vector2.hpp"

struct MovementComponent
{
    MovementComponent(float mass, float maxSpeed, float maxForce, float maxTurnRate)
        : velocity(0.0f, 0.0f)
        , mass(mass)
        , maxSpeed(maxSpeed)
        , maxForce(maxForce)
        , maxTurnRate(maxTurnRate)
    {}
        
    // TODO: Look to move these elsewhere possibly
    sf::Vector2f heading;
    sf::Vector2f side;

    sf::Vector2f velocity;

    float mass;
    float maxSpeed;
    float maxForce;
    float maxTurnRate;
};