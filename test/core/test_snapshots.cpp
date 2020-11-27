#include "core/snapshot.hpp"
#include <gtest/gtest.h>

class SnapshotTest : public ::testing::Test {
};

class DummyObject : public core::GameObject {
public:
    DummyObject(float x, float y) : _position(x, y)
    {
        add_values();
    }

    ~DummyObject() = default;

    core::vec2f_t position()
    {
        return _position;
    }

private:
    virtual void add_values() override
    {
        _values["position"] = &_position;
    }

    core::vec2f_t _position;
};

class DummyObject2 : public core::GameObject {
public:
    DummyObject2(int health) : _health(health)
    {
        add_values();
    }

    ~DummyObject2() = default;

    core::int_value_t health()
    {
        return _health;
    }

private:
    virtual void add_values() override
    {
        _values["health"] = &_health;
    }

    core::int_value_t _health;

};

TEST_F(SnapshotTest, compare_delta)
{
    core::Snapshot snap_1(0);
    core::Snapshot snap_2(1);

    std::unique_ptr<core::GameObject> obj_1 =
        std::make_unique<DummyObject>(0.5, 0.5);

    std::unique_ptr<core::GameObject> obj_2 =
        std::make_unique<DummyObject>(1.0, 1.0);

    std::unique_ptr<core::GameObject> obj_3 =
        std::make_unique<DummyObject>(1.0, 1.0);

    std::unique_ptr<core::GameObject> obj_4 =
        std::make_unique<DummyObject>(0.75, 0.6);

    snap_1.add_object(0, obj_1);
    snap_1.add_object(1, obj_2);

    snap_2.add_object(0, obj_3);
    snap_2.add_object(1, obj_4);

    // evaluates the differences in the object between the two snapshots.
    core::DeltaSnapshot delta_snap(0, 1);
    delta_snap.evaluate(snap_1, snap_2);

    // the differences are mapped in diffmap
    auto& diffmap = delta_snap.delta_values();

    auto& differences_1 = diffmap.at(0);
    auto& differences_2 = diffmap.at(1);

    ASSERT_EQ(differences_1.at(0).first, "position");
    ASSERT_EQ(differences_2.at(0).first, "position");

    auto differences_1_val = differences_1.at(0).second->cast<core::vec2f_t>();
    auto differences_2_val = differences_2.at(0).second->cast<core::vec2f_t>();

    ASSERT_FLOAT_EQ(differences_1_val->x(), 0.5);
    ASSERT_FLOAT_EQ(differences_1_val->y(), 0.5);

    ASSERT_FLOAT_EQ(differences_2_val->x(), -0.25);
    ASSERT_FLOAT_EQ(differences_2_val->y(), -0.4);
}

TEST_F(SnapshotTest, compare_delta_2)
{
    core::Snapshot snap_1(0);
    core::Snapshot snap_2(1);

    std::unique_ptr<core::GameObject> obj_1 = std::make_unique<DummyObject2>(10);

    // oof
    std::unique_ptr<core::GameObject> obj_2 = std::make_unique<DummyObject2>(8);

    snap_1.add_object(0, obj_1);
    snap_2.add_object(0, obj_2);

    core::DeltaSnapshot delta_snap(0, 1);
    delta_snap.evaluate(snap_1, snap_2);

    auto& diffmap = delta_snap.delta_values();

    auto& differences_1 = diffmap.at(0);

    auto differences_1_val = differences_1.at(0).second->cast<core::int_value_t>();

    ASSERT_EQ(differences_1_val->get_value(), -2);
}

TEST_F(SnapshotTest, deleted_objects)
{
    core::Snapshot snap_1(0);
    core::Snapshot snap_2(1);

    std::unique_ptr<core::GameObject> obj_1 = std::make_unique<DummyObject2>(10);
    // Little cheat as obj_1 pointer is set to null once passed in the snapshot.
    auto* obj_1_address = obj_1.get();

    snap_1.add_object(0, obj_1);

    core::DeltaSnapshot delta_snap(0, 1);
    delta_snap.evaluate(snap_1, snap_2);

    auto& deleted_objects = delta_snap.deleted_objects();

    auto const* deleted_obj = deleted_objects.at(0);

    ASSERT_EQ(deleted_obj, obj_1_address);
}

TEST_F(SnapshotTest, added_objects)
{
    core::Snapshot snap_1(0);
    core::Snapshot snap_2(1);

    std::unique_ptr<core::GameObject> obj_1 = std::make_unique<DummyObject2>(10);
    auto* obj_1_address = obj_1.get();

    snap_2.add_object(0, obj_1);

    core::DeltaSnapshot delta_snap(0, 1);
    delta_snap.evaluate(snap_1, snap_2);

    auto& added_objects = delta_snap.added_objects();

    auto const* added_obj = added_objects.at(0);

    ASSERT_EQ(added_obj, obj_1_address);
}