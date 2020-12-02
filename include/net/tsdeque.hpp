#pragma once
#include <deque>
#include <mutex>

/**
 * std::deque wrapper with thread safety mutex
 */
namespace net {

template <typename T>
class ThreadSafeDeque {
public:
    ThreadSafeDeque() = default;

    /**
     * Delete as we don't want to copy the mutex.
     * @param other
     */
    ThreadSafeDeque(ThreadSafeDeque<T> const& other) = delete;

    ~ThreadSafeDeque()
    {
        _queue.clear();
    }

    const T& front()
    {
        std::scoped_lock lock(_mutex);
        return _queue.front();
    }

    const T& back()
    {
        std::scoped_lock lock(_mutex);
        return _queue.back();
    }

    T pop_front()
    {
        std::scoped_lock lock(_mutex);
        auto t = std::move(_queue.front());
        _queue.pop_front();
        return t;
    }

    T pop_back()
    {
        std::scoped_lock lock(_mutex);
        auto t = std::move(_queue.back());
        _queue.pop_back();
        return t;
    }

    void push_back(const T& item)
    {
        std::scoped_lock lock(_mutex);
        _queue.emplace_back(std::move(item));
    }

    void push_front(const T& item)
    {
        std::scoped_lock lock(_mutex);
        _queue.emplace_front(std::move(item));
    }

    bool empty()
    {
        std::scoped_lock lock(_mutex);
        return _queue.empty();
    }

    size_t count()
    {
        std::scoped_lock lock(_mutex);
        return _queue.size();
    }

    void clear()
    {
        std::scoped_lock lock(_mutex);
        _queue.clear();
    }

private:
    std::mutex _mutex;
    std::deque<T> _queue;
};
} // namespace net
