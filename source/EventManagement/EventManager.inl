#pragma once

template <typename EventType, typename Receiver>
void EventManager::subscribe(Receiver& receiver)
{
    void (Receiver::*receive)(const EventType&) = &Receiver::receive;

    auto signal = signalFor(Event<EventType>::family());
    auto wrapper = EventCallbackWrapper<EventType>(std::bind(receive, &receiver, std::placeholders::_1));
    auto connection = signal->connect(wrapper);

    BaseReceiver& baseReceiver = receiver;
    baseReceiver.connections.insert(std::make_pair(Event<EventType>::family(), std::make_pair(std::weak_ptr<EventSignal>(signal), connection)));
}

template <typename EventType, typename Receiver>
void EventManager::unsubscribe(Receiver& receiver)
{
    BaseReceiver& baseRec = receiver;

    assert(baseRec.connections.find(Event<EventType>::family()) != baseRec.connections.end());

    auto pair = baseRec.connections[Event<EventType>::family()];
    auto connection = pair.second;
    auto& ptr = pair.first;

    if (!ptr.expired())
    {
        ptr.lock()->disconnect(connection);
    }

    baseRec.connections.erase(Event<EventType>::family());
}

template <typename EventType>
void EventManager::emit(const EventType& event)
{
    auto signal = signalFor(Event<EventType>::family());
    signal->emit(&event);
}

template <typename EventType>
void EventManager::emit(std::unique_ptr<EventType> event)
{
    auto signal = signalFor(Event<EventType>::family());
    signal->emit(event.get());
}

template <typename EventType, typename ... EventArgs>
void EventManager::emit(EventArgs&& ... args)
{
    EventType event = EventType(std::forward<EventArgs>(args)...);
    auto signal = signalFor(Event<EventType>::family());
    signal->emit(&event);
}