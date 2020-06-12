
#pragma once
#include "../Include/BoardGame.hpp"
#include "../Include/Extra/External/tqdm.h"
#include "../Include/Extra/hash_utilities.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <stack>
#include <torch/torch.h>
#include <unordered_map>
#include <utility>
#include <vector>

using Action = int;
using vertex = int;

int get_random_action(torch::Tensor prob_, int num_actions)
{
    double value = (double)rand()/RAND_MAX;

    double accumulative_result = 0.0;
    for (int i = 0; i < num_actions - 1; ++i)
    {
        accumulative_result += prob_[0][i].item<double>();
        if (value <= accumulative_result)
            return i;
    }
    // Pass action.
    return -1;
}
//----------------------------------------------------------------------------------
// Data structure.
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
    std::unordered_map<vertex, int> N;
    std::unordered_map<vertex, int> N_;
    std::unordered_map<vertex, double> Q;
    std::unordered_map<vertex, double> Q_;
};

class RAVE_Node
{
public:
    /*
    explicit RAVE_Node(const BoardGame& game_state,
                        Action action,
                        int height,
                        RAVE_Node* Parent = nullptr)
        : parent_(Parent),
          state_(game_state),
          applied_action_(action),
          height_(height)
    { objectCount++; }

    */

    RAVE_Node(const BoardGame& game_state,
              Action action,
              int height,
              RAVE_Map& stats,
              RAVE_Node* Parent = nullptr)
        : parent_(Parent)
        , state_(game_state)
        , applied_action_(action)
        , height_(height)
    {
        objectCount++;

        if (stats.N_.find(action) != stats.N_.end())
        {
            N_ = stats.N_[action];
            N = stats.N[action];
            Q = stats.Q[action]/stats.N[action];
            Q_ = stats.Q_[action];
        }
        else
        {
            N_ = 0;
            N = 0;
            Q = 0.0;
            Q_ = 0.0;
        }
    }

    ~RAVE_Node() { objectCount--; }

    void add_child(Action action_, RAVE_Map& stats)
    {
        auto game_state_ = state_;
        game_state_.make_action(action_);
        std::shared_ptr<RAVE_Node> child =
          std::make_shared<RAVE_Node>(game_state_, action_, height_ + 1, stats, this);
        children_.push_back(child);
    }
    void set_parent() { parent_ = nullptr; }

    void update_board(const BoardGame& game_state) { state_ = game_state; }

    void update_stats(double average_reward, int total_visits)
    {
        N += total_visits;
        Q += average_reward;
    }

    double confidence_of_node() const
    {
        auto b = 1.0;
        double B = N_ /
          (N + N_ + 4.0*N*N_*std::pow(b, 2) + 0.0000001); // Lo utilizo
                                                                // para que no
                                                                // sea 0/0

        return (1.0 - B)*Q + B*Q_;
    }

    BoardGame state() { return state_; }

    int num_visits() const { return N; }

    Action action() const { return applied_action_; }

    RAVE_Node* parent() const { return parent_; }

    bool is_root() const { return parent_ == nullptr; }

    bool is_leaf() const { return children_.empty(); }

    std::vector<std::shared_ptr<RAVE_Node>>& children() { return children_; }

    std::vector<char> show_board() { return state_.show_current_state(); }

    int get_height() const { return height_; }

    static int objectCount;

private:
    RAVE_Node* parent_{nullptr};
    BoardGame state_;
    Action applied_action_ = -1; // Null action
    int height_;
    int N = 0;
    int N_ = 0;
    double Q_ = 0.0;
    double Q = 0.0;

    std::vector<std::shared_ptr<RAVE_Node>> children_;
};

int RAVE_Node::objectCount = 0;

//----------------------------------------------------------------------------------
// RAVE declaration.
//----------------------------------------------------------------------------------

template <class Net_architect, class encoder>
class MC_RAVE
{
public:
    MC_RAVE(const BoardGame& current_board,
            Net_architect& Net,
            encoder& encode,
            int num_times,
            char player,
            int num_simulation = 30,
            int depth = 10)
        : simulation_num(num_simulation)
        , times_to_repeat(num_times)
        , player_(player)
        , depth_(depth)
        , Net_(Net)
        , encoder_(encode)
    {
        root = std::make_shared<RAVE_Node>(current_board,
                                           -1,
                                           0,
                                           simulation_stats,
                                           nullptr);
    }

    Action search(const BoardGame& current_board);

    void fit_precompute_tree(Action A)
    {

        bool is_changed = false;
        // std::cout<<"Accion del estado actual "<<root->action()<<std::endl;
        // std::cout<<"Size before to update tree is:
        // "<<root->objectCount<<std::endl;
        for (std::shared_ptr<RAVE_Node>& child : root->children())
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

        // Caso que no tenga información de ese segmento.
        if (!is_changed)
            is_unknown = true;
    }

    void reset_tree() { is_unknown = true; }

    void set_player(char player) { player_ = player; }

    // This give you probabilities vector.
    std::vector<double> get_probabilities_current_state() const;

private:
    // Parameters.
    int simulation_num;
    int times_to_repeat;
    RAVE_Map simulation_stats;
    char player_;
    int depth_;
    int Actions_space = -1;

    // Local information.
    Net_architect Net_;
    encoder encoder_;
    bool is_unknown = true;
    std::shared_ptr<RAVE_Node> root;

    using state_t = std::vector<char>;

    std::shared_ptr<RAVE_Node> child_highest_confidence(
      std::shared_ptr<RAVE_Node>& node, int max_min_val);

    std::shared_ptr<RAVE_Node> Select(std::shared_ptr<RAVE_Node>& node);

    void Backpropagation(std::shared_ptr<RAVE_Node> leaf,
                         const double reward,
                         const int num_visits);

    void Expand_by_RAVE(std::shared_ptr<RAVE_Node>& node);

    double Simulations_by_RAVE(std::shared_ptr<RAVE_Node>& node);

    std::vector<Action> simulation_recording(int num_steps, BoardGame state);
};

//----------------------------------------------------------------------------------
// Public RAVE functions.
//----------------------------------------------------------------------------------

template <class Net_architect, class encoder>
Action MC_RAVE<Net_architect, encoder>::search(const BoardGame& current_board)
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
    bar.set_label("MC RAVE");

    for (int i = 0; i < times_to_repeat; ++i)
    {
        bar.progress(i, times_to_repeat);

        std::shared_ptr<RAVE_Node> leaf = Select(root);

        auto reward = Simulations_by_RAVE(leaf);

        Expand_by_RAVE(leaf);

        Backpropagation(leaf, reward, leaf->children().size());

        simulation_stats.clear();
    }
    bar.finish();

    auto node = child_highest_confidence(root, 1);

    return node->action();
}

template <class Net_architect, class encoder>
std::vector<double>
MC_RAVE<Net_architect, encoder>::get_probabilities_current_state() const
{
    std::vector<double> probabilities(Actions_space + 1, 0);
    int id, total;
    double total_visits_counter = 0;

    for (std::shared_ptr<RAVE_Node>& child : root->children())
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
// Private RAVE functions.
//----------------------------------------------------------------------------------

template <class Net_architect, class encoder>
double MC_RAVE<Net_architect, encoder>::Simulations_by_RAVE(
  std::shared_ptr<RAVE_Node>& node)
{
    double reward = 0.0;
    double tmp_reward;
    for (int i = 0; i < simulation_num; ++i)
    {
        auto actions = simulation_recording(60, node->state());
        tmp_reward = actions.back();
        reward += tmp_reward;
        simulation_stats.Q[actions[0]] = tmp_reward;
        simulation_stats.N[actions[0]]++;

        for (int i = 1; i < actions.size() - 1; ++i)
        {
            simulation_stats.Q_[actions[i]] = tmp_reward;
            simulation_stats.N_[actions[i]]++;
        }
    }

    return reward/simulation_num;
}

template <class Net_architect, class encoder>
void MC_RAVE<Net_architect, encoder>::Backpropagation(
  std::shared_ptr<RAVE_Node> leaf, const double reward, const int num_visits)
{
    leaf->update_stats(reward, num_visits);
    if (leaf->is_root())
        return;

    RAVE_Node* node = leaf->parent();
    while (!node->is_root())
    {
        node->update_stats(reward, num_visits);
        node = node->parent();
    }
    node->update_stats(reward, num_visits);
}

template <class Net_architect, class encoder>
void MC_RAVE<Net_architect, encoder>::Expand_by_RAVE(std::shared_ptr<RAVE_Node>& node)
{
    int real_height = node->get_height() - root->get_height();
    if (real_height > depth_)
        return;

    BoardGame state = node->state();
    std::vector<vertex> actions_set = state.get_available_sample_cells(1.0);

    for (auto v : actions_set)
    {
        // It's because I haven't information yet.
        if (simulation_stats.Q.find(v) == simulation_stats.Q.end())
            continue;

        node->add_child(v, simulation_stats);
    }
}

template <class Net_architect, class encoder>
std::shared_ptr<RAVE_Node> MC_RAVE<Net_architect, encoder>::Select(
  std::shared_ptr<RAVE_Node>& node)
{
    std::shared_ptr<RAVE_Node> current = node;
    int max_min = 1;

    while (!current->is_leaf())
    {
        current = child_highest_confidence(current, max_min);
        max_min *= -1;
    }

    return current;
}

template <class Net_architect, class encoder>
std::shared_ptr<RAVE_Node> MC_RAVE<Net_architect, encoder>::child_highest_confidence(
  std::shared_ptr<RAVE_Node>& node, int max_min_val)
{
    double confidence = std::numeric_limits<double>::lowest();
    double tmp_confidence;
    std::shared_ptr<RAVE_Node> child_highest_confidence_;

    for (std::shared_ptr<RAVE_Node>& child : node->children())
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
        std::cout.put('\n');
        auto array = node->children();
        std::cout << "Available action set " << array.size() << std::endl;
        std::cout << "Action " << array[0]->action() << std::endl;
        std::cout << "node score by AMAF: " << array[0]->confidence_of_node()
                  << std::endl;
    }

    assert(child_highest_confidence_ != nullptr);

    return child_highest_confidence_;
}

// The last place in the array is the reward.
template <class Net_architect, class encoder>
std::vector<Action> MC_RAVE<Net_architect, encoder>::simulation_recording(
  int num_steps, BoardGame state)
{
    std::vector<Action> made_actions;
    for (int i = 0; i < 60 && !state.is_complete(); ++i)
    {
        int player = (state.player_status() == 'B' ? 0 : 1);
        auto encoded_state =
          encoder_.Encode_data(state.show_current_state(),
                               state.get_available_sample_cells(1.0),
                               player);

        auto net_output = Net_.forward(encoded_state);

        Action cell = get_random_action(net_output,
                                        state.Board.num_vertices() + 1);

        made_actions.push_back(cell);

        state.make_action(cell);
    }
    made_actions.push_back(state.reward(player_));

    return made_actions;
}
