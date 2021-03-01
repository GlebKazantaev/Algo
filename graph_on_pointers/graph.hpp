//
// Created by Gleb Dmitrievich on 1/12/21.
//

#pragma once

#include <memory>
#include <vector>
#include <string>

class Result;
class Parameter;

using ResultVector = std::vector<std::shared_ptr<Result>>;
using ParameterVector = std::vector<std::shared_ptr<Parameter>>;

class Function {
public:
    explicit Function(ResultVector results, ParameterVector parameters)
        : m_results(std::move(results)), m_parameters(std::move(parameters)) {}

    const std::vector<std::shared_ptr<Result>> & get_results() { return m_results; }

    const std::vector<std::shared_ptr<Parameter>> & get_parameters() { return m_parameters; }

    std::shared_ptr<Result> get_result(size_t index) const
    {
        if (index >= m_results.size())
        {
            throw std::runtime_error("Index out of range");
        }
        return m_results.at(index);
    }
private:
    ResultVector m_results;
    ParameterVector m_parameters;
};
