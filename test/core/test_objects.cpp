#include "core/game_object.hpp"
#include <gtest/gtest.h>

class ObjectTest : public ::testing::Test {
};

class DummyObject : public core::GameObject {
public:
    DummyObject(std::uint32_t id, int value)
        : core::GameObject(id), _value(value)
    {
        add_values();
    }

    DummyObject(DummyObject const& other)
        : core::GameObject(other._id), _value(other._value)
    {
        add_values();
    }

    void react_event(Observable* observer, core::Event& event) override
    {
    }

    void update(float delta_time) override
    {
    }

    std::unique_ptr<GameObject> clone() override
    {
        return std::make_unique<DummyObject>(*this);
    }

protected:
    void add_values() override
    {
        _values["value"] = &_value;
    }

private:
    core::int_value_t _value;
};

class ComplexDummyObject : public core::GameObject {
public:
    ComplexDummyObject(std::uint32_t id, float x, float y, int a)
        : core::GameObject(id), _position(x, y), _health(a)
    {
        add_values();
    }

    ComplexDummyObject(ComplexDummyObject const& other)
        : core::GameObject(other._id),
          _position(other._position),
          _health(other._health)
    {
        add_values();
    }

    void react_event(Observable* observer, core::Event& event) override
    {
    }

    void update(float delta_time) override
    {
    }

    std::unique_ptr<GameObject> clone() override
    {
        return std::make_unique<ComplexDummyObject>(*this);
    }

protected:
    void add_values() override
    {
        _values["position"] = &_position;
        _values["health"] = &_health;
    }

private:
    core::vec2f_t _position;
    core::int_value_t _health;
};

TEST_F(ObjectTest, test_checksum)
{
    // same objects but different value
    auto object = std::make_shared<DummyObject>(0, 0);
    auto object_2 = std::make_shared<DummyObject>(0, 1);

    auto object_3 = std::make_shared<DummyObject>(1, 0);

    ASSERT_NE(object->checksum(), object_2->checksum());
    ASSERT_EQ(object->checksum(), object_3->checksum());
    std::cout << object->checksum() << std::endl;
    std::cout << object_2->checksum() << std::endl;
}

TEST_F(ObjectTest, test_checksum_complex)
{
    auto object = std::make_shared<ComplexDummyObject>(0, 0.0f, 0.0f, 10);
    auto object_2 = std::make_shared<ComplexDummyObject>(0, 0.0f, 0.0f, 5);
    auto object_3 = std::make_shared<ComplexDummyObject>(0, 0.0f, 0.0f, 10);
    auto object_4 = std::make_shared<ComplexDummyObject>(0, 0.5f, 0.0f, 5);

    ASSERT_NE(object->checksum(), object_2->checksum());
    ASSERT_NE(object->checksum(), object_4->checksum());
    ASSERT_EQ(object->checksum(), object_3->checksum());

    ASSERT_NE(object_2->checksum(), object_3->checksum());
    ASSERT_NE(object_2->checksum(), object_4->checksum());

    ASSERT_NE(object_3->checksum(), object_4->checksum());

    std::cout << object->checksum() << std::endl;
    std::cout << object_2->checksum() << std::endl;
    std::cout << object_3->checksum() << std::endl;
    std::cout << object_4->checksum() << std::endl;
}