#include "../Search_Algorithms/C_49/encoders.hpp"
#include "../Include/Extra/Graph.hpp"
#include "../Search_Algorithms/C_49/game_structure.hpp"
#include "../Include/BoardGame.hpp"
#include <gtest/gtest.h>
#include <iostream>


using vertex = std::int64_t;

TEST(Encoder , SimpleEncoderTest)
{
    int rows = 1;
    int columns = 1;
    Graph G = graphs::Grid(rows,columns);
    BoardGame BG(G);
    game D;
    SimpleEncoder encoder(G);

    std::vector<vertex> actions = BG.get_available_sample_cells(1.0);
    D.add_game_state({0.25,0.25,0.25,0.25}, BG.show_current_state(), actions);

    BG.make_action(0);
    actions = BG.get_available_sample_cells(1.0);
    D.add_game_state({0,0.333,0.333,0.333},BG.show_current_state() , actions);

    BG.make_action(1);
    actions = BG.get_available_sample_cells(1.0);
    D.add_game_state({0,0,0.5,0.5} , BG.show_current_state() , actions);

    D.set_reward( BG.reward('B'));


    auto vector_torch_planes = encoder.Encode_episode<game>(D);
    
    c10::IntArrayRef size_data = {3, G.num_vertices() , G.num_vertices()};
    c10::IntArrayRef size_target = {5};

    for(int  i = 0 ; i < vector_torch_planes.size() ; ++i)
    {
      ASSERT_TRUE(size_data == vector_torch_planes[i].data.sizes());
      ASSERT_TRUE(size_target == vector_torch_planes[i].target.sizes());
    
    }

}
