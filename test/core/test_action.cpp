#include "core/action.hpp"
#include "core/game_object_old.hpp"
#include "core/gameinstance.hpp"
#include "core/types_old.hpp"
#include "core/world.hpp"
#include <gtest/gtest.h>
#include <memory>

class DummyObject : public core::GameObject {
public:
    DummyObject(std::uint32_t id, int health)
        : core::GameObject(id), _health(health)
    {
    }

    ~DummyObject() = default;

    void update(Observable* observer, core::Event& event) override
    {
    }

    int health()
    {
        return _health;
    }

    void on_turn_begin() override
    {
    }

    void on_turn_end() override
    {
    }

private:
    int _health;
};

class DummyAction : public core::GameAction {
public:
    DummyAction(std::uint32_t id) : core::GameAction(id)
    {
    }

protected:
    bool check_action(core::World& world) override
    {
        auto obj_cast =
            std::dynamic_pointer_cast<DummyObject>(world.get_gameobject(_id));
        if (obj_cast == nullptr) {
            return false;
        }
        auto health = obj_cast->health();
        if (health > 5) {
            return true;
        }
        return false;
    }
    void perform_act(core::World& world) override
    {
        std::cout << "Hit" << std::endl;
    }
};

class UnitObject : public core::GameObject {
public:
    UnitObject(std::uint32_t id, core::vec2i& pos, core::Team& team)
        : core::GameObject(id), _pos(pos), _team(team)
    {
    }

    void update(Observable* observer, core::Event& event) override
    {
    }

    ~UnitObject() = default;

    core::Team& team()
    {
        return _team;
    }

    void move(core::vec2i delta)
    {
        _pos += delta;
    }

    void on_turn_begin() override
    {
    }

    void on_turn_end() override
    {
    }

private:
    core::vec2i _pos;
    core::Team _team;
};

class MoveUnitDownAction : public core::GameAction {
public:
    MoveUnitDownAction(std::uint32_t id, core::Player& player)
        : core::GameAction(id), _player(player)
    {
    }

protected:
    bool check_action(core::World& world) override
    {
        // check if unit belongs to the same team than the player
        auto unit_obj =
            std::dynamic_pointer_cast<UnitObject>(world.get_gameobject(_id));
        if (unit_obj == nullptr) return false;
        if (!unit_obj->team().is_same(_player.team())) return false;

        return true;
    }

    void perform_act(core::World& world) override
    {
        auto unit_obj = std::dynamic_pointer_cast<UnitObject>(world.get_gameobject(_id));
        unit_obj->move(core::vec2i(0, 1));
    }

private:
    core::Player& _player;
};

class ActionTest : public ::testing::Test {
};

TEST_F(ActionTest, test_valid_action)
{
    core::World world;
    core::GameInstance instance(world);

    // 10 Hp shoud work
    std::uint32_t object_id = 0;
    auto object = std::make_shared<DummyObject>(object_id, 10);

    world.add_gameobject(std::dynamic_pointer_cast<core::GameObject>(object));

    auto action = std::make_shared<DummyAction>(object_id);

    ASSERT_TRUE(instance.play_action(action));

    // Unexisting object
    object_id = 2;
    action = std::make_shared<DummyAction>(object_id);

    ASSERT_FALSE(instance.play_action(action));
}

TEST_F(ActionTest, test_unvalid_action)
{
    core::World world;
    core::GameInstance instance(world);

    std::uint32_t object_id = 0;
    auto object = std::make_shared<DummyObject>(object_id, 2);

    world.add_gameobject(std::dynamic_pointer_cast<core::GameObject>(object));

    auto action = std::make_shared<DummyAction>(object_id);

    ASSERT_FALSE(instance.play_action(action));
}

TEST_F(ActionTest, test_unexisting_object_action)
{
    core::World world;
    core::GameInstance instance(world);

    std::uint32_t object_id = 0;

    auto action = std::make_shared<DummyAction>(object_id);

    ASSERT_FALSE(instance.play_action(action));
}

TEST_F(ActionTest, test_same_team_action)
{
    core::World world;
    core::GameInstance instance(world);

    std::uint32_t object_id = 0;
    std::uint32_t player_id = 0;

    core::Team team = {core::Team::BLUE};

    core::Player player(player_id, team);

    core::vec2i pos(0, 0);

    auto object = std::make_shared<UnitObject>(object_id, pos, team);

    world.add_gameobject(object);

    auto action = std::make_shared<MoveUnitDownAction>(object_id, player);

    ASSERT_TRUE(instance.play_action(action));
}

TEST_F(ActionTest, test_wrong_team_action)
{
    core::World world;
    core::GameInstance instance(world);

    std::uint32_t object_id = 0;
    std::uint32_t player_id = 0;

    core::Team team = {core::Team::BLUE};
    core::Team enemy = { core::Team::RED };

    core::Player player(player_id, team);
    core::vec2i pos(0, 0);

    auto object = std::make_shared<UnitObject>(object_id, pos, enemy);

    world.add_gameobject(object);

    auto action = std::make_shared<MoveUnitDownAction>(object_id, player);

    ASSERT_FALSE(instance.play_action(action));
}