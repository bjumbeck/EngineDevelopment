#include "EventManager.hpp"

BaseEvent::Family BaseEvent::familyCounter = 0;

std::size_t EventManager::connectReceivers() const
{
    std::size_t size = 0;
    for (const std::shared_ptr<EventSignal>& handler : eventHandlers)
    {
        if (handler)
        {
            size += handler->size();
        }
    }

    return size;
}

std::shared_ptr<EventSignal>& EventManager::signalFor(std::size_t handlerId)
{
    if (handlerId >= eventHandlers.size())
    {
        eventHandlers.resize(handlerId + 1);
    }

    if (!eventHandlers[handlerId])
    {
        eventHandlers[handlerId] = std::make_shared<EventSignal>();
    }

    return eventHandlers[handlerId];
}