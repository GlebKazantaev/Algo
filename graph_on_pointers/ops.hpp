//
// Created by Gleb Dmitrievich on 1/12/21.
//

#pragma once

#include <memory>
#include <vector>
#include <string>

#include "node.hpp"

class Parameter : public Node {
public:
    Parameter(const Shape & shape) : Node({}), m_shape(shape) {
        constructor_validate();
    }

    void validate() override {
        set_output_size(1);
        output(0).set_shape(m_shape);
    }

private:
    Shape m_shape;
};

class Result : public Node {
public:
    Result(const Output & input_data) : Node({input_data}) {
        constructor_validate();
    }
};

class Relu : public Node {
public:
    Relu(const Output & input_data) : Node({input_data}) {
        constructor_validate();
    }

    void validate() override {
        set_output_size(1);
        output(0).set_shape(input(0).get_shape());
    }
};
