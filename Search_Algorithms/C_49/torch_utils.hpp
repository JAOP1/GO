#pragma once
#include <torch/torch.h>

namespace nn_utils
{

class neural_options
{
public:
    struct conv_opt;
    struct linear_opt;

    explicit neural_options() {}

    explicit neural_options(std::vector<conv_opt> conv_list,
                            std::vector<linear_opt> linear_list)
    {
        for (auto& e : conv_list)
            conv_layers.emplace_back(e);

        for (auto& e : linear_list)
            linear_layers.emplace_back(e);
    }

    void add_conv_options(int in_c, int out_c, int kernel)
    {
        conv_layers.emplace_back(in_c, out_c, kernel);
    }

    void add_linear_options(int input, int output)
    {
        linear_layers.emplace_back(input, output);
    }

    conv_opt get_options_convlayer(int i) { return conv_layers[i]; }

    linear_opt get_options_linearlayer(int i) { return linear_layers[i]; }

    bool pool_layer(int ind) const { return conv_layers[ind].pool == 1; }

    int num_linearlayers() const { return linear_layers.size(); }

    int num_convlayers() const { return conv_layers.size(); }

    struct conv_opt
    {
        conv_opt(int in, int out, int kernel, int pool_ = 0)
            : in_channels(in), out_channels(out), kernel_size(kernel), pool(pool_)
        {}
        int in_channels;
        int out_channels;
        int kernel_size;
        int pool;
    };

    struct linear_opt
    {
        linear_opt(int in, int out) : input(in), output(out) {}
        int input;
        int output;
    };

private:
    std::vector<conv_opt> conv_layers;
    std::vector<linear_opt> linear_layers;
};

torch::Device get_device(bool verbose = false)
{
    torch::DeviceType device_type;
    if (torch::cuda::is_available())
    {
        // std::cout << "Cuda diveces:" << torch::cuda::device_count() <<
        // std::endl;
        device_type = torch::kCUDA;
    }
    else
    {
        device_type = torch::kCPU;
    }

    if (verbose)
    {
        std::string output_ = "Training on ";
        output_ += (device_type == torch::kCUDA ? "GPU." : "CPU.");
        std::cout << output_ << std::endl;
    }

    torch::Device device(device_type);
    return device;
}

template <class NN>
void load_net(std::string Path, NN& Model, neural_options& options_)
{
    if (std::filesystem::exists(Path))
    {
        std::cout << "Loading model in path: " << Path << std::endl;
        auto load_model_ptr = std::make_shared<NN>(options_);
        torch::load(load_model_ptr, Path);
        Model = *load_model_ptr;
    }
}

template <class NN>
void save_net(std::string Path, NN& Model)
{
    std::cout << "Saving model in path: " << Path << std::endl;
    auto save_model_ptr = std::make_shared<NN>(Model);
    torch::save(save_model_ptr, Path);
}
} // namespace nn_utils