#include <core/gameinstance.hpp>
#include <gtest/gtest.h>

class GameInstanceTest : public ::testing::Test {
};

class DummyObject : public core::GameObject {
public:
    DummyObject(std::uint32_t id, core::vec2f_t pos, core::vec2f_t speed)
        : core::GameObject(id), _pos(std::move(pos)), _speed(std::move(speed))
    {
        add_values();
    }

    void react_event(Observable* observer, core::Event& event) override
    {
    }

    void update(float delta_time) override
    {
        auto delta_pos = _speed * delta_time;
        _pos += delta_pos;
    }

    std::unique_ptr<GameObject> clone() override
    {
        return std::make_unique<DummyObject>(_id, _pos, _speed);
    }

protected:
    void add_values() override
    {
        _values["position"] = &_pos;
        _values["speed"] = &_speed;
    }

private:
    core::vec2f_t _pos, _speed;
};

TEST_F(GameInstanceTest, test_delta_update)
{
    std::uint32_t id = 0;
    auto object = std::make_shared<DummyObject>(
        id, core::vec2f_t(.0f, .0f), core::vec2f_t(.5f, .0f));
    core::Snapshot snapshot(0);
    snapshot.add_object(object);

    core::GameInstance instance(snapshot);
    auto delta = instance.update_tick();  // tickrate is 15, so 66,6666.. ms per tick.

    auto delta_position =
        delta->delta_values().at(0).at("position")->cast<core::vec2f_t>();
    ASSERT_FLOAT_EQ(delta_position->x(), .5f * (1.f/15.f));
    ASSERT_FLOAT_EQ(delta_position->y(), .0f);
}
