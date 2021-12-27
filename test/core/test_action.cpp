#include "core/action.hpp"
#include "core/game_object.hpp"
#include "core/gameinstance.hpp"
#include "core/types.hpp"
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

protected:
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
};

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

class MoveUnitDownAction : public core::GameAction {
public:
    MoveUnitDownAction(std::uint32_t id, core::Player& player)
        : core::GameAction(id), _player(player)
    {
    }

protected:
    bool check_action(const core::Snapshot& snap) override
    {
        // check if unit belongs to the same team than the player
        auto unit_obj =
            std::dynamic_pointer_cast<const UnitObject>(snap.get_object(_id));
        if (unit_obj == nullptr)
            return false;
        if (!unit_obj->team().is_same(_player.team()))
            return false;

        return true;
    }

    void perform_act(core::Snapshot& snap) override
    {
        auto unit_obj =
            std::dynamic_pointer_cast<UnitObject>(snap.get_object(_id));
        unit_obj->move(core::vec2i_t(0, 1));
    }

private:
    core::Player& _player;
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

TEST_F(ActionTest, test_same_team_action)
{
    core::Snapshot snapshot(0);

    std::uint32_t object_id = 0;
    std::uint32_t player_id = 0;

    core::Team team(core::Team::BLUE);

    core::Player player(player_id, team);

    core::vec2i_t pos(0, 0);

    auto object = std::make_shared<UnitObject>(object_id, pos, team);

    snapshot.add_object(object);

    core::GameInstance instance(snapshot);

    auto action = std::make_shared<MoveUnitDownAction>(object_id, player);

    instance.add_action(action);
    instance.update_tick();

    auto report_list = instance.action_status_list();

    ASSERT_TRUE(report_list.at(0).success);
}

TEST_F(ActionTest, test_wrong_team_action)
{
    core::Snapshot snapshot(0);

    std::uint32_t object_id = 0;
    std::uint32_t player_id = 0;

    core::Team team(core::Team::BLUE);
    core::Team enemy(core::Team::RED);

    core::Player player(player_id, team);
    core::vec2i_t pos(0, 0);

    auto object = std::make_shared<UnitObject>(object_id, pos, enemy);

    snapshot.add_object(object);

    core::GameInstance instance(snapshot);

    auto action = std::make_shared<MoveUnitDownAction>(object_id, player);

    instance.add_action(action);
    instance.update_tick();

    auto report_list = instance.action_status_list();

    ASSERT_FALSE(report_list.at(0).success);
}