#pragma once

#include <cstdint>

class EntityManager;

class Entity
{
    public:
        // Id implementation inspired by the entityX framework, all credit of 
        // the representation of Id goes to them.
        struct Id
        {
            Id() : internalId(0) {}
            explicit Id(std::uint64_t id) : internalId(id) {}
            Id(std::uint32_t idIndex, std::uint32_t idVersion)
                : internalId(static_cast<std::uint64_t>(idIndex) | static_cast<std::uint64_t>(idVersion) << 32UL) 
            {}

            std::uint64_t getId() const { return internalId; }
            std::uint32_t index() const { return internalId & 0xffffffffUL; }
            std::uint32_t version() const { return internalId >> 32; }
            
            bool operator==(const Id& other) const { return internalId == other.internalId; }
            bool operator!=(const Id& other) const { return internalId != other.internalId; }
            bool operator<(const Id& other) const { return internalId < other.internalId; }

            private:
                std::uint64_t internalId;
        };

    public:
        static const Entity::Id INVALID_ID;

        Entity();
        Entity(EntityManager* manager, Entity::Id paramId);
        Entity(const Entity& other) = default;
        Entity& operator=(const Entity& other) = default;

        bool valid() const;
        Entity::Id id() const { return internalId; }

        // Convience operator overloads
        operator bool() const { return valid(); }
        bool operator==(const Entity& other) const { return other.internalId == internalId; }
        bool operator!=(const Entity& other) const { return other.internalId != internalId; }
        bool operator<(const Entity& other) const { return other.internalId < internalId; }

    private:
        Entity::Id internalId;
        EntityManager* entityManager;
};