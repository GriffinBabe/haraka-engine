#include "core/action.hpp"
#include "core/game_object.hpp"
#include "core/gameinstance.hpp"
#include "core/types.hpp"
#include <google/protobuf/util/json_util.h>
#include <gtest/gtest.h>
#include <memory>

class DummyObject : public core::GameObject {
public:
    DummyObject(std::uint32_t id, int health)
        : core::GameObject(id), _health(health)
    {
    }

    DummyObject(DummyObject const& other)
        : core::GameObject(other._id), _health(other._health)
    {
        add_values();
    }

    ~DummyObject(){};

    void update(float delta_time) override
    {
    }

    std::string type_name() const override
    {
        return "DummyObject";
    }

protected:
    void add_values() override
    {
        _values["health"] = &_health;
    }

public:
    std::unique_ptr<GameObject> clone() override
    {
        return std::make_unique<DummyObject>(*this);
    }

    void react_event(Observable* observer, core::Event& event) override
    {
    }

    core::int_value_t health() const
    {
        return _health;
    }

private:
    core::int_value_t _health;
};

class DummyAction : public core::GameAction {
public:
    DummyAction(std::uint32_t id) : core::GameAction(id)
    {
    }

    DummyAction() : core::GameAction()
    {
    }

    static std::unique_ptr<core::GameAction> create()
    {
        return std::make_unique<DummyAction>();
    }

protected:
    void add_values() override
    {
    }

    std::string type_name() const override
    {
        return "DummyAction";
    }

    bool check_action(const core::Snapshot& snap) override
    {
        auto obj_cast =
            std::dynamic_pointer_cast<const DummyObject>(snap.get_object(_id));
        if (obj_cast == nullptr) {
            return false;
        }
        auto health = obj_cast->health();
        if (health.get_value() > 5) {
            return true;
        }
        return false;
    }
    void perform_act(core::Snapshot& snap) override
    {
        std::cout << "Hit" << std::endl;
    }

private:
    static core::ActionRegistrar registrar;
};

core::ActionRegistrar DummyAction::registrar("DummyAction",
                                             DummyAction::create);

class UnitObject : public core::GameObject {
public:
    UnitObject(std::uint32_t id, core::vec2i_t& pos, core::Team& team)
        : core::GameObject(id), _pos(pos), _team(team)
    {
        add_values();
    }

    UnitObject(UnitObject const& other)
        : core::GameObject(other._id), _pos(other._pos), _team(other._team)
    {
        add_values();
    }

    void update(float delta_time) override
    {
    }

    void react_event(Observable* observer, core::Event& event) override
    {
    }

    std::unique_ptr<GameObject> clone() override
    {
        return std::make_unique<UnitObject>(*this);
    }

    std::string type_name() const override
    {
        return "UnitObject";
    }

protected:
    void add_values() override
    {
        _values["position"] = &_pos;
        _values["team"] = &_team;
    }

public:
    ~UnitObject(){};

    core::Team const& team() const
    {
        return _team;
    }

    void move(core::vec2i_t delta)
    {
        _pos += delta;
    }

private:
    core::vec2i_t _pos;
    core::Team _team;
};

class ActionTest : public ::testing::Test {
};

TEST_F(ActionTest, test_valid_action)
{
    // 10 Hp shoud work
    std::uint32_t object_id = 0;
    auto object = std::make_shared<DummyObject>(object_id, 10);

    core::Snapshot snapshot(0);
    snapshot.add_object(object);

    auto action = std::make_shared<DummyAction>(object_id);

    core::GameInstance instance(snapshot);
    instance.add_action(action);

    // Unexisting object
    object_id = 2;
    auto action_2 = std::make_shared<DummyAction>(object_id);
    instance.add_action(action_2);

    instance.update_tick();

    // get action status
    auto report_list = instance.action_status_list();

    auto status_1 = report_list.at(0);
    auto status_2 = report_list.at(1);

    ASSERT_TRUE(status_1.success);
    ASSERT_FALSE(status_2.success);
    std::cout << status_2.message << std::endl;
}

TEST_F(ActionTest, test_invalid_action)
{
    std::uint32_t object_id = 0;
    auto object = std::make_shared<DummyObject>(object_id, 2);

    core::Snapshot snapshot(0);
    snapshot.add_object(object);

    core::GameInstance instance(snapshot);

    auto action = std::make_shared<DummyAction>(object_id);
    instance.add_action(action);

    instance.update_tick();

    auto report_list = instance.action_status_list();

    ASSERT_FALSE(report_list.at(0).success);
}

TEST_F(ActionTest, test_inexisting_object_action)
{
    core::Snapshot snapshot(0);
    core::GameInstance instance(snapshot);

    std::uint32_t object_id = 0;

    auto action = std::make_shared<DummyAction>(object_id);
    instance.add_action(action);

    instance.update_tick();

    auto report_list = instance.action_status_list();

    ASSERT_FALSE(report_list.at(0).success);
}

TEST_F(ActionTest, test_serialize_action)
{
    auto action = std::make_shared<DummyAction>(0);

    auto serialized_action = action->serialize();

    std::string json;
    google::protobuf::util::MessageToJsonString(serialized_action, &json);
    std::cout << json << std::endl;

    auto deserialized_action = core::GameAction::deserialize(serialized_action);

    ASSERT_EQ(action->id(), deserialized_action->id());
}

TEST_F(ActionTest, test_serialize_action_status)
{
    core::Snapshot snapshot(0);

    auto object = std::make_shared<DummyObject>(0, 10);
    snapshot.add_object(object);

    core::GameInstance instance(snapshot);

    auto action_1 = std::make_shared<DummyAction>(0);
    auto action_2 = std::make_shared<DummyAction>(1); // invalid action

    instance.add_action(action_1);
    instance.add_action(action_2);

    instance.update_tick();

    auto action_status_list = instance.action_status_list();

    ASSERT_EQ(action_status_list.at(0).action_id, 0);
    ASSERT_TRUE(action_status_list.at(0).success);
    ASSERT_EQ(action_status_list.at(0).message, "");

    ASSERT_EQ(action_status_list.at(1).action_id, 1);
    ASSERT_FALSE(action_status_list.at(1).success);

    auto serialized_status_list =
        core::serialize_action_status_list(action_status_list);

    std::string json;
    google::protobuf::util::MessageToJsonString(serialized_status_list, &json);
    std::cout << json << std::endl;

    auto deserialized_status_list =
        core::deserialize_action_status_list(serialized_status_list);

    ASSERT_EQ(deserialized_status_list.at(0).action_id, 0);
    ASSERT_TRUE(deserialized_status_list.at(0).success);
    ASSERT_EQ(deserialized_status_list.at(0).message, "");

    ASSERT_EQ(deserialized_status_list.at(1).action_id, 1);
    ASSERT_FALSE(deserialized_status_list.at(1).success);
}
