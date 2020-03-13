#include "../GUI/BoardGame.hpp"
#include <gtest/gtest.h>
#include <iostream>

TEST(BoardGameTest, Creation)
{
    Graph G = graphs::Grid(4, 4);
    BoardGame Go(G, 0.5);

    std::vector<char> initial_state(25, 'N');
    ASSERT_EQ(Go.show_current_state(), initial_state);
}

/*
TEST(BoardGameTest, )

TEST(BoardGameTest, Valid_action)
{
    Graph G = graphs::Grid(4, 4);
    BoardGame Go(G, 0.5);

    std::vector<char> state(25, 'N');
}
*/