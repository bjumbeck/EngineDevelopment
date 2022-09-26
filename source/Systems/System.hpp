#pragma once

#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>
#include <cstddef>

class SystemManager;
class EventManager;
class EntityManager;

class BaseSystem : private sf::NonCopyable
{
    public:
        using Family = std::size_t;

        virtual ~BaseSystem() {}

        virtual void configure(EventManager& eventManager) = 0;
        virtual void update(EntityManager& entityManager, EventManager& eventManager, const sf::Time& deltaTime) = 0;

    protected:
        static Family familyCounter()
        {
            static Family familyCounter = 0;

            return familyCounter++;
        }
};

template <typename Derived>
class System : public BaseSystem
{
    public:
        virtual ~System() {}

    private:
        static Family family() 
        { 
            static Family family = familyCounter(); 
            
            return family; 
        }

    private:
        friend class SystemManager;
};