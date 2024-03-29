#include "core/exception.hpp"
#include "core/snapshot.hpp"
#include <google/protobuf/util/json_util.h>
#include <gtest/gtest.h>

class SnapshotTest : public ::testing::Test {
};

class DummyObject : public core::GameObject {
public:
    DummyObject(std::uint32_t id, float x, float y)
        : core::GameObject(id), _position(x, y)
    {
        add_values();
    }

    DummyObject(DummyObject const& other)
        : core::GameObject(other._id), _position(other._position)
    {
        add_values();
    }

    DummyObject() : core::GameObject()
    {
    }

    void update(float delta_time) override
    {
    }

    ~DummyObject() = default;

    void react_event(Observable* observer, core::Event& event) override
    {
    }

    virtual std::unique_ptr<GameObject> clone() override
    {
        return std::make_unique<DummyObject>(*this);
    }

    core::vec2f_t position()
    {
        return _position;
    }

    std::string type_name() const override
    {
        return "DummyObject";
    }

    static std::unique_ptr<core::GameObject> create()
    {
        return std::make_unique<DummyObject>();
    }

private:
    virtual void add_values() override
    {
        _values["position"] = &_position;
    }

    core::vec2f_t _position;

    static core::Registrar registrar;
};

class DummyObject2 : public core::GameObject {
public:
    DummyObject2(std::uint32_t id, int health)
        : core::GameObject(id), _health(health)
    {
        add_values();
    }

    DummyObject2(DummyObject2 const& other)
        : core::GameObject(other._id), _health(other._health)
    {
        add_values();
    }

    DummyObject2() : core::GameObject()
    {
    }

    ~DummyObject2() = default;

    void update(float delta_time) override
    {
    }

    void react_event(Observable* observer, core::Event& event) override
    {
    }

    virtual std::unique_ptr<GameObject> clone() override
    {
        return std::make_unique<DummyObject2>(*this);
    }

    core::int_value_t health()
    {
        return _health;
    }

    std::string type_name() const override
    {
        return "DummyObject2";
    }

    static std::unique_ptr<core::GameObject> create()
    {
        return std::make_unique<DummyObject2>();
    }

private:
    virtual void add_values() override
    {
        _values["health"] = &_health;
    }

    core::int_value_t _health;

    static core::Registrar registrar;
};

core::Registrar DummyObject::registrar("DummyObject", DummyObject::create);
core::Registrar DummyObject2::registrar("DummyObject2", DummyObject2::create);

TEST_F(SnapshotTest, compare_delta)
{
    core::Snapshot snap_1(0);
    core::Snapshot snap_2(1);

    auto obj_1 = std::make_shared<DummyObject>(0, 0.5, 0.5);

    auto obj_2 = std::make_shared<DummyObject>(1, 1.0, 1.0);

    auto obj_3 = std::make_shared<DummyObject>(0, 1.0, 1.0);

    auto obj_4 = std::make_shared<DummyObject>(1, 0.75, 0.6);

    snap_1.add_object(obj_1);
    snap_1.add_object(obj_2);

    snap_2.add_object(obj_3);
    snap_2.add_object(obj_4);

    // evaluates the differences in the object between the two snapshots.
    core::DeltaSnapshot delta_snap(0, 1);
    delta_snap.evaluate(snap_1, snap_2);

    // the differences are mapped in diffmap
    auto& diffmap = delta_snap.delta_values();

    auto& differences_1 = diffmap.at(0);
    auto& differences_2 = diffmap.at(1);

    auto differences_1_val =
        differences_1.at("position")->cast<core::vec2f_t>();
    auto differences_2_val =
        differences_2.at("position")->cast<core::vec2f_t>();

    ASSERT_FLOAT_EQ(differences_1_val->x(), 0.5);
    ASSERT_FLOAT_EQ(differences_1_val->y(), 0.5);

    ASSERT_FLOAT_EQ(differences_2_val->x(), -0.25);
    ASSERT_FLOAT_EQ(differences_2_val->y(), -0.4);
}

TEST_F(SnapshotTest, compare_delta_2)
{
    core::Snapshot snap_1(0);
    core::Snapshot snap_2(1);

    auto obj_1 = std::make_shared<DummyObject2>(0, 10);

    // oof
    auto obj_2 = std::make_shared<DummyObject2>(0, 8);

    snap_1.add_object(obj_1);
    snap_2.add_object(obj_2);

    core::DeltaSnapshot delta_snap(0, 1);
    delta_snap.evaluate(snap_1, snap_2);

    auto& diffmap = delta_snap.delta_values();

    auto& differences_1 = diffmap.at(0);

    auto differences_1_val =
        differences_1.at("health")->cast<core::int_value_t>();

    ASSERT_EQ(differences_1_val->get_value(), -2);
}

TEST_F(SnapshotTest, deleted_objects)
{
    core::Snapshot snap_1(0);
    core::Snapshot snap_2(1);

    std::uint32_t obj_1_id = 0;
    auto obj_1 = std::make_shared<DummyObject2>(obj_1_id, 10);

    snap_1.add_object(obj_1);

    core::DeltaSnapshot delta_snap(0, 1);
    delta_snap.evaluate(snap_1, snap_2);

    auto& deleted_objects = delta_snap.deleted_objects();

    auto deleted_obj_id = deleted_objects.at(0);

    ASSERT_EQ(deleted_obj_id, obj_1->id());
}

TEST_F(SnapshotTest, added_objects)
{
    core::Snapshot snap_1(0);
    core::Snapshot snap_2(snap_1);

    std::uint32_t obj_1_id = 0;
    auto obj_1 = std::make_shared<DummyObject2>(obj_1_id, 10);
    auto* obj_1_address = obj_1.get();

    snap_2.add_object(obj_1);

    core::DeltaSnapshot delta_snap(0, 1);
    delta_snap.evaluate(snap_1, snap_2);

    auto& added_objects = delta_snap.added_objects();

    auto const added_obj = added_objects.at(obj_1_id);

    ASSERT_EQ(added_obj.get(), obj_1_address);
}

TEST_F(SnapshotTest, snapshot_deep_copy)
{
    core::Snapshot snap_1(0);

    std::uint32_t obj_1_id = 0;
    std::uint32_t obj_2_id = 1;

    auto obj_1 = std::make_shared<DummyObject2>(obj_1_id, 10);
    auto obj_2 = std::make_shared<DummyObject>(obj_2_id, 0.5f, 0.5f);
    snap_1.add_object(obj_1);
    snap_1.add_object(obj_2);

    core::Snapshot snap_2(snap_1);

    ASSERT_EQ(snap_2.tick(), 1);

    auto const snap_1_obj_1 = snap_1.get_object(obj_1_id);
    auto const snap_1_obj_2 = snap_1.get_object(obj_2_id);

    auto const snap_2_obj_1 = snap_2.get_object(obj_1_id);
    auto const snap_2_obj_2 = snap_2.get_object(obj_2_id);

    ASSERT_NE(snap_1_obj_1.get(), snap_2_obj_1.get());
    ASSERT_NE(snap_1_obj_2.get(), snap_2_obj_2.get());

    ASSERT_EQ(snap_1_obj_1->get_value("health")
                  ->cst_cast<core::int_value_t>()
                  ->get_value(),
              snap_2_obj_1->get_value("health")
                  ->cst_cast<core::int_value_t>()
                  ->get_value());

    ASSERT_EQ(
        snap_1_obj_2->get_value("position")->cst_cast<core::vec2f_t>()->x(),
        snap_2_obj_2->get_value("position")->cst_cast<core::vec2f_t>()->x());

    ASSERT_EQ(
        snap_1_obj_2->get_value("position")->cst_cast<core::vec2f_t>()->y(),
        snap_2_obj_2->get_value("position")->cst_cast<core::vec2f_t>()->y());
}

TEST_F(SnapshotTest, test_apply_snapshot)
{
    core::Snapshot snap_1(0);
    core::Snapshot snap_2(1);

    auto obj_snap_1 = std::make_shared<DummyObject>(0, 0.0f, 0.0f);
    auto obj_2_snap_1 = std::make_shared<DummyObject>(1, 0.0f, 0.0f);
    snap_1.add_object(obj_snap_1);
    snap_1.add_object(obj_2_snap_1);

    auto obj_snap_2 = std::make_shared<DummyObject>(0, 1.0f, 1.0f);
    snap_2.add_object(obj_snap_2);

    core::DeltaSnapshot delta(snap_1.tick(), snap_2.tick());
    delta.evaluate(snap_1, snap_2);

    // interpolates a snapshot
    auto snap_interpolated = snap_1.apply(delta, 0.5f);

    auto obj_1_interpolated = snap_interpolated.get_object(0);
    auto obj_2_interpolated = snap_interpolated.get_object(1);

    ASSERT_NE(obj_snap_1.get(), obj_1_interpolated.get());
    ASSERT_FALSE(obj_2_interpolated
                 == nullptr); // Shouldn't be deleted if interp < 1.0f

    ASSERT_FLOAT_EQ(obj_1_interpolated->get_value("position")
                        ->cst_cast<core::vec2f_t>()
                        ->x(),
                    0.5f);
    ASSERT_FLOAT_EQ(obj_1_interpolated->get_value("position")
                        ->cst_cast<core::vec2f_t>()
                        ->y(),
                    0.5f);
}

TEST_F(SnapshotTest, test_apply_snapshot_deleted)
{
    core::Snapshot snap_1(0);
    core::Snapshot snap_2(1);

    auto obj_snap_1 = std::make_shared<DummyObject>(0, 0.0f, 0.0f);
    snap_1.add_object(obj_snap_1);

    // don't add this object tot the second snap
    core::DeltaSnapshot delta(snap_1.tick(), snap_2.tick());
    delta.evaluate(snap_1, snap_2);

    // interpolated a snapshot: beware of float precision in the interp arg
    auto snap_interpolated = snap_1.apply(delta, 0.99f);

    ASSERT_FALSE(snap_interpolated.get_object(0) == nullptr);

    snap_interpolated = snap_1.apply(delta, 1.0f);

    ASSERT_TRUE(snap_interpolated.get_object(0) == nullptr);
}

TEST_F(SnapshotTest, test_apply_snapshot_added)
{
    core::Snapshot snap_1(0);
    core::Snapshot snap_2(snap_1);

    auto obj_snap_2 = std::make_shared<DummyObject>(0, 0.0f, 0.0f);

    snap_2.add_object(obj_snap_2);

    // don't add this object tot the second snap
    core::DeltaSnapshot delta(snap_1.tick(), snap_2.tick());
    delta.evaluate(snap_1, snap_2);

    // interpolated a snapshot
    auto snap_interpolated = snap_1.apply(delta, 0.99f);

    ASSERT_TRUE(snap_interpolated.get_object(0) == nullptr);

    snap_interpolated = snap_1.apply(delta, 1.0f);

    ASSERT_FALSE(snap_interpolated.get_object(0) == nullptr);
}

TEST_F(SnapshotTest, test_apply_out_of_bounds_interp)
{
    core::Snapshot snap_1(0);

    auto obj = std::make_shared<DummyObject>(0, 0.0f, 0.0f);

    snap_1.add_object(obj);

    core::Snapshot snap_2(snap_1);

    core::DeltaSnapshot delta(snap_1.tick(), snap_2.tick());
    delta.evaluate(snap_1, snap_2);

    try {
        auto snap_interpolated = snap_1.apply(delta, 1.5f); // out of bounds
        FAIL();
    }
    catch (core::HarakaException const& exc) {
        SUCCEED();
    }

    try {
        auto snap_interpolated = snap_1.apply(delta, -0.1f); // out of bounds
        FAIL();
    }
    catch (core::HarakaException const& exc) {
        SUCCEED();
    }
}

TEST_F(SnapshotTest, test_serialization)
{
    core::Snapshot snapshot(0);

    auto obj = std::make_shared<DummyObject>(0, 1.0f, 1.0f);
    snapshot.add_object(obj);

    auto serialized_snapshot = snapshot.serialize();

    std::string json;
    google::protobuf::util::MessageToJsonString(serialized_snapshot, &json);
    std::cout << json << std::endl;

    auto deserialized_snapshot =
        core::Snapshot::deserialize(serialized_snapshot);

    ASSERT_EQ(snapshot.tick(), deserialized_snapshot.tick());
    for (auto const& serialized_object : serialized_snapshot.objects()) {
        auto id = serialized_object.id();
        auto const& object_snapshot = snapshot.get_object(id);
        auto const& object_deserialized_snapshot =
            deserialized_snapshot.get_object(id);
        ASSERT_EQ(object_snapshot->checksum(),
                  object_deserialized_snapshot->checksum());
    }
}

TEST_F(SnapshotTest, test_deltasnapshot_serialization)
{
    core::Snapshot snapshot_1(0);
    core::Snapshot snapshot_2(1);

    auto obj_1 = std::make_shared<DummyObject>(0, 1.0f, 1.0f);
    auto obj_2 = std::make_shared<DummyObject>(1, 1.0f, 1.0f);
    snapshot_1.add_object(obj_1);
    snapshot_1.add_object(obj_2);

    auto obj_3 = std::make_shared<DummyObject>(0, 1.5f, 0.5f);
    auto obj_4 = std::make_shared<DummyObject>(2, 1.5f, 1.5f);
    snapshot_2.add_object(obj_3);
    snapshot_2.add_object(obj_4);

    core::DeltaSnapshot delta_snapshot(0, 1);
    delta_snapshot.evaluate(snapshot_1, snapshot_2);

    auto serialized_delta = delta_snapshot.serialize();

    std::string json;
    google::protobuf::util::MessageToJsonString(serialized_delta, &json);
    std::cout << json << std::endl;

    auto deserialized_delta =
        core::DeltaSnapshot::deserialize(serialized_delta, snapshot_1);

    ASSERT_EQ(deserialized_delta.deleted_objects().at(0), obj_2->id());
    ASSERT_EQ(deserialized_delta.added_objects().begin()->second->checksum(),
              obj_4->checksum());
    for (const auto& pair : delta_snapshot.delta_values()) {
        auto object_id = pair.first;
        for (const auto& value_pair : pair.second) {
            auto deserialized_value =
                deserialized_delta.delta_values().at(object_id).at(
                    value_pair.first);
            ASSERT_EQ(value_pair.second->checksum(),
                      deserialized_value->checksum());
        }
    }
}
