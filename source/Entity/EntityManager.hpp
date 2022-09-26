#pragma once

// TODO: This file and the inline file is a bit of a mess at the moment, clean this up 
// when you have a few hours to make the design more manageable.

#include <SFML/System/NonCopyable.hpp>
#include <cstdint>
#include <vector>
#include <bitset>
#include <iterator>
#include <algorithm>
#include <tuple>

#include "Helpers/MemoryPool.hpp"
#include "Entity.hpp"
#include "EventManagement/EventManager.hpp"
#include "Components/Component.hpp"


class EntityManager : private sf::NonCopyable
{
    public:
        using ComponentMask = std::bitset<64>; // TODO: Move to configuration file when it is up and running

        //************************************************
        // Note: All this below is just to make working
        // with the entity manager more easy. 
        //************************************************

        // An iterator over the entities in EntityManager (Through the views below)
        // If All is true then it will iterate over all entities and  
        // ignore entity masks.
        template<class Delegate, bool All = false>
        class ViewIterator : public std::iterator<std::input_iterator_tag, Entity::Id>
        {
            public:
                Delegate& operator++()
                {
                    ++idIndex;
                    next();

                    return *static_cast<Delegate*>(this);
                }

                bool operator==(const Delegate& rhs) const { return idIndex == rhs.idIndex; }
                bool operator!=(const Delegate& rhs) const { return idIndex != rhs.idIndex; }
                Entity operator*() { return Entity(entityManager, entityManager->createEntityId(idIndex)); }
                const Entity operator*() const { return Entity(entityManager, entityManager->createEntityId(idIndex)); }

            protected:
                ViewIterator(EntityManager* manager, uint32_t index)
                    : entityManager(manager)
                    , idIndex(index)
                    , capacity(entityManager->capacity())
                    , freeCursor(~0UL)
                {
                    if (All)
                    {
                        std::sort(entityManager->freeIds.begin(), entityManager.freeIds.end()); //-V539
                        freeCursor = 0;
                    }
                }

                ViewIterator(EntityManager* manager, const EntityManager::ComponentMask mask, uint32_t index)
                    : entityManager(manager)
                    , compMask(mask)
                    , idIndex(index)
                    , capacity(entityManager->capacity())
                    , freeCursor(~0UL)
                {
                    if (All)
                    {
                        std::sort(entityManager->freeIds.begin(), entityManager->freeIds.end());
                        freeCursor = 0;
                    }
                }

                void next()
                {
                    while (idIndex < capacity && !predicate())
                    {
                        ++idIndex;
                    }

                    if (idIndex < capacity)
                    {
                        Entity entity = entityManager->getEntity(entityManager->createEntityId(idIndex));
                        static_cast<Delegate*>(this)->nextEntity(entity);
                    }
                }

                inline bool predicate()
                {
                    return (All && validEntity()) || 
                           (entityManager->entityComponentMasks.at(idIndex) & compMask) == compMask;
                }

                inline bool validEntity()
                {
                    const std::vector<uint32_t>& freeVec = entityManager->freeIds;
                    if (freeCursor < freeVec.size() && freeVec[freeCursor] == idIndex)
                    {
                        ++freeCursor;

                        return false;
                    }

                    return true;
                }

            public:
                EntityManager* entityManager;
                EntityManager::ComponentMask compMask;
                uint32_t idIndex;
                size_t capacity;
                size_t freeCursor;
        };

        template <bool All>
        class BaseView
        {
            public:
                class Iterator : public ViewIterator<Iterator, All>
                {
                    public:
                        Iterator(EntityManager* manager,
                                const EntityManager::ComponentMask mask,
                                uint32_t index)
                            : ViewIterator<Iterator, All>(manager, mask, index)
                        {
                            ViewIterator<Iterator, All>::next();
                        }

                        void nextEntity(Entity& entity) {}
                };

                Iterator begin() { return Iterator(entityManager, compMask, 0); }
                Iterator end() { return Iterator(entityManager, compMask, static_cast<uint32_t>(entityManager->capacity())); }
                const Iterator begin() const { return Iterator(entityManager, compMask, 0); }
                const Iterator end() const { return Iterator(entityManager, compMask, entityManager->capacity()); }

            private:
                friend class EntityManager;

                explicit BaseView(EntityManager* manager)
                    : entityManager(manager)
                {
                    compMask.set();
                }

                BaseView(EntityManager* manager, EntityManager::ComponentMask mask)
                    : entityManager(manager)
                    , compMask(mask)
                {}

            private:
                EntityManager* entityManager;
                EntityManager::ComponentMask compMask;
        };

        typedef BaseView<false> View;
        typedef BaseView<true> DebugView;

        template <typename ... Components>
        class UnpackingView
        {
            public:
                class Unpacker
                {
                    public:
                        explicit Unpacker(EntityManager* manager, ComponentPtr<Components>& ... ptrs)
                            : eManager(manager)
                            , compPtrs(std::tuple<ComponentPtr<Components>& ...>(ptrs...))
                        {}

                        void unpack(const Entity& entity) const
                        {
                            unpackImpl<0, Components...>(entity);
                        }

                    private:
                        template <int N, typename CompType>
                        void unpackImpl(Entity entity) const
                        {
                            std::get<N>(compPtrs) = eManager->getComponent<CompType>(entity.id());
                        }

                        template <int N, typename Comp1, typename Comp2, typename ... CompN>
                        void unpackImpl(Entity entity) const
                        {
                            std::get<N>(compPtrs) = eManager->getComponent<Comp1>(entity.id());
                            unpackImpl<N + 1, Comp2, CompN...>(entity);
                        }

                    private:
                        std::tuple<ComponentPtr<Components>& ...> compPtrs;
                        EntityManager* eManager;
                };

                class Iterator : public ViewIterator<Iterator>
                {
                    public:
                        Iterator(EntityManager* manager,
                                const EntityManager::ComponentMask mask,
                                uint32_t index,
                                const Unpacker& unpacker)
                            : ViewIterator<Iterator>(manager, mask, index)
                            , unpacker(unpacker)
                        {
                            ViewIterator<Iterator>::next();
                        }

                        void nextEntity(Entity& entity)
                        {
                            unpacker.unpack(entity);
                        }

                    private:
                        const Unpacker& unpacker;
                };

            public:
                Iterator begin() { return Iterator(manager, compMask, 0, unpacker); }
                Iterator end() { return Iterator(manager, compMask, manager->capacity(), unpacker); }
                const Iterator begin() const { return Iterator(manager, compMask, 0, unpacker); }
                const Iterator end() const { return Iterator(manager, compMask, manager->capacity(), unpacker); }

            private:
                UnpackingView(EntityManager* manager, EntityManager::ComponentMask mask, ComponentPtr<Components>& ... ptrs)
                    : manager(manager)
                    , compMask(mask)
                    , unpacker(manager, ptrs...)
                {}

            private:
                friend class EntityManager;

                EntityManager* manager;
                EntityManager::ComponentMask compMask;
                Unpacker unpacker;
        };

    public:
        explicit EntityManager(EventManager& eventManager);
        ~EntityManager();

        // Entity Management
        Entity createEntity();
        Entity::Id createEntityId(uint32_t index) const;
        void destroyEntity(Entity::Id entityId);
        Entity getEntity(Entity::Id entityId);
        bool validEntity(Entity::Id id) const;

        // Container Management
        void reset();
        size_t size() const { return entityComponentMasks.size() - freeIds.size(); }
        size_t capacity() const { return entityComponentMasks.size(); }

        // Entity Component Management
        template <typename CompType>
        static size_t componentFamily();

        template <typename CompType, typename ... Args>
        ComponentPtr<CompType> assignComponent(Entity::Id id, Args&& ... args);

        template <typename CompType>
        void removeComponent(Entity::Id id);

        template <typename CompType>
        bool hasComponent(Entity::Id id) const;

        template <typename CompType, typename = std::enable_if_t<!std::is_const<CompType>::value>>
        ComponentPtr<CompType> getComponent(Entity::Id id);

        template <typename CompType, typename = std::enable_if_t<std::is_const<CompType>::value>>
        const ComponentPtr<CompType, const EntityManager> getComponent(Entity::Id id) const;

        template <typename ... Components>
        std::tuple<ComponentPtr<Components>...> getComponents(const Entity::Id id);

        template <typename ... Components>
        std::tuple<ComponentPtr<const Components, const EntityManager>...> getComponents(Entity::Id id) const;

        // TODO: Refactor so that we can retun this as View (Typedef of BaseView<false>) instead
        template <typename ... Components>
        BaseView<false> getEntitiesWithComponents();

        template <typename ... Components>
        UnpackingView<Components...> getEntitiesWithComponents(ComponentPtr<Components>& ... components);

        template <typename CompType>
        void unpack(Entity::Id id, ComponentPtr<CompType>& outputParam);

        template <typename CompOne, typename ... CompArgs>
        void unpack(Entity::Id id, ComponentPtr<CompOne>& outputCompOne, ComponentPtr<CompArgs>& ... compArgs);

    private:
        BaseView<true> entitiesForDebugging();
        void assertValidId(Entity::Id id) const;

        template <typename CompType>
        CompType* getComponentPtr(Entity::Id id);

        template <typename CompType>
        const CompType* getComponentPtr(Entity::Id id) const;

        std::vector<ComponentMask> allComponentMasks() const;

        ComponentMask componentMask(Entity::Id id);

        template <typename CompType>
        ComponentMask componentMask();

        template <typename CompType1, typename CompType2, typename ... CompTypeArgs>
        ComponentMask componentMask();

        template <typename CompType>
        ComponentMask componentMask(const ComponentPtr<CompType>& comp);

        template <typename CompType1, typename ... CompTypeArgs>
        ComponentMask componentMask(const ComponentPtr<CompType1>& compOne, const ComponentPtr<CompTypeArgs>& ... args);

        void accomodateComponent(uint32_t index);

        template <typename CompType>
        Pool<CompType>* accomodateComponent();

    private:
        friend class Entity;

        template <typename CompType, typename EManager>
        friend class ComponentPtr;

        uint32_t indexCounter = 0;

        EventManager& eventManager;

        std::vector<BasePool*> componentPools;
        std::vector<ComponentMask> entityComponentMasks;
        std::vector<uint32_t> entityVersions;
        std::vector<uint32_t> freeIds;
};

#include "EntityManager.inl"