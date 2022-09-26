#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <functional>
#include <unordered_map>
#include <utility>
#include <SFML/System/NonCopyable.hpp>

#include "SimpleSignal.hpp"

using EventSignal = Simple::Signal<void (const void*)>;

class EventManager : private sf::NonCopyable
{
    public:
        EventManager() = default;
        ~EventManager() = default;

        template <typename EventType, typename Receiver>
        void subscribe(Receiver& receiver);

        template <typename EventType, typename Receiver>
        void unsubscribe(Receiver& receiver);

        template <typename EventType>
        void emit(const EventType& event);

        template <typename EventType>
        void emit(std::unique_ptr<EventType> event);

        template <typename EventType, typename ... EventArgs>
        void emit(EventArgs&& ... args);

        std::size_t connectReceivers() const;

    private:
        std::shared_ptr<EventSignal>& signalFor(std::size_t handlerId);

        template <typename EventType>
        struct EventCallbackWrapper
        {
            explicit EventCallbackWrapper(std::function<void (const EventType&)>& callback) : callback(callback) {}
            void operator()(const void* event) { callback( *(static_cast<const EventType*>(event))); }

            std::function<void (const EventType&)> callback;
        };

    private:
        std::vector<std::shared_ptr<EventSignal>> eventHandlers;
};

/// Base classes for the event system
class BaseEvent
{
    public:
        using Family = std::size_t;

        virtual ~BaseEvent() {}

    protected:
        static Family familyCounter;
};

template <typename DerivedEvent>
class Event : public BaseEvent
{
    public:
        static Family family()
        {
            static Family family = familyCounter++;

            return family;
        }
};

class BaseReceiver
{
    public:
        virtual ~BaseReceiver()
        {
            for (const auto& connection : connections)
            {
                auto& ptr = connection.second.first;
                if (!ptr.expired())
                {
                    ptr.lock()->disconnect(connection.second.second);
                }
            }
        }

        std::size_t connectedSignals() const
        {
            std::size_t size = 0;
            for (const auto& connection : connections)
            {
                if (!connection.second.first.expired())
                {
                    ++size;
                }
            }

            return size;
        }

    private:
        friend class EventManager;
        
        std::unordered_map<size_t, std::pair<std::weak_ptr<EventSignal>, std::size_t>> connections;
};

template <typename Derived>
class Receiver : public BaseReceiver
{
    public:
        virtual ~Receiver() {}
};

#include "EventManager.inl"