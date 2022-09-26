#pragma once

// FIXME: Refactor to use std::unique_ptr. There is no reason why
// we should be using std::shared_ptr here since even though other
// systems might need to access other systems (Not ideal) they by 
// no means should be in a ownership position. That ownership position
// falls solely on the SystemManager. So we can use unique_ptr and 
// raw non-owning pointers instead.

#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>
#include <unordered_map>
#include <memory>

class BaseSystem;
class EventManager;
class EntityManager;

class SystemManager : private sf::NonCopyable
{
    public:
        SystemManager(EntityManager& entityManager, EventManager& eventManager)
            : isInitialized(false)
            , entityManager(entityManager)
            , eventManager(eventManager)
        {}

        template <typename SystemType>
        void addSystem(std::shared_ptr<SystemType> system);

        template <typename SystemType, typename ... Args>
        std::shared_ptr<SystemType> addSystem(Args&& ... args);

        template <typename SystemType>
        std::shared_ptr<SystemType> getSystem();

        template <typename SystemType>
        void update(const sf::Time& deltaTime);
        
        void updateAllSystems(const sf::Time& deltaTime);

        void configure();

    private:
        bool isInitialized;
        EntityManager& entityManager;
        EventManager& eventManager;
        std::unordered_map<std::size_t, std::shared_ptr<BaseSystem>> systems;
};

#include "SystemManager.inl"