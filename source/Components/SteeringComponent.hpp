#pragma once

#include <SFML/System/Vector2.hpp>
#include <type_traits>
#include "Entity/Entity.hpp"

enum class BehaviorType : int
{
    None    = 0x00000,
    Seek    = 0x00002,
    Flee    = 0x00004,
    Arrive  = 0x00008,
};

// TODO: Refactor a lot of the information in here out and into the SteeringSystem
struct SteeringComponent
{
    public:
        enum class Deceleration : int
        {
            Slow = 3,
            Normal = 2,
            Fast = 1,
        };

    public:
        BehaviorType behaviorFlags;

        // Seek
        sf::Vector2f seekTarget;

        // Flee
        sf::Vector2f fleeTarget;
        float fleePanicDistance;

        // Arrive
        sf::Vector2f arrivePosition;
        Deceleration arriveDeceleration;

        // Pursuit
        Entity pursuitTarget;
};

// Scoped enums need to implement their own operators
inline BehaviorType operator&(BehaviorType lhs, BehaviorType rhs)
{
    using Type = std::underlying_type_t<BehaviorType>;

    return static_cast<BehaviorType>(static_cast<Type>(lhs) | static_cast<Type>(rhs));
}

inline BehaviorType& operator|=(BehaviorType& lhs, BehaviorType rhs)
{
    using Type = std::underlying_type_t<BehaviorType>;

    lhs = static_cast<BehaviorType>(static_cast<Type>(lhs) | static_cast<Type>(rhs));

    return lhs;
}