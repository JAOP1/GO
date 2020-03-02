#pragma once
#include <torch/torch.h>
#include <vector>

// You can for example just read your data and directly store it as tensor.
torch::Tensor read_data(const std::string& loc)
{
    torch::Tensor tensor = ...

      // Here you need to get your data.

      return tensor;
};

class custom_loader : public torch::data::Dataset<MyDataset>
{
private:
    torch::Tensor states_, labels_;

public:
        explicit MyDataset(const std::string& loc_states, const std::string& loc_labels) 
            : states_(read_data(loc_states)),
              labels_(read_data(loc_labels) {   };

        torch::data::Example<> get(size_t index) override;
};

torch::data::Example<> custom_loader::get(size_t index)
{
    // You may for example also read in a .csv file that stores locations
    // to your data and then read in the data at this step. Be creative.
    return {states_[index], labels_[index]};
}