#include"jsonValue.h"
#include"jsonException.h"

namespace LeptJson
{
//获取类型，利用variant的holds_alternative获取类型，返回相应的type
JsonType JsonValue::getType() const noexcept
{
    if(std::holds_alternative<std::nullptr_t>(_val))
        return JsonType::kNull;
    else if(std::holds_alternative<bool>(_val))
        return JsonType::kBool;
    else if(std::holds_alternative<double>(_val))
        return JsonType::kNumber;
    else if(std::holds_alternative<std::string>(_val))
        return JsonType::kString;
    else if(std::holds_alternative<Json::_array>(_val))
        return JsonType::kArray;
    else
        return JsonType::kObject;
}

//对于数组或对象返回其大小，即vec或map的size
size_t JsonValue::size() const
{
    if(std::holds_alternative<Json::_array>(_val))
        return std::get<Json::_array>(_val).size();
    else if(std::holds_alternative<Json::_object>(_val))
        return std::get<Json::_object>(_val).size();
    else
        throw JsonException("not a array or object");
}

//重载数组的[]，vec已自带[]索引元素
const Json& JsonValue::operator[](size_t pos) const
{
    if(std::holds_alternative<Json::_array>(_val))
        return std::get<Json::_array>(_val)[pos];
    else 
        throw JsonException("not a array");
}

//先转换成const类型去调用const版本的[]，然后去除结果的const
Json& JsonValue::operator[](size_t pos)
{
    return const_cast<Json&>(static_cast<const JsonValue&>(*this)[pos]);
}

const Json& JsonValue::operator[](const std::string& key) const
{
    if(std::holds_alternative<Json::_object>(_val))
        return std::get<Json::_object>(_val).at(key);
    else
        throw JsonException("not a object");
}

Json& JsonValue::operator[](const std::string& key)
{
    return const_cast<Json&>(static_cast<const JsonValue&>(*this)[key]);
}

//提取null
std::nullptr_t JsonValue::toNull() const
{
    try
    {
        return std::get<std::nullptr_t>(_val);
    }
    catch(const std::bad_variant_access&)
    {
        throw JsonException("not a null");
    }
}

bool JsonValue::toBool() const
{
    try
    {
        return std::get<bool>(_val);
    }
    catch(const std::bad_variant_access&)
    {
        throw JsonException("not a bool");
    }
}

double JsonValue::toNumber() const
{
    try
    {
        return std::get<double>(_val);
    }
    catch(const std::bad_variant_access&)
    {
        throw JsonException("not a number");
    }
}

const std::string& JsonValue::toString() const
{
    try
    {
        return std::get<std::string>(_val);
    }
    catch(const std::bad_variant_access&)
    {
        throw JsonException("not a string");
    }
}

const Json::_array& JsonValue::toArray() const
{
    try
    {
        return std::get<Json::_array>(_val);
    }
    catch(const std::bad_variant_access&)
    {
        throw JsonException("not a array");
    }
}

const Json::_object& JsonValue::toObject() const
{
    try
    {
        return std::get<Json::_object>(_val);
    }
    catch(const std::bad_variant_access&)
    {
        throw JsonException("not a object");
    }
}
}//namespace LeptJson