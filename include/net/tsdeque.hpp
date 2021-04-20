#pragma once
#include <condition_variable>
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
        if (_queue.empty()) {
            throw std::runtime_error(
                "Attempted to access front element on empty queue.");
        }

        return _queue.front();
    }

    const T& back()
    {
        std::scoped_lock lock(_mutex);
        if (_queue.empty()) {
            throw std::runtime_error(
                "Attempted to access front element on empty queue.");
        }
        return _queue.back();
    }

    T pop_front()
    {
        std::scoped_lock lock(_mutex);
        if (_queue.empty()) {
            throw std::runtime_error(
                "Attempted to access front element on empty queue.");
        }
        auto t = std::move(_queue.front());
        _queue.pop_front();
        return t;
    }

    T pop_back()
    {
        std::scoped_lock lock(_mutex);
        if (_queue.empty()) {
            throw std::runtime_error(
                "Attempted to access front element on empty queue.");
        }
        auto t = std::move(_queue.back());
        _queue.pop_back();
        return t;
    }

    void push_back(const T& item)
    {
        std::scoped_lock lock(_mutex);
        _queue.emplace_back(std::move(item));

        std::unique_lock<std::mutex> lock_blocking(_mutex_blocking);
        _blocking.notify_one();
    }

    void push_front(const T& item)
    {
        std::scoped_lock lock(_mutex);
        _queue.emplace_front(std::move(item));

        std::unique_lock<std::mutex> lock_blocking(_mutex_blocking);
        _blocking.notify_one();
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

    void wait()
    {
        while (empty()) {
            std::unique_lock<std::mutex> lock(_mutex_blocking);
            _blocking.wait(lock);
        }
    }

private:
    std::mutex _mutex;
    std::deque<T> _queue;
    std::condition_variable _blocking;
    std::mutex _mutex_blocking;
};
} // namespace net
