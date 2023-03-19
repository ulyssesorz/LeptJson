#pragma once

#include<variant>
#include"json.h"
#include"jsonException.h"

namespace LeptJson
{
class JsonValue
{
public:
    //构造函数
    explicit JsonValue(std::nullptr_t) : _val(nullptr){}
    explicit JsonValue(bool val) : _val(val){}
    explicit JsonValue(double val) : _val(val){}
    explicit JsonValue(const std::string& val) : _val(val){}
    explicit JsonValue(const Json::_array& val) : _val(val){}
    explicit JsonValue(const Json::_object& val) : _val(val){}

public:
    //移动构造
    explicit JsonValue(const std::string&& val) : _val(std::move(val)){}
    explicit JsonValue(const Json::_array&& val) : _val(std::move(val)){}
    explicit JsonValue(const Json::_object&& val) : _val(std::move(val)){}

public:
    //析构函数
    ~JsonValue() = default;

public:
    JsonType getType() const noexcept;

public:
    //把json类型转化为真实值
    std::nullptr_t toNull() const;
    bool toBool() const;
    double toNumber() const;
    const std::string& toString() const;
    const Json::_array& toArray() const;
    const Json::_object& toObject() const;

public:
    //数组和对象随机存取
    size_t size() const;
    Json& operator[](size_t);
    const Json& operator[](size_t) const;
    Json& operator[](const std::string&);
    const Json& operator[](const std::string&) const; 

private:
    std::variant<std::nullptr_t, bool, double, std::string, Json::_array, Json::_object> _val;
};
}//namespace LeptJson