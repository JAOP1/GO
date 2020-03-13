#include "../GUI/BoardGame.hpp"
#include <gtest/gtest.h>
#include <iostream>

TEST(BoadGameTest , Creation)
{
    Graph G = graphs::Grid(4,4);
    BoardGame Go(G , 0.5);

    std::vector<char> initial_state(4*4, 'N');

    ASSERT_EQ(Go.show_current_state() , initial_state);
}