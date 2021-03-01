//
// Created by Gleb Dmitrievich on 1/19/21.
//

#include "node.hpp"

// Node class methods
Output Node::output(size_t index) const
{
    if (index >= m_outputs.size()) throw std::runtime_error("Index out of range");
    return m_outputs[index];
}

Input Node::input(size_t index) const
{
    if (index >= m_inputs.size()) throw std::runtime_error("Index out of range");
    return m_inputs[index];
}

std::shared_ptr<Node> Node::input_node(size_t index) const
{
    if (index >= m_inputs.size()) throw std::runtime_error("Index out of range");
    return m_inputs.at(index).get_src_node();
}

Node::Node(std::vector<Output> outputs)
{
    for (size_t i = 0; i < outputs.size(); ++i)
    {
        m_inputs.emplace_back(this, outputs[i], i);
    }
}

Node::~Node() {
    for (auto & input : m_inputs) {
        input.remove_output();
    }
}

void Node::set_output_size(size_t size)
{
    if (size <= m_outputs.size())
    {
        return;
    }

    for (size_t i = 0; i < size; ++i)
    {
        m_outputs.emplace_back(this, i);
    }
}

// Output class methods
std::vector<Input> Output::target_inputs() const
{
    std::vector<Input> inputs;
    for (auto & p : *m_target_inputs)
    {
        inputs.emplace_back(p.first->input(p.second));
    }
    return inputs;
}

void Output::add_target_input(const Input & input)
{
    m_target_inputs->insert({input.get_node(), input.get_index()});
}

void Output::replace(const Output & output)
{
    for (auto && target_input : target_inputs())
    {
        target_input.replace_output(output);
    }
    m_target_inputs->clear();
}