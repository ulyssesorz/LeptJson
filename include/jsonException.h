#pragma once

#include<stdexcept>

namespace LeptJson
{
class JsonException : public std::runtime_error
{
public:
    explicit JsonException(const std::string& errMsg) : runtime_error(errMsg) {}

public:
    const char* what() const noexcept override {return runtime_error::what();}
};
}