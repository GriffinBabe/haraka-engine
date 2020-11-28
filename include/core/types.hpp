#pragma once
#include <memory>

namespace core {

/**
 * Base abstract class for all the Game object values.
 */
class GameValue : public std::enable_shared_from_this<GameValue> {
public:
    /**
     * Computes the difference between the two GameValues and stores it in a
     * GameValue
     * @param new_value, the more recent GameValue.
     */
    virtual std::shared_ptr<GameValue>
    get_delta(GameValue* new_value) = 0;

    /**
     * Interpolates the delta
     * @param delta, the delta delta to the next snapshot.
     * @param interval, between 0.0 and 1.0 where to linearly interpolate in
     * time.
     * @return
     */
    virtual std::shared_ptr<GameValue> interp(GameValue* delta,
                                              float interval) = 0;

    /**
     * Dynamically casts the game value in the specified derived GameValue
     * class.
     * @tparam T
     * @return the dynamically casted GameValue.
     */
    template <typename T,
              typename = typename std::enable_if<
                  std::is_base_of<GameValue, T>::value>::type>
    T* cast()
    {
        auto ptr = dynamic_cast<T*>(this);
#ifndef NDEBUG
        assert(ptr != nullptr);
#endif
        return ptr;
    }

    /**
     * Same as cast<T>(), but by returning an std::shared_ptr()
     * class.
     * @tparam T
     * @return the dynamically casted GameValue.
     */
    template <typename T,
              typename = typename std::enable_if<
                  std::is_base_of<GameValue, T>::value>::type>
    std::shared_ptr<T> cast_shared()
    {
        auto ptr = std::dynamic_pointer_cast<T>(shared_from_this());
#ifndef NDEBUG
        assert(ptr != nullptr);
#endif
        return ptr;
    }

    /**
     * Same as cast<T>() but returns a const pointer to it.
     * @tparam T
     * @return
     */
    template <typename T,
        typename = typename std::enable_if<
            std::is_base_of<GameValue, T>::value>::type>
    T const* cst_cast() const
    {
        auto ptr = dynamic_cast<T const*>(this);
#ifndef NDEBUG
        assert(ptr != nullptr);
#endif
        return ptr;
    }

protected:
private:
};

/**
 * Derived from GameValue. Represents a primitive type (for example a int,
 * float)
 * @tparam T the primitive type.
 */
template <typename T,
          typename = typename std::enable_if<std::is_fundamental<T>::value>>
class PrimitiveValue : public GameValue {
public:
    PrimitiveValue() = default;

    PrimitiveValue(T value) : _value(value)
    {
    }

    virtual std::shared_ptr<GameValue>
    get_delta(GameValue* delta) override
    {
        auto new_value = delta->cast<PrimitiveValue<T>>();
        auto delta_value =
            std::make_shared<PrimitiveValue<T>>(new_value->_value - _value);
        return delta_value->template cast_shared<GameValue>();
    }

    virtual std::shared_ptr<GameValue> interp(GameValue* delta,
                                              float interval) override
    {
        auto delta_value = delta->cast<PrimitiveValue<T>>();
        auto new_value = std::make_shared<PrimitiveValue<T>>();
        new_value->_value = _value + (delta_value->_value * interval);
        return new_value->template cast_shared<GameValue>();
    }

    T get_value() const
    {
        return _value;
    }

private:
    T _value;
};

/**
 * Pair of two GameValues.
 * @tparam T the template type.
 */
template <typename T,
          typename = typename std::enable_if<std::is_fundamental<T>::value>>
class Vec2 : public GameValue {
public:
    Vec2() = default;

    Vec2(T x, T y) : _x(x), _y(y)
    {
    }

    virtual std::shared_ptr<GameValue>
    get_delta(GameValue* delta) override
    {
        auto new_value = delta->cast<Vec2<T>>();
        auto delta_value =
            std::make_shared<Vec2<T>>(new_value->_x - _x, new_value->_y - _y);
        return delta_value->template cast_shared<GameValue>();
    }

    virtual std::shared_ptr<GameValue> interp(GameValue* delta,
                                              float interval) override
    {
        auto delta_value = delta->cast<Vec2<T>>();
        auto new_value = std::make_shared<Vec2<T>>();
        new_value->_x = _x + (delta_value->_x * interval);
        new_value->_y = _y + (delta_value->_y * interval);
        return new_value->template cast_shared<GameValue>();
    }

    T x() const
    {
        return _x;
    }

    T y() const
    {
        return _y;
    }

private:
    T _x;
    T _y;
};

/**
 * Predefined common Game value types.
 */
typedef std::shared_ptr<GameValue> value_t;

typedef PrimitiveValue<float> float_value_t;
typedef PrimitiveValue<int> int_value_t;

typedef Vec2<float> vec2f_t;
typedef Vec2<int> vec2i_t;

} // namespace core
