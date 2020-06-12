#pragma once
#include "../Include/BoardGame.hpp"
#include "../Include/Extra/External/tqdm.h"
#include "../Include/Extra/hash_utilities.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <stack>
#include <unordered_map>
#include <utility>
#include <vector>

using Action = int;

//----------------------------------------------------------------------------------
// Data structure.
//----------------------------------------------------------------------------------
class Node
{
public:
    Node(const BoardGame& game_state,
         Action action,
         int height,
         Node* Parent = nullptr)
        : parent_(Parent)
        , state_(game_state)
        , applied_action_(action)
        , height_(height)
    {
        // objectCount++;
    }

    ~Node() = default;

    //~Node(){ objectCount--;};

    void add_child(Action action_)
    {
        auto game_state_ = state_;
        game_state_.make_action(action_);
        std::shared_ptr<Node> child =
          std::make_shared<Node>(game_state_, action_, height_ + 1, this);
        children_.push_back(child);
    }

    void update_stats(double average_reward, int total_visits)
    {
        num_visits_ += total_visits;
        value_ += average_reward;
    }

    double confidence_of_node() const
    {
        auto N = (is_root() ? 0 : parent_->num_visits());
        double C = 2.0;
        return value_ + C*std::sqrt(std::log1p(N)/num_visits_);
    }

    void update_board(const BoardGame& game_state) { state_ = game_state; }

    BoardGame state() const { return state_; }

    int get_height() const { return height_; }

    int num_visits() const { return num_visits_; }

    Action action() const { return applied_action_; }

    Node* parent() const { return parent_; }

    bool is_root() const { return parent_ == nullptr; }

    bool is_leaf() const { return children_.empty(); }

    std::vector<std::shared_ptr<Node>>& children() { return children_; }

    std::vector<char> show_board() const { return state_.show_current_state(); }

    void set_parent() { parent_ = nullptr; };

    static int objectCount;

private:
    Node* parent_{nullptr};
    BoardGame state_;
    Action applied_action_ = -1; // Null action
    int num_visits_ = 0;
    double value_ = 0.0;
    int height_;
    std::vector<std::shared_ptr<Node>> children_;
};

// int Node::objectCount = 0;

//----------------------------------------------------------------------------------
// MCTS declaration.
//----------------------------------------------------------------------------------

class MCTS
{
public:
    explicit MCTS(const BoardGame& current_board,
                  int num_simulation,
                  int num_times,
                  char player,
                  bool do_memoization = true,
                  int depth = 10)
        :

        simulation_num(num_simulation)
        , times_to_repeat(num_times)
        , player_(player)
        , do_memoization_(do_memoization) // nuevo
        , depth_(depth)
    //, root(current_board , -1, 0 , nullptr)
    {
        root = std::make_shared<Node>(current_board, -1, 0, nullptr);
    }

    Action search(const BoardGame& current_board);

    void fit_precompute_tree(Action A)
    {

        bool is_changed = false;
        for (std::shared_ptr<Node>& child : root->children())
        {
            if (child->action() == A)
            {
                is_changed = true;
                root = std::move(child);
                root->set_parent();
                // root = child;
                break;
            }
        }

        if (!is_changed)
            is_unknown = true;
    }

    void reset_tree() { is_unknown = true; }

    void set_player(char player) { player_ = player; }

    std::vector<double> get_probabilities_current_state() const;

private:
    // Parametros
    int simulation_num;
    int times_to_repeat;
    bool do_memoization_; // nuevo
    char player_;
    int depth_;
    int Actions_space = -1;

    // Local information.
    using state_t = std::vector<char>;
    using memoizer = std::unordered_map<state_t, double, polynomial_hash<char>>;
    memoizer global_information; // nuevo
    bool is_unknown = true;
    std::shared_ptr<Node> root;

    std::shared_ptr<Node> child_highest_confidence(std::shared_ptr<Node>& node,
                                                   int max_min_val);

    double Simulation(std::shared_ptr<Node> node);

    void Backpropagation(std::shared_ptr<Node> leaf,
                         const double reward,
                         const int num_visits);

    void Expand(std::shared_ptr<Node>& node);

    std::shared_ptr<Node> Select(std::shared_ptr<Node> node);

    double get_reward_from_one_simulation(int num_steps, BoardGame state);
};

//----------------------------------------------------------------------------------
// Public MCTS functions.
//----------------------------------------------------------------------------------

Action MCTS::search(const BoardGame& current_board)
{
    if (is_unknown)
    {
        Actions_space = current_board.Board.num_vertices();
        root->update_board(current_board);
        is_unknown = false;
    }

    std::vector<char> s = root->show_board();
    for (auto i : s)
        std::cout << i << " ";
    std::cout << std::endl;

    tqdm bar;
    bar.set_label("MCTS");

    for (int i = 0; i < times_to_repeat; ++i)
    {
        bar.progress(i, times_to_repeat);

        std::shared_ptr<Node> leaf = Select(root);

        Expand(leaf);

        double average_reward = 0;
        int total_children = 0;

        if (leaf->children().size() > 0)
        {

            for (std::shared_ptr<Node>& child : leaf->children())
            {

                double simulation_reward = Simulation(child);
                average_reward += simulation_reward;
                total_children++;

                child->update_stats(simulation_reward, 1);

                //-------------------- nuevo
                //-------------------------------------

                auto vstate = child->show_board();
                if (do_memoization_)
                {
                    auto iter = global_information.find(vstate);
                    if (iter == global_information.end())
                        global_information.insert({vstate, simulation_reward});
                }

                //----------------------------------------------------------------
            }
            average_reward /= total_children;
        }
        else
        {
            average_reward = Simulation(leaf);
            total_children++;
        }

        Backpropagation(leaf, average_reward, total_children);
    }
    bar.finish();

    auto node = child_highest_confidence(root, 1);
    return node->action();
}

std::vector<double> MCTS::get_probabilities_current_state() const
{
    std::vector<double> probabilities(Actions_space + 1, 0); // Por el pasar.
    int id, total;
    double total_visits_counter = 0;

    for (std::shared_ptr<Node>& child : root->children())
    {
        id = child->action();
        if (id == -1)
            id = probabilities.size() - 1;

        total = child->num_visits();
        probabilities[id] = total;
        total_visits_counter += total;
    }

    // Implica que si tiene algún hijo.
    if (total_visits_counter)
    {
        for (int i = 0; i < probabilities.size(); ++i)
            probabilities[i] /= total_visits_counter;
    }

    return probabilities;
}

//----------------------------------------------------------------------------------
// Private MCTS functions.
//----------------------------------------------------------------------------------

double MCTS::Simulation(std::shared_ptr<Node> node)
{
    double reward = 0.0;

    for (int i = 0; i < simulation_num; ++i)
    {
        reward += get_reward_from_one_simulation(60, node->state());
    }

    return reward/simulation_num;
}

void MCTS::Backpropagation(std::shared_ptr<Node> leaf,
                           const double reward,
                           const int num_visits)
{
    leaf->update_stats(reward, num_visits);
    if (leaf->is_root())
        return;

    Node* node = leaf->parent();
    while (!node->is_root())
    {
        node->update_stats(reward, num_visits);
        node = node->parent();
    }
    node->update_stats(reward, num_visits);
}

void MCTS::Expand(std::shared_ptr<Node>& node)
{
    int real_height = node->get_height() - root->get_height();
    if (real_height > depth_)
        return;

    BoardGame state = node->state();

    std::vector<vertex> actions_set = state.get_available_sample_cells(1.0);

    for (auto v : actions_set)
        node->add_child(v);
}

std::shared_ptr<Node> MCTS::Select(std::shared_ptr<Node> node)
{
    std::shared_ptr<Node> current = node;
    int max_min = 1;

    while (!current->is_leaf())
    {
        current = child_highest_confidence(current, max_min);
        max_min *= -1;
    }

    return current;
}

std::shared_ptr<Node> MCTS::child_highest_confidence(std::shared_ptr<Node>& node,
                                                     int max_min_val)
{
    double confidence = std::numeric_limits<double>::lowest();
    double tmp_confidence;
    std::shared_ptr<Node> child_highest_confidence_;

    for (std::shared_ptr<Node>& child : node->children())
    {
        tmp_confidence = child->confidence_of_node()*max_min_val;

        if (confidence < tmp_confidence)
        {
            child_highest_confidence_ = child;
            confidence = tmp_confidence;
        }
    }

    if (!child_highest_confidence_)
    {
        std::cout << node->children().size() << std::endl;
        std::cout << confidence << std::endl;
    }

    assert(child_highest_confidence_ != nullptr);

    return child_highest_confidence_;
}

double MCTS::get_reward_from_one_simulation(int num_steps, BoardGame state)
{
    for (int i = 0; i < 60 && !state.is_complete(); ++i)
    {
        Action cell = state.random_action();
        state.make_action(cell);

        //------------------------ nuevo --------------------------------
        auto vstate = state.show_current_state();
        if (do_memoization_)
        {
            auto iter = global_information.find(vstate);
            if (iter != global_information.end())
                return iter->second;
        }
        //--------------------------------------------------------------
    }
    return state.reward(player_);
}
