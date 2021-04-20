#pragma once

#include <vector>
#include <algorithm>

namespace core {
class Event {
public:

    enum EventType {
        OBJECT_DELETED
    };

    Event(EventType type) : type(type) {}

    EventType type;
};

// Forward decl.
class Observer;

class Observable {

public:
    void notify(Event event);

    void add_observer(Observer* observer);

    void remove_observer(Observer* observer);

private:

    std::vector<Observer*> _observers;
};

class Observer {
public:
    virtual void update(Observable* observer, Event& event) = 0;
};

} // namespace core
