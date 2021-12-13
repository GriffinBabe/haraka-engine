#include "core/events.hpp"

void core::Observable::notify(Event event)
{
    for (const auto obs : _observers) {
        obs->react_event(this, event);
    }
}

void core::Observable::add_observer(core::Observer* observer)
{
    _observers.push_back(observer);
}

void core::Observable::remove_observer(core::Observer* observer)
{
    _observers.erase(std::remove_if(
        _observers.begin(), _observers.end(), [observer](const Observer* o) {
            return (o == observer);
        }));
}
