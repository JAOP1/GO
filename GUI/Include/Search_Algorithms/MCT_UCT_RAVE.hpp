#pragma once

#include "../BoardGame.hpp"
#include "../Extra/hash_utilities.hpp"
#include <cmath>
#include <limits>
#include <stack>
#include <unordered_map>
#include <vector>

using Action = int;
using vertex = std::int64_t ;
//----------------------------------------------------------------------------------
struct RAVE_Map
{

    void clear()
    {
        N.clear();
        N_.clear();
        Q_.clear();
        Q.clear();
    }
    std::unordered_map<vertex,int> N;
    std::unordered_map<vertex,int> N_;
    std::unordered_map<vertex,double> Q;
    std::unordered_map<vertex,double> Q_;
};




class MC_RAVE
{
public:
    MC_RAVE(int num_simulation,
         int num_times,
         char player,
         double pruning_portion = 350)
        : simulation_num(num_simulation)
        , times_to_repeat(num_times)
        , player_(player)
        , pruning_portion_(pruning_portion)
    {}

    Action search(const BoardGame& current_board);

private:
    class RAVE_Node
    {
    public:
        explicit RAVE_Node(const BoardGame& game_state, Action action,RAVE_Node* Parent = nullptr) :
            parent_(Parent),
            state_(game_state),
            applied_action_(action)
        {}

        explicit RAVE_Node( const BoardGame& game_state ,  Action action, RAVE_Map& stats , RAVE_Node* Parent = nullptr) :
            parent_(Parent),
            state_(game_state),
            applied_action_(action),
            N_(stats.N_[action]),
            N(stats.N[action]),
            Q(stats.Q[action] / stats.N[action]),
            Q_(stats.Q_[action])
        {}

        ~RAVE_Node() = default;

        void add_child(Action action_, RAVE_Map& stats)
        {
            auto game_state_ = state_;
            game_state_.make_action(action_);
            children_.emplace_back(game_state_, action_, stats, this);
        }

        void update_stats(double average_reward, int total_visits)
        {
            N += total_visits;
            Q += average_reward;
        }

        double confidence_of_node() const
        {
            auto b = 1.0;
            double B = N_ / (N  + N_ + 4.0 * N * N_ * std::pow(b,2));

            return (1.0 - B) * Q + B * Q_;
        }

        BoardGame state() { return state_; }

        int num_visits() const { return N; }

        Action action() const { return applied_action_; }

        RAVE_Node* parent() const { return parent_; }

        bool is_root() const { return parent_ == nullptr; }

        bool is_leaf() const { return children_.empty(); }

        std::vector<RAVE_Node>& children() { return children_; }

        std::vector<char> show_board() { return state_.show_current_state(); }

    private:
        RAVE_Node* parent_{nullptr};
        BoardGame state_;
        Action applied_action_ = -1; // Null action
        int N = 0;
        int N_ = 0;
        double Q_ = 0.0;
        double Q = 0.0;

        std::vector<RAVE_Node> children_;
    };
    // Parametros
    int simulation_num;
    int times_to_repeat;
    RAVE_Map simulation_stats;
    char player_;
    double pruning_portion_;

    double tree_size = 0;
    using state_t = std::vector<char>;

    double get_pruning_portion() const
    {
        double percentage = tree_size/pruning_portion_;
        return 1.0 - percentage;
    }

    RAVE_Node& child_highest_confidence(RAVE_Node& node, int max_min_val);

    RAVE_Node& Select(RAVE_Node& node);

    void Backpropagation(RAVE_Node& leaf, const double reward, const int num_visits);

    void Expand_by_RAVE(RAVE_Node& node);

    double Simulations_by_RAVE(RAVE_Node& node);

    std::vector<Action> simulation_recording(int num_steps, BoardGame state);


};



Action MC_RAVE::search(const BoardGame& current_board)
{
    RAVE_Node root(current_board, -1, nullptr);

    std::cout << "Buscando la mejor acción para el jugador: "
              << current_board.player_status() << std::endl;
    std::cout << '[';

    for (int i = 0; i < times_to_repeat; ++i)
    {
        std::cout << '*' << std::flush;

        RAVE_Node& leaf = Select(root);

        auto reward = Simulations_by_RAVE(leaf);

        Expand_by_RAVE(leaf);

        Backpropagation(leaf,reward , leaf.children().size());

        simulation_stats.clear();

    }
    std::cout << ']' << std::endl;
    tree_size = 0;
    RAVE_Node best_choice = child_highest_confidence(root, 1);

    return best_choice.action();
}

//Aun falta hacer algunas modificaciones a esta función y listo :D
double MC_RAVE::Simulations_by_RAVE(MC_RAVE::RAVE_Node &node)
{
    double reward = 0.0;
    double tmp_reward;
    for (int i = 0; i < simulation_num; ++i)
    {
        auto actions =  simulation_recording(60, node.state());
        tmp_reward =  actions.back();
        reward += tmp_reward;
        simulation_stats.Q[actions[0]] = tmp_reward;
        simulation_stats.N[actions[0]]++;

        for(int  i = 1 ;  i < actions.size()-1 ; ++i)
        {
            simulation_stats.Q_[actions[i]] = tmp_reward;
            simulation_stats.N_[actions[i]]++;
        }

    }

    return reward/simulation_num;
}

void MC_RAVE::Backpropagation(RAVE_Node& leaf, const double reward, const int num_visits)
{
    RAVE_Node* node = &leaf;
    while (!node->is_root())
    {
        node->update_stats(reward, num_visits);
        node = node->parent();
    }
    node->update_stats(reward, num_visits);
}

void MC_RAVE::Expand_by_RAVE(MC_RAVE::RAVE_Node &node)
{
    BoardGame state = node.state();
    std::vector<vertex> actions_set = state.get_available_sample_cells(1.0);


    tree_size += actions_set.size();

    for (auto v : actions_set)
    {
        if(simulation_stats.Q.find(v) == simulation_stats.Q.end())
            continue;

        node.add_child(v, simulation_stats);
    }

}

MC_RAVE::RAVE_Node& MC_RAVE::Select(RAVE_Node& node)
{
    RAVE_Node* current = &node;
    int max_min = 1;

    while (!current->is_leaf())
    {
        current = &child_highest_confidence(*current, max_min);
        max_min *= -1;
    }

    return *current;
}

MC_RAVE::RAVE_Node& MC_RAVE::child_highest_confidence(RAVE_Node& node, int max_min_val)
{
    double confidence = std::numeric_limits<double>::lowest();
    double tmp_confidence;
    RAVE_Node* child_highest_confidence_ = nullptr;

    std::cout<<"The confidence is: "<<confidence<<std::endl;

    for (auto& child : node.children())
    {
        tmp_confidence = child.confidence_of_node() * max_min_val;
        std::cout<<"tmp confidence "<<tmp_confidence<<std::endl;

        if (confidence < tmp_confidence)
        {
            child_highest_confidence_ = &child;
            confidence = tmp_confidence;
        }
    }

    if (child_highest_confidence_ == nullptr)
    {
        std::cout.put('\n');
        std::cout << node.children().size() << std::endl;
        std::cout << confidence << std::endl;
        std::cout<<node.is_leaf()<<std::endl;
    }

    assert(child_highest_confidence_ != nullptr);

    return *child_highest_confidence_;
}

//El ultimo es la recompensa.
std::vector<Action> MC_RAVE::simulation_recording(int num_steps, BoardGame state)
{
    std::vector<Action> made_actions;
    for (int i = 0; i < 60 && !state.is_complete(); ++i)
    {
        Action cell = state.random_action();
        made_actions.push_back(cell);

        state.make_action(cell);
    }
    made_actions.push_back(state.reward(player_));

    return made_actions;
}
