#include "core/types.hpp"
#include <gtest/gtest.h>

class TypeTest : public ::testing::Test {
};

TEST_F(TypeTest, TestIntegerAndFloatType)
{
    core::int_value_t fst_integer(100);
    core::int_value_t snd_integer(150);
    core::value_t delta_integer = fst_integer.get_delta(&snd_integer);

    int delta_int_value = delta_integer->cast_shared<core::int_value_t>()->get_value();

    ASSERT_EQ(delta_int_value, 50);

    core::float_value_t fst_float(100.0);
    core::float_value_t snd_float(150.0);
    core::value_t delta_float = fst_float.get_delta(&snd_float);

    float delta_float_value =
        delta_float->cast_shared<core::float_value_t>()->get_value();

    ASSERT_FLOAT_EQ(delta_float_value, 50.0);
}

TEST_F(TypeTest, TestVectorType)
{
    core::vec2f_t pos(1.0, 5.0);
    core::vec2f_t new_pos(2.0, 3.0);

    core::value_t delta_pos_pt = pos.get_delta(&new_pos);

    float d_x = delta_pos_pt->cast<core::vec2f_t>()->x();
    float d_y = delta_pos_pt->cast<core::vec2f_t>()->y();

    ASSERT_FLOAT_EQ(d_x, 1.0);
    ASSERT_FLOAT_EQ(d_y, -2.0);
}
