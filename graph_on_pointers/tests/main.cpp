//
// Created by Gleb Dmitrievich on 1/10/21.
//

#include "../graph.hpp"
#include "../node.hpp"
#include "../ops.hpp"

#include <queue>

#include <iostream>

void print(Shape shape)
{
    std::cout << "SHAPE: ";
    for(auto && dim : shape)
    {
        std::cout << dim << ", ";
    }
    std::cout << std::endl;
}

std::shared_ptr<Function> create_function()
{
    // Create Function with following operations:
    // Parameter(0)->ReLU(0)->Result

    auto param = std::make_shared<Parameter>(Shape{1, 3, 64, 64});
    param->set_name("param");

    auto relu = std::make_shared<Relu>(param->output(0));
    relu->set_name("relu");

    auto result = std::make_shared<Result>(relu->output(0));
    result->set_name("result");

    return std::make_shared<Function>(ResultVector{result}, ParameterVector{param});
}

void traverse_function_from_outputs(std::shared_ptr<Function> f)
{
    // Traverse Function starting from Result operations to Parameters
    std::queue<Node *> q;
    for (const auto & result : f->get_results())
    {
        q.push(result.get());
    }

    std::cout << "Start traversing from outputs:\n";

    while(!q.empty())
    {
        auto node = q.front();
        q.pop();

        std::cout << node->get_name() << std::endl;
        for (const auto & input : node->inputs())
        {
            q.push(input.get_src_node().get());
        }
    }

    std::cout << std::endl;
}

void traverse_function_from_inputs(std::shared_ptr<Function> f)
{
    // Traverse Function starting from Result operations to Parameters
    std::queue<Node *> q;
    for (const auto & parameter : f->get_parameters())
    {
        q.push(parameter.get());
    }

    std::cout << "Start traversing from inputs:\n";

    while(!q.empty())
    {
        auto node = q.front();
        q.pop();

        std::cout << node->get_name() << std::endl;
        for (const auto & output : node->outputs())
        {
            for (const auto & input : output.target_inputs())
            {
                q.push(input.get_node());
            }
        }
    }

    std::cout << std::endl;
}

void replace_relu(std::shared_ptr<Function> f)
{
    auto param = f->get_parameters()[0];
    auto relu = param->output(0).target_inputs()[0].get_node()->shared_from_this();

    auto new_relu = std::make_shared<Relu>(param->output(0));
    new_relu->set_name("new_relu");

    relu->output(0).replace(new_relu->output(0));
    std::cout << "Relu target inputs: " << relu->output(0).target_inputs().size() << "\n";
}

int main()
{
    auto f = create_function();
    traverse_function_from_inputs(f);
    traverse_function_from_outputs(f);

    replace_relu(f);

    traverse_function_from_inputs(f);
    traverse_function_from_outputs(f);
}