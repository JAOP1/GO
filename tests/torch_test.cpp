/*
#include <iostream>
#include <numeric>
#include <torch/torch.h>
#include <vector>

TEST(TensorTest, ToOptionsWithRequiresGrad) {
  {
    // Respects requires_grad
    auto tensor = torch::empty({3, 4}, at::requires_grad());
    ASSERT_TRUE(tensor.requires_grad());

    tensor = tensor.to(at::kDouble);
    ASSERT_TRUE(tensor.requires_grad());

}

*/
