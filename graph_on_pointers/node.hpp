//
// Created by Gleb Dmitrievich on 1/12/21.
//

#pragma once

#include <memory>
#include <vector>
#include <string>
#include <set>

#include "tensor.hpp"

class Input;
class Output;

class Node : public std::enable_shared_from_this<Node>
{
public:
    Node(std::vector<Output> outputs);
    ~Node();

    Input input(size_t index) const;
    Output output(size_t index) const;

    const std::vector<Input> & inputs() const { return m_inputs; };
    const std::vector<Output> & outputs() const { return m_outputs; };

    std::shared_ptr<Node> input_node(size_t index) const;

    virtual void validate() {};

    void constructor_validate()
    {
        validate();
    }

    void set_name(const std::string & name) { m_name = name; }

    const std::string & get_name() const { return m_name; }

protected:
    void set_output_size(size_t size);

private:
    std::vector<Input> m_inputs;
    std::vector<Output> m_outputs;
    std::string m_name;
};

class PortBase {
public:
    PortBase(Node * node, size_t index) : m_node(node), m_index(index) {}

    Node * get_node() const { return m_node; }

    size_t get_index() const { return m_index; }

protected:
    Node * m_node;
    size_t m_index;
};

class Output : public PortBase
{
public:
    Output() = delete;

    Output(Node * node, size_t id) : PortBase(node, id)
    {
        m_tensor = std::make_shared<Tensor>(Shape{});
        m_target_inputs = std::make_shared<std::set<std::pair<Node *, size_t>>>();
    }

    const Shape & get_shape() const { return m_tensor->get_shape(); }

    void set_shape(const Shape & shape) { m_tensor->set_shape(shape); }

    void add_target_input(const Input & input);

    std::vector<Input> target_inputs() const;

    void replace(const Output &);

    void remove_target_input(Node * node, size_t index) {
        m_target_inputs->erase({node, index});
    }

private:
    std::shared_ptr<Tensor> m_tensor;
    std::shared_ptr<std::set<std::pair<Node *, size_t>>> m_target_inputs;
};

class Input : public PortBase
{
public:
    Input() = delete;

    Input(Node * node, const Output & output, size_t id)
        : PortBase(node, id), m_output(output)
    {
        // hold input node pointer
        m_src_node = std::make_shared<std::shared_ptr<Node>>(m_output.get_node()->shared_from_this());
        m_output.add_target_input(*this);
    }

    std::shared_ptr<Node> get_src_node() const { return *m_src_node; }

    const Shape & get_shape() const { return m_output.get_shape(); }

    void replace_output(const Output & output)
    {
        remove_output();
        m_output = output;
        *m_src_node = m_output.get_node()->shared_from_this();
        m_output.add_target_input(*this);
    }

    void remove_output() {
        m_output.remove_target_input(m_node, m_index);
    }

private:
    std::shared_ptr<std::shared_ptr<Node>> m_src_node;
    Output m_output;
};