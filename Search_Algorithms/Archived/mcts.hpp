#pragma once

#include "../BoardGame.hpp"
#include "../Extra/hash_utilities.hpp"
#include <cmath>
#include <limits>
#include <stack>
#include <unordered_map>
#include <vector>

using Action = int;

//----------------------------------------------------------------------------------

class mcts
{
public:
    mcts(int num_simulation,
         int num_times,
         char player,
         bool do_memoization = true,
         double pruning_portion = 350)
        : simulation_num(num_simulation)
        , times_to_repeat(num_times)
        , player_(player)
        , do_memoization_(do_memoization) // nuevo
        , pruning_portion_(pruning_portion)
    {}

    Action search(const BoardGame& current_board);

private:
    class Node
    {
    public:
        Node(const BoardGame& game_state, Action action, Node* Parent = nullptr)
            : parent_(Parent), state_(game_state), applied_action_(action)
        {}

        ~Node() = default;

        void add_child(Action action_)
        {
            auto game_state_ = state_;
            game_state_.make_action(action_);
            children_.emplace_back(game_state_, action_, this);
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

        BoardGame state() { return state_; }

        int num_visits() const { return num_visits_; }

        Action action() const { return applied_action_; }

        Node* parent() const { return parent_; }

        bool is_root() const { return parent_ == nullptr; }

        bool is_leaf() const { return children_.empty(); }

        std::vector<Node>& children() { return children_; }

        std::vector<char> show_board() { return state_.show_current_state(); }

    private:
        Node* parent_{nullptr};
        BoardGame state_;
        Action applied_action_ = -1; // Null action
        int num_visits_ = 0;
        double value_ = 0.0;
        std::vector<Node> children_;
    };
    // Parametros
    int simulation_num;
    int times_to_repeat;
    bool do_memoization_; // nuevo
<<<<<<< HEAD
    int tree_size = 0;
=======
    char player_;
    double pruning_portion_;
>>>>>>> 0126554cfc67a62004616659dc8b650097c9d326

    double tree_size = 0;
    using state_t = std::vector<char>;
    using memoizer = std::unordered_map<state_t, double, polynomial_hash<char>>;
    memoizer global_information; // nuevo

    double get_pruning_portion() const
    {
        double percentage = tree_size/pruning_portion_;
        return 1.0 - percentage;
    }

    Node& child_highest_confidence(Node& node);

    double Simulation(Node& node);

    void Backpropagation(Node& leaf, const double reward, const int num_visits);

    void Expand(Node& node);

    Node& Select(Node& node);

    double get_reward_from_one_simulation(int num_steps, BoardGame state);
};

Action mcts::search(const BoardGame& current_board)
{
    Node root(current_board, -1, nullptr);
    std::cout << "Buscando la mejor acción para el jugador: "
              << current_board.player_status() << std::endl;
    std::cout << '[';

    for (int i = 0; i < times_to_repeat; ++i)
    {
<<<<<<< HEAD:Search_Algorithms/Archived/mcts.hpp
        std::cout << '*';
=======
<<<<<<< HEAD
        std::cout << "Step " << i << " of " << times_to_repeat << std::endl;
        std::cout << "Size tree " << tree_size << std::endl;
=======

        std::cout << "Step " << i << " of " << times_to_repeat << std::endl;
        std::cout << "Current size of tree " << tree_size << std::endl;
        
>>>>>>> 0126554cfc67a62004616659dc8b650097c9d326
>>>>>>> master:GUI/Include/Search_Algorithms/MCTS.hpp
        Node& leaf = Select(root);
        // std::cout << "Ha finalizado etapa de seleccion" << std::endl;
        Expand(leaf);
        // std::cout << "Ha finalizado etapa de expandir" << std::endl;
        double simulation_reward = 0;
        int total_children = 0;
        for (auto& child : leaf.children())
        {

            double reward = Simulation(child);
            simulation_reward += reward;
            total_children++;

            child.update_stats(reward, 1);
            // std::cout << "Ha finalizado etapa de simulacion" << std::endl;
            // std::cout << "Ha finalizado etapa de propagacion" << std::endl;

            //-------------------- nuevo -------------------------------------

            auto vstate = child.show_board();
            if (do_memoization_)
            {
                auto iter = global_information.find(vstate);
                if (iter == global_information.end())
                    global_information.insert({vstate, simulation_reward});
            }

            //----------------------------------------------------------------
        }

<<<<<<< HEAD:Search_Algorithms/Archived/mcts.hpp
        Backpropagation(leaf, simulation_reward/total_children, total_children);

        // std::cout << "Tableros en memoria: " << global_information.size() <<
        // std::endl;
    }
    std::cout << ']' << std::endl;
=======
        // std::cout << "Tableros en memoria: " << global_information.size() <<
        // std::endl;
    }
<<<<<<< HEAD
=======

>>>>>>> 0126554cfc67a62004616659dc8b650097c9d326
>>>>>>> master:GUI/Include/Search_Algorithms/MCTS.hpp
    tree_size = 0;
    Node best_choice = child_highest_confidence(root);
    // std::cout<<"Encuentra el que da mayor recompensa"<<std::endl;
    return best_choice.action();
}

double mcts::Simulation(Node& node)
{
    double reward = 0.0;

    for (int i = 0; i < simulation_num; ++i)
    {
        reward += get_reward_from_one_simulation(60, node.state());
    }

    return reward/simulation_num;
}

void mcts::Backpropagation(Node& leaf, const double reward, const int num_visits)
{
    Node* node = &leaf;
    while (!node->is_root())
    {
        node->update_stats(reward, num_visits);
        node = node->parent();
    }
    node->update_stats(reward, num_visits);
}

void mcts::Expand(Node& node)
{
    BoardGame state = node.state();
<<<<<<< HEAD:Search_Algorithms/Archived/mcts.hpp
    // std::vector<vertex> actions_set =
    // state.get_available_sample_cells(get_pruning_portion());
    std::vector<vertex> actions_set = state.get_available_sample_cells(1.0);

    // std::cout << "Conjunto de acciones " << actions_set.size() << std::endl;
=======
<<<<<<< HEAD
    std::vector<vertex> actions_set = state.available_cells();
    tree_size += actions_set.size();
=======
    // std::vector<vertex> actions_set =
    // state.get_available_sample_cells(get_pruning_portion());
    std::vector<vertex> actions_set = state.get_available_sample_cells(1.0);

>>>>>>> 0126554cfc67a62004616659dc8b650097c9d326
    // std::cout << "Conjunto de acciones " << actions_set.size() << std::endl;
>>>>>>> master:GUI/Include/Search_Algorithms/MCTS.hpp
    tree_size += actions_set.size();

    for (auto v : actions_set)
        node.add_child(v);
}

mcts::Node& mcts::Select(Node& node)
{
    Node* current = &node;

    while (!current->is_leaf())
    {
        current = &child_highest_confidence(*current);
    }

    return *current;
}

mcts::Node& mcts::child_highest_confidence(Node& node)
{
    double confidence = std::numeric_limits<double>::lowest();
    Node* child_highest_confidence_ = nullptr;

    for (auto& child : node.children())
    {
        if (confidence < child.confidence_of_node())
        {
            child_highest_confidence_ = &child;
            confidence = child.confidence_of_node();
        }
    }

    if (child_highest_confidence_ == nullptr)
    {
        std::cout << node.children().size() << std::endl;
        std::cout << confidence << std::endl;
    }

    assert(child_highest_confidence_ != nullptr);

    return *child_highest_confidence_;
}

double mcts::get_reward_from_one_simulation(int num_steps, BoardGame state)
{
    for (int i = 0; i < 60 && !state.is_complete(); ++i)
    {
        Action cell = state.random_action();
        if (cell == -1)
            break;

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
    // std::cout<< "Ha finalizado el proceso de conseguir recompensa"<<std::endl;
    return state.reward(player_);
}
