#pragma once

#include <utility>
#include "System.hpp"
#include "Entity/EntityManager.hpp"
#include "EventManagement/EventManager.hpp"

template <typename SystemType>
void SystemManager::addSystem(std::shared_ptr<SystemType> system)
{
    systems.insert(std::make_pair(SystemType::family(), system));
}

template <typename SystemType, typename ... Args>
std::shared_ptr<SystemType> SystemManager::addSystem(Args&& ... args)
{
    std::shared_ptr<SystemType> system = std::make_shared<SystemType>(std::forward<Args>(args)...);
    addSystem(system);

    return system;
}

template <typename SystemType>
std::shared_ptr<SystemType> SystemManager::getSystem()
{
    auto iter = systems.find(SystemType::family());
    assert(iter != systems.end()); // TODO: Replace with a soft fail and logging 

    return std::static_pointer_cast<SystemType>(iter->second); // TODO: Is the cast needed here?
}

template <typename SystemType>
void SystemManager::update(const sf::Time& deltaTime)
{
    assert(isInitialized && "Trying to call SystemManager::update before you have called SystemManager::configure().");

    getSystem<SystemType>().update(entityManager, eventManager, deltaTime);
}

inline void SystemManager::updateAllSystems(const sf::Time& deltaTime)
{
    assert(isInitialized && "Trying to call SystemManager::update before you have called SystemManager::configure().");

    for (auto& pair : systems)
    {
        pair.second->update(entityManager, eventManager, deltaTime);
    }
}

inline void SystemManager::configure()
{
    for (auto& pair : systems)
    {
        pair.second->configure(eventManager);
    }

    isInitialized = true;
}