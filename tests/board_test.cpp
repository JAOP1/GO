#include "../GUI/BoardGame.hpp"
#include <gtest/gtest.h>
#include <iostream>

TEST(Board_Test, Creation)
{
    Graph G = graphs::Grid(4, 4);
    BoardGame Go(G, 0.5);

    std::vector<char> initial_state(25, 'N');
    ASSERT_EQ(Go.show_current_state(), initial_state);
}

TEST(Board_Test, Valid_action)
{
    Graph G = graphs::Grid(3, 3); // Grid size is 4 x 4.
    BoardGame Go(G, 0.5);

    std::vector<char> state(16, 'N');
    state[1] = 'B';
    state[4] = 'B';
    state[5] = 'W';

    Go.make_action(1); // Black has done an action in node 1.
    Go.make_action(-1); // White pass.
    Go.make_action(4); // Black has done an action in node 4.
    Go.make_action(5); // White.

    auto v = Go.get_current_node_state(0);
    auto v1 = Go.get_current_node_state(1);
    auto v2 = Go.get_current_node_state(4);

    ASSERT_EQ(Go.show_current_state(), state);
    ASSERT_EQ(Go.player_status(), 'B');
    ASSERT_EQ(v.type, 'N');
    ASSERT_EQ(v1.type, 'B');
    ASSERT_EQ(v2.type, 'B');
    ASSERT_EQ(v1.libertiesNodes.size(), 2);
    ASSERT_EQ(v2.libertiesNodes.size(), 2);
    ASSERT_EQ(v.libertiesNodes.size(), 0);
    ASSERT_TRUE(Go.is_valid_move(0));
    ASSERT_TRUE(Go.is_valid_move(2));
    ASSERT_TRUE(!Go.is_valid_move(5));
}

TEST(Board_Test, Eliminate)
{
    Graph G = graphs::Complete(3);
    BoardGame Go(G);
    Go.make_action(1);
    Go.make_action(2);

    std::vector<char> state(3, 'N');
    state[1] = 'B';
    state[2] = 'W';

    ASSERT_EQ(Go.show_current_state(), state);

    state[2] = 'N';
    state[0] = 'B';
    Go.make_action(0);

    ASSERT_EQ(Go.show_current_state(), state);
}

TEST(Board_Test, reward)
{
    Graph G = graphs::Grid(2, 2);
    BoardGame Go(G);

    Go.make_action(4);
    Go.make_action(-1);
    Go.make_action(3);
    Go.make_action(-1);
    Go.make_action(1);

    ASSERT_EQ(Go.reward('B'), 1);
    ASSERT_EQ(Go.reward('W'), -1);
}