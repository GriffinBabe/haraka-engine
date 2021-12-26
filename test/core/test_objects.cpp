#include "core/exception.hpp"
#include "core/game_object.hpp"
#include <google/protobuf/util/json_util.h>
#include <gtest/gtest.h>

class ObjectTest : public ::testing::Test {
};

class DummyObject : public core::GameObject {
public:
    // for gameobject factory, useful for deserialization
    static std::unique_ptr<core::GameObject> create()
    {
        return std::make_unique<DummyObject>();
    }

    DummyObject() : core::GameObject()
    {
    }

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

    std::string type_name() override
    {
        return "DummyObject";
    }

    int value()
    {
        return _value.get_value();
    }

protected:
    void add_values() override
    {
        _values["value"] = &_value;
    }

private:
    core::int_value_t _value;

    // for gameobject factory, useful for deserialization
    static core::Registrar registrar;
};

class ComplexDummyObject : public core::GameObject {
public:
    // for gameobject factory, useful for deserialization
    static std::unique_ptr<core::GameObject> create()
    {
        return std::make_unique<ComplexDummyObject>();
    }

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

    ComplexDummyObject() : core::GameObject()
    {
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

    std::string type_name() override
    {
        return "ComplexDummyObject";
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

    // for gameobject factory, useful for deserialization
    static core::Registrar registrar;
};

core::Registrar DummyObject::registrar("DummyObject", DummyObject::create);
core::Registrar ComplexDummyObject::registrar("ComplexDummyObject",
                                              ComplexDummyObject::create);

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

TEST_F(ObjectTest, test_object_factory)
{
    auto test = core::GameObject::instantiate("DummyObject");
    ASSERT_NE(test, nullptr);

    std::shared_ptr<core::GameObject> dummy_object =
        core::GameObject::instantiate("DummyObject");
    auto dummy_object_casted =
        std::dynamic_pointer_cast<DummyObject>(dummy_object);
    ASSERT_NE(dummy_object_casted, nullptr);

    std::shared_ptr<core::GameObject> complex_object =
        core::GameObject::instantiate("ComplexDummyObject");
    auto complex_object_casted =
        std::dynamic_pointer_cast<ComplexDummyObject>(complex_object);

    ASSERT_NE(complex_object_casted, nullptr);

    try {
        std::shared_ptr<core::GameObject> invalid_object =
            core::GameObject::instantiate("NotExistingObject");
        FAIL();
    }
    catch (core::UnknownClassTypeException const& exc) {
        std::cout << "Exception occurred (success)" << std::endl;
    }
}

TEST_F(ObjectTest, test_object_serialization)
{
    auto dummy_object = std::make_unique<DummyObject>(0, 50);
    auto serialized_object = dummy_object->serialize();

    std::string json;
    google::protobuf::util::MessageToJsonString(serialized_object, &json);
    std::cout << json << std::endl;

    std::shared_ptr<core::GameObject> dummy_object_deserialized =
        core::GameObject::deserialize(serialized_object);

    ASSERT_EQ(dummy_object->id(), dummy_object_deserialized->id());
    ASSERT_EQ(dummy_object->value(),
              std::dynamic_pointer_cast<DummyObject>(dummy_object_deserialized)
                  ->value());
}
