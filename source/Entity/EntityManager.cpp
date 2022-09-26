#include "EntityManager.hpp"
#include "EventManagement/Events/EntityEvents.hpp"

EntityManager::EntityManager(EventManager& eventManager)
    : eventManager(eventManager)
{}

EntityManager::~EntityManager()
{
    reset();
}

Entity EntityManager::createEntity()
{
    uint32_t index;
    uint32_t version;

    if (freeIds.empty())
    {
        index = indexCounter++;
        accomodateComponent(index);
        version = entityVersions[index] = 1; // Will always be the first version on this method call.
    }
    else
    {
        index = freeIds.back();
        freeIds.pop_back();
        version = entityVersions[index];
    }

    Entity entity(this, Entity::Id(index, version));
    eventManager.emit<EntityCreatedEvent>(entity);

    return entity;
}

Entity::Id EntityManager::createEntityId(uint32_t index) const
{
    return Entity::Id(index, entityVersions[index]);
}

void EntityManager::destroyEntity(Entity::Id entityId)
{
    assertValidId(entityId);

    uint32_t index = entityId.index();
    auto compMask = entityComponentMasks[index];

    eventManager.emit<EntityDestroyedEvent>(Entity(this, entityId));
    for (size_t i = 0; i < componentPools.size(); ++i)
    {
        BasePool* pool = componentPools[i];
        if (pool && compMask.test(i))
        {
            pool->destroy(index);
        }
    }

    entityComponentMasks[index].reset();
    entityVersions[index]++;
    freeIds.push_back(index);
}

Entity EntityManager::getEntity(Entity::Id entityId)
{
    assertValidId(entityId);

    return Entity(this, entityId);
}

bool EntityManager::validEntity(Entity::Id id) const
{
    return id.index() < entityVersions.size() &&
           entityVersions[id.index()] == id.version();
}

void EntityManager::reset()
{
    // Destroy all entities, entitiesForDebugging() will return 
    // every single entity the manager is tracking.
    for (Entity entity : entitiesForDebugging())
    {
        destroyEntity(entity.id());
    }

    // Delete all the memory pools that hold the components
    // this in turn will delete all the components.
    for (BasePool* pool : componentPools)
    {
        if (pool)
        {
            delete pool;
        }
    }

    componentPools.clear();
    entityComponentMasks.clear();
    entityVersions.clear();
    freeIds.clear();

    indexCounter = 0;
}

EntityManager::DebugView EntityManager::entitiesForDebugging()
{
    return DebugView(this);
}

void EntityManager::assertValidId(Entity::Id id) const
{
    assert(id.index() < entityComponentMasks.size() && "Entity::Id ~ ID is outside the entity vector range");
    assert(entityVersions[id.index()] == id.version() && "Entity::Id ~ Trying to access an Entity through an old(Invalid) Entity::Id");
}

std::vector<EntityManager::ComponentMask> EntityManager::allComponentMasks() const
{
    return entityComponentMasks;
}

EntityManager::ComponentMask EntityManager::componentMask(Entity::Id id)
{
    assertValidId(id);

    return entityComponentMasks.at(id.index());
}

void EntityManager::accomodateComponent(uint32_t index)
{
    if (entityComponentMasks.size() <= index)
    {
        entityComponentMasks.resize(index + 1);
        entityVersions.resize(index + 1);

        for (BasePool* pool : componentPools)
        {
            if (pool)
            {
                pool->expand(index + 1);
            }
        }
    }
}