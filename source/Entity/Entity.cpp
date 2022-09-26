#include "Entity.hpp"
#include "EntityManager.hpp"

const Entity::Id Entity::INVALID_ID;

Entity::Entity()
    : internalId(INVALID_ID)
    , entityManager(nullptr)
{}

Entity::Entity(EntityManager* manager, Entity::Id paramId)
    : internalId(paramId)
    , entityManager(manager)
{}

bool Entity::valid() const
{
    return entityManager && entityManager->validEntity(internalId);
}