#pragma once
#include <assert.h>
#include <memory>
#include <boost/crc.hpp>
#include <sstream>

namespace core {

/**
 * Base abstract class for all the Game object values.
 */
class GameValue : public std::enable_shared_from_this<GameValue> {
public:
    /**
     * Returns the checksum of the object from it's value with the CRC hashing
     * algorithm.
     * @return
     */
    virtual std::uint32_t checksum() const = 0;

    /**
     * Takes the values of the given GameValues and copies it to this game
     * values. This is a kind of copy assignment but supports polymorphism
     *
     * @param other
     */
    virtual void change(std::shared_ptr<GameValue>& other) = 0;

    /**
     * Computes the difference between the two GameValues and stores it in a
     * GameValue
     * @param new_value, the more recent GameValue.
     */
    virtual std::shared_ptr<GameValue> get_delta(GameValue* new_value) = 0;

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
     * Serializes the game value into a string. This will be put in a protocol
     * buffer.
     * @return, the serialized value in bytes.
     */
    virtual std::string serialize() const = 0;

    /**
     * Deserializes a string into a game value. This class is obviously polymorphic
     * and it is assumed as the right derived class object is used to parse this
     * string.
     * @return a shared pointer to a newly created game value.
     */
    virtual std::shared_ptr<GameValue> deserialize(std::string const& bytes) const = 0;

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
     * Same as cast<EnumType>(), but by returning an std::shared_ptr()
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
     * Same as cast<EnumType>() but returns a const pointer to it.
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

    uint32_t checksum() const override
    {
        boost::crc_32_type result;
        result.process_bytes(&_value, sizeof(_value));
        return result.checksum();
    }

    void change(std::shared_ptr<GameValue>& other) override
    {
        auto other_casted = std::dynamic_pointer_cast<PrimitiveValue>(other);
        _value = other_casted->_value;
    }

    virtual std::shared_ptr<GameValue> get_delta(GameValue* delta) override
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

    std::string serialize() const override
    {
        std::stringstream ss;
        ss.write((char*) &_value, sizeof(_value));
        return ss.str();
    }

    std::shared_ptr<GameValue> deserialize(const std::string& bytes) const override
    {
        std::stringstream ss(bytes);
        auto value = std::make_shared<PrimitiveValue<T>>();
        ss.read((char*) &(value->_value), sizeof(T));
        return value;
    }

    T get_value() const
    {
        return _value;
    }

protected:
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

    Vec2& operator=(Vec2 const& other) = default;

    uint32_t checksum() const override
    {
        boost::crc_32_type result;
        result.process_bytes(&_x, sizeof(_x));
        result.process_bytes(&_y, sizeof(_y));
        return result.checksum();
    }

    void change(std::shared_ptr<GameValue>& other) override
    {
        auto other_casted = std::dynamic_pointer_cast<Vec2>(other);
        _x = other_casted->_x;
        _y = other_casted->_y;
    }

    virtual std::shared_ptr<GameValue> get_delta(GameValue* delta) override
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

    Vec2 operator-(Vec2 const& other) const
    {
        return Vec2(_x - other._x, _y - other._y);
    }

    Vec2 operator+(Vec2 const& other) const
    {
        return Vec2(_x + other._x, _y + other._y);
    }

    Vec2 operator*(T scalar) const
    {
        return Vec2(_x * scalar, _y * scalar);
    }

    Vec2& operator+=(Vec2 const& other)
    {
        _x += other._x;
        _y += other._y;
        return *this;
    }

    Vec2& operator-=(Vec2 const& other)
    {
        _x -= other._x;
        _y -= other._y;
        return *this;
    }

    T x() const
    {
        return _x;
    }

    T y() const
    {
        return _y;
    }

    std::string serialize() const override
    {
        std::stringstream ss;
        ss.write((char*) &_x, sizeof(T));
        ss.write((char*) &_y, sizeof(T));
        return ss.str();
    }

    std::shared_ptr<GameValue> deserialize(const std::string& bytes) const override
    {
        auto value = std::make_shared<Vec2<T>>();

        std::stringstream ss(bytes);
        ss.read((char*) &(value->_x), sizeof(T));
        ss.read((char*) &(value->_y), sizeof(T));

        return value;
    }

protected:
    T _x;
    T _y;
};

/**
 * This primitive value disables interpolation. Meaning that the value directly
 * "jumps" from one tick to the other.
 */
template <typename T,
          typename = typename std::enable_if<std::is_fundamental<T>::value>>
class PrimitiveValueNoInterp : public PrimitiveValue<T> {
public:
    PrimitiveValueNoInterp(T value) : PrimitiveValue<T>(value)
    {
    }

    virtual std::shared_ptr<GameValue> interp(GameValue* delta,
                                              float interval) override
    {
        // specifying the THIS keyword tells the compiler that the variable
        // _value is dependant of the initialization of the object.
        // This is required because the compiler does not initialize the
        // parent template before reaching this part of the code
        return std::make_shared<PrimitiveValueNoInterp>(this->_value);
    }
};

/**
 * Predefined common Game value types.
 */
typedef std::shared_ptr<GameValue> value_t;

typedef PrimitiveValue<float> float_value_t;
typedef PrimitiveValue<int> int_value_t;

typedef PrimitiveValueNoInterp<int> int_value_nointerp_t;

typedef Vec2<float> vec2f_t;
typedef Vec2<int> vec2i_t;

} // namespace core
