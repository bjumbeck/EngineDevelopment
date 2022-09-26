#pragma once

#include "Components/Component.hpp"
#include "EventManagement/Events/ComponentEvents.hpp"

template <typename CompType>
size_t EntityManager::componentFamily()
{
    return Component<typename std::remove_const_t<CompType>>::family();
}

template <typename CompType, typename ... Args>
ComponentPtr<CompType> EntityManager::assignComponent(Entity::Id id, Args&& ... args)
{
    assertValidId(id);

    const BaseComponent::Family family = componentFamily<CompType>();
    assert(!entityComponentMasks[id.index()].test(family));

    // Add it into a memory pool for the component family
    Pool<CompType>* pool = accomodateComponent<CompType>();
    new(pool->get(id.index())) CompType(std::forward<Args>(args) ...);

    // Set the bit for the component
    entityComponentMasks[id.index()].set(family);

    // Create the component
    ComponentPtr<CompType> component(this, id);
    eventManager.emit<ComponentAddedEvent<CompType>>(Entity(this, id), component);

    return component;
}

template <typename CompType>
void EntityManager::removeComponent(Entity::Id id)
{
    assertValidId(id);

    const BaseComponent::Family family = componentFamily<CompType>();
    const uint32_t index = id.index();

    BasePool* pool = componentPools[family];
    ComponentPtr<CompType> component(this, id);
    eventManager.emit<ComponentRemovedEvent<CompType>>(Entity(this, id), component);

    entityComponentMasks[id.index()].reset(family);
    pool->destroy(index);
}

template <typename CompType>
bool EntityManager::hasComponent(Entity::Id id) const
{
    assertValidId(id);

    const BaseComponent::Family family = componentFamily<CompType>();
    if (family >= componentPools.size())
    {
        return false;
    }

    BasePool* pool = componentPools[family];
    if (!pool || !entityComponentMasks[id.index()][family])
    {
        return false;
    }

    return true;
}

template <typename CompType, typename>
ComponentPtr<CompType> EntityManager::getComponent(Entity::Id id)
{
    assertValidId(id);

    const BaseComponent::Family family = componentFamily<CompType>();
    if (family >= componentPools.size())
    {
        return ComponentPtr<CompType>();
    }

    BasePool* pool = componentPools[family];
    if (!pool || !entityComponentMasks[id.index()][family])
    {
        return ComponentPtr<CompType>();
    }

    return ComponentPtr<CompType>(this, id);
}

template <typename CompType, typename>
const ComponentPtr<CompType, const EntityManager> EntityManager::getComponent(Entity::Id id) const
{
    assertValidId(id);

    const BaseComponent::Family family = componentFamily<CompType>();
    if (family >= componentPools.size())
    {
        return ComponentPtr<CompType, const EntityManager>();
    }

    BasePool* pool = componentPools[family];
    if (!pool || !entityComponentMasks[id.index()][family])
    {
        return ComponentPtr<CompType, const EntityManager>();
    }

    return ComponentPtr<CompType, const EntityManager>(this, id);
}

template <typename ... Components>
std::tuple<ComponentPtr<Components>...> EntityManager::getComponents(Entity::Id id)
{
    return std::make_tuple(getComponent<Components>(id)...);
}

template <typename ... Components>
std::tuple<ComponentPtr<const Components, const EntityManager>...> EntityManager::getComponents(Entity::Id id) const
{
    return std::make_tuple(getComponent<const Components>(id)...);
}

template <typename ... Components>
EntityManager::View EntityManager::getEntitiesWithComponents()
{
    auto compMask = componentMask<Components...>();

    return View(this, compMask);
}

template <typename ... Components>
EntityManager::UnpackingView<Components...> EntityManager::getEntitiesWithComponents(ComponentPtr<Components>& ... components)
{
    auto compMask = componentMask<Components...>();

    return UnpackingView<Components...>(this, compMask, components...);
}

template <typename CompType>
void EntityManager::unpack(Entity::Id id, ComponentPtr<CompType>& outputParam)
{
    assertValidId(id);

    outputParam = getComponent<CompType>(id);
}

template <typename CompOne, typename ... CompArgs>
void EntityManager::unpack(Entity::Id id, ComponentPtr<CompOne>& outputCompOne, ComponentPtr<CompArgs>& ... compArgs)
{
    assertValidId(id);

    outputCompOne = getComponent<CompOne>(id);
    unpack<CompArgs...>(id, compArgs...);
}

template <typename CompType>
CompType* EntityManager::getComponentPtr(Entity::Id id)
{
    assertValidId(id);

    BasePool* pool = componentPools[componentFamily<CompType>()];
    assert(pool);

    return static_cast<CompType*>(pool->get(id.index()));
}

template <typename CompType>
const CompType* EntityManager::getComponentPtr(Entity::Id id) const
{
    assertValidId(id);

    BasePool* pool = componentPools[componentFamily<CompType>()];
    assert(pool);

    return static_cast<const CompType*>(pool->get(id.index()));
}

template <typename CompType>
EntityManager::ComponentMask EntityManager::componentMask()
{
    ComponentMask compMask;
    compMask.set(componentFamily<CompType>());

    return compMask;
}

template <typename CompType1, typename CompType2, typename ... CompTypeArgs>
EntityManager::ComponentMask EntityManager::componentMask()
{
    return componentMask<CompType1>() | componentMask<CompType2, CompTypeArgs...>();
}

template <typename CompType>
EntityManager::ComponentMask EntityManager::componentMask(const ComponentPtr<CompType>& comp)
{
    return componentMask<CompType>();
}

template <typename CompType1, typename ... CompTypeArgs>
EntityManager::ComponentMask EntityManager::componentMask(const ComponentPtr<CompType1>& compOne, const ComponentPtr<CompTypeArgs>& ... args)
{
    return componentMask<CompType1, CompTypeArgs...>();
}

template <typename CompType>
Pool<CompType>* EntityManager::accomodateComponent()
{
    BaseComponent::Family family = componentFamily<CompType>();
    if (componentPools.size() <= family)
    {
        componentPools.resize(family + 1, nullptr);
    }

    if (!componentPools[family])
    {
        Pool<CompType>* pool = new Pool<CompType>();
        pool->expand(indexCounter);
        componentPools[family] = pool;
    }

    return static_cast<Pool<CompType>*>(componentPools[family]);
}