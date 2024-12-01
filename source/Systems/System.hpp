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
        using FamilyType = std::size_t;

        virtual ~BaseSystem() = default;

        virtual void Configure(EventManager& eventManager) = 0;
        virtual void Update(EntityManager& entityManager, EventManager& eventManager, const sf::Time& deltaTime) = 0;

    protected:
        static FamilyType FamilyCounter()
        {
            static FamilyType familyCounter = 0;

            return familyCounter++;
        }
};

template <typename Derived>
class System : public BaseSystem
{
    public:
        ~System() override {}

        void Configure(EventManager& eventManager) override = 0;
        void Update(EntityManager& entityManager, EventManager& eventManager, const sf::Time& deltaTime) override = 0;

    private:
        static FamilyType Family()
        {
            static FamilyType family = FamilyCounter();

            return family;
        }

    private:
        friend class SystemManager;
};
