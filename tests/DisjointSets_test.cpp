#include "../GUI/Include/Extra/DisjointSets.hpp"
#include "../GUI/Include/Extra/Graph.hpp"
#include <gtest/gtest.h>
#include <iostream>

TEST(Disjoint_Test, creation)
{
    disjoint_sets D(10);

    D.merge(0, 3);
    D.merge(2, 4);
    D.merge(6, 3);
    D.merge(9, 0);
    ASSERT_TRUE(D.are_in_same_connected_component(0, 6));
    ASSERT_TRUE(!D.are_in_same_connected_component(2, 9));

    ASSERT_EQ(D.get_num_elements_in_component(0), 4);
    ASSERT_EQ(D.get_num_elements_in_component(2), 2);
}