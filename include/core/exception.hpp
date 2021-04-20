#pragma once
#include <exception>
#include <string>
#include <memory>


namespace core {

class GameAction;

/**
 * Base class for all engine exceptions.
 */
class HarakaException : public std::exception {
public:
    HarakaException(std::string const& message) : _message(message)
    {
    }

    virtual char const* what() const noexcept
    {
        return _message.c_str();
    }

protected:
    std::string _message;
};

/**
 * Unknown ID exception.
 */
class UnknownIDException : public core::HarakaException {
public:
    UnknownIDException(std::string const& message, std::uint32_t id)
        : core::HarakaException(message), _id(id)
    {
    }

    [[nodiscard]] std::uint32_t get_id()
    {
        return _id;
    }

private:
    std::uint32_t _id;
};

class ImpossibleActionException : public core::HarakaException {
public:
    ImpossibleActionException(std::string const& message,
                              GameAction& action)
        : core::HarakaException(message), _action(action)
    {
    }

    [[nodiscard]] GameAction& get_action()
    {
        return _action;
    }

private:
    GameAction& _action;
};

} // namespace core
