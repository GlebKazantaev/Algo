//
// Created by Gleb Dmitrievich on 1/12/21.
//

#pragma once

#include <memory>
#include <vector>
#include <string>

using Shape = std::vector<size_t>;

class Tensor {
public:
    Tensor(const Shape & shape, const std::string & name = "")
        : m_shape(shape), m_name(name) {}

    const Shape & get_shape() const { return m_shape; }

    void set_shape(const Shape & shape) { m_shape = shape; }

    const std::string & get_name() const { return m_name; }

    void set_name(const std::string & name) { m_name = name; }

private:
    Shape m_shape;
    std::string m_name;
};