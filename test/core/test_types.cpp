#include "core/types.hpp"
#include <gtest/gtest.h>

class TypeTest : public ::testing::Test {
};

TEST_F(TypeTest, TestIntegerAndFloatType)
{
    core::int_value_t fst_integer(100);
    core::int_value_t snd_integer(150);
    core::value_t delta_integer = fst_integer.get_delta(&snd_integer);

    int delta_int_value =
        delta_integer->cast_shared<core::int_value_t>()->get_value();

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

TEST_F(TypeTest, TestInterplationFloat)
{
    core::vec2f_t pos(1.0, 5.0);
    core::vec2f_t new_pos(2.0, 3.0);

    core::value_t delta_pos_ptr = pos.get_delta(&new_pos);

    core::value_t middle_pos = pos.interp(delta_pos_ptr.get(), 0.5);

    ASSERT_FLOAT_EQ(middle_pos->cast<core::vec2f_t>()->x(), 1.5);
    ASSERT_FLOAT_EQ(middle_pos->cast<core::vec2f_t>()->y(), 4.0);
}

TEST_F(TypeTest, TestInterpolationInt)
{
    core::vec2i_t grid(5, 5);
    core::vec2i_t new_grid(10, 10);

    core::value_t delta_grid_ptr = grid.get_delta(&new_grid);

    core::value_t middle_grid_1 = grid.interp(delta_grid_ptr.get(), 0.5);

    ASSERT_EQ(middle_grid_1->cast<core::vec2i_t>()->x(), 7);
    ASSERT_EQ(middle_grid_1->cast<core::vec2i_t>()->y(), 7);

    core::value_t middle_grid_2 = grid.interp(delta_grid_ptr.get(), 0.6);

    ASSERT_EQ(middle_grid_2->cast<core::vec2i_t>()->x(), 8);
    ASSERT_EQ(middle_grid_2->cast<core::vec2i_t>()->y(), 8);
}

TEST_F(TypeTest, TestInterpolationNoInterpInt)
{
    core::int_value_nointerp_t point(5);
    core::int_value_nointerp_t new_point(10);

    core::value_t delta_grid_ptr = point.get_delta(&new_point);

    for (float interp_interval = 0.0; interp_interval <= 1.1;
         interp_interval += 0.1) {
        core::value_t middle_grid_1 =
            point.interp(delta_grid_ptr.get(), interp_interval);
        ASSERT_EQ(
            middle_grid_1->cast<core::int_value_nointerp_t>()->get_value(), 5);
        ASSERT_EQ(
            middle_grid_1->cast<core::int_value_nointerp_t>()->get_value(), 5);
    }
}
