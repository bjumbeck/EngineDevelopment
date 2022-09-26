#pragma once

#include "EventManagement/EventManager.hpp"
#include "Entity/Entity.hpp"

struct EntityCreatedEvent : public Event<EntityCreatedEvent>
{
    explicit EntityCreatedEvent(const Entity& entity) : entity(entity) {}
    virtual ~EntityCreatedEvent() {}

    Entity entity;
};

struct EntityDestroyedEvent : public Event<EntityDestroyedEvent>
{
    explicit EntityDestroyedEvent(const Entity& entity) : entity(entity) {}
    virtual ~EntityDestroyedEvent() {}

    Entity entity;
};