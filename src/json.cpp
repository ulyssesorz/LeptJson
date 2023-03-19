#include<cstdio>
#include"json.h"
#include"jsonValue.h"
#include"parse.h"

namespace LeptJson
{
//构造函数，用make_unique为jsonvalue初始化
Json::Json(std::nullptr_t) : _jsonValue(std::make_unique<JsonValue>(nullptr)){}
Json::Json(bool val) : _jsonValue(std::make_unique<JsonValue>(val)){}    
Json::Json(double val) : _jsonValue(std::make_unique<JsonValue>(val)){}
Json::Json(const std::string& val) : _jsonValue(std::make_unique<JsonValue>(val)){}
Json::Json(const _array& val) : _jsonValue(std::make_unique<JsonValue>(val)){}
Json::Json(const _object& val) : _jsonValue(std::make_unique<JsonValue>(val)){}

//移动构造
Json::Json(std::string&& val) : _jsonValue(std::make_unique<JsonValue>(std::move(val))){}
Json::Json(_array&& val) : _jsonValue(std::make_unique<JsonValue>(std::move(val))){}
Json::Json(_object&& val) : _jsonValue(std::make_unique<JsonValue>(std::move(val))){}

//析构
Json::~Json() = default;

//拷贝构造
Json::Json(const Json& rhs)
{
    switch(rhs.getType())
    {
        case JsonType::kNull : _jsonValue = std::make_unique<JsonValue>(nullptr);break;
        case JsonType::kBool : _jsonValue = std::make_unique<JsonValue>(rhs.toBool());break;
        case JsonType::kNumber : _jsonValue = std::make_unique<JsonValue>(rhs.toNumber());break;
        case JsonType::kString : _jsonValue = std::make_unique<JsonValue>(rhs.toString());break;
        case JsonType::kArray : _jsonValue = std::make_unique<JsonValue>(rhs.toArray());break;
        case JsonType::kObject : _jsonValue = std::make_unique<JsonValue>(rhs.toObject());break;
        default : break;
    }
}

//拷贝赋值，copy and swap方法
Json& Json::operator=(const Json& rhs) noexcept
{
    Json temp(rhs);
    swap(temp);
    return *this;
}

//默认移动构造
Json::Json(Json&& rhs) noexcept = default;
Json& Json::operator=(Json&& rhs) noexcept = default;

//反序列化，string->json
Json Json::parse(const std::string& content, std::string& errMsg) noexcept
{
    try
    {
        Parser p(content);
        return p.parse();
    }
    catch(JsonException& e)
    {
        errMsg = e.what();
        return Json(nullptr);
    }
}

//序列化，json->string
std::string Json::serialize() const noexcept
{
    switch(_jsonValue->getType())
    {
        case JsonType::kNull: 
            return "null";
        case JsonType::kBool: 
            return _jsonValue->toBool() ? "true" : "false";
        case JsonType::kNumber: 
            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%.17g", _jsonValue->toNumber());
            return std::string(buffer);
        case JsonType::kString:
            return serializeString();
        case JsonType::kArray:
            return serializeArray();
        default:
            return serializeObject();
    }
}

//类型获取接口
JsonType Json::getType() const noexcept
{
    return _jsonValue->getType();
}

bool Json::isNull() const noexcept
{
    return getType() == JsonType::kNull;
}
bool Json::isBool() const noexcept
{
    return getType() == JsonType::kBool;
}
bool Json::isNumber() const noexcept
{
    return getType() == JsonType::kNumber;
}
bool Json::isString() const noexcept
{
    return getType() == JsonType::kString;
}
bool Json::isArray() const noexcept
{
    return getType() == JsonType::kArray;
}
bool Json::isObject() const noexcept
{
    return getType() == JsonType::kObject;
}

//获取值的接口
bool Json::toBool() const
{
    return _jsonValue->toBool();
}
double Json::toNumber() const
{
    return _jsonValue->toNumber();
}
const std::string& Json::toString() const
{
    return _jsonValue->toString();
}
const Json::_array& Json::toArray() const
{
    return _jsonValue->toArray();
}
const Json::_object& Json::toObject() const
{
    return _jsonValue->toObject();
}

//数组和对象的[]接口
size_t Json::size() const
{
    return _jsonValue->size();
}
Json& Json::operator[](size_t pos)
{
    return _jsonValue->operator[](pos);
}
const Json& Json::operator[](size_t pos) const
{
    return _jsonValue->operator[](pos);
}
Json& Json::operator[](const std::string& key)
{
    return _jsonValue->operator[](key);
}
const Json& Json::operator[](const std::string& key) const
{
    return _jsonValue->operator[](key);
}

//用于拷贝赋值的copy and swap
void Json::swap(Json& rhs) noexcept
{
    using std::swap;
    swap(_jsonValue, rhs._jsonValue);
}

//序列化字符串，带/和“的字符需要转义
std::string Json::serializeString() const noexcept
{
    std::string res = "\"";
    for(auto e : _jsonValue->toString())
    {
        switch(e)
        {
            //需加/转义的几种字符
            case '\"' : res += "\\\"";break;
            case '\\' : res += "\\\\";break;
            case '\b' : res += "\\b";break;
            case '\f' : res += "\\f";break;
            case '\n' : res += "\\n";break;
            case '\r' : res += "\\r";break;
            case '\t' : res += "\\t";break;
            default:
                if(static_cast<unsigned char>(e) < 0x20)//前缀加u的字符
                {
                    char buffer[7];
                    sprintf(buffer, "\\u%04X", e);
                    res += buffer;
                }
                else    //普通字符
                {
                    res += e;
                }
        }
    }
    res += '"';
    return res;
}

//序列化数组，每个元素单独调用serialize()
std::string Json::serializeArray() const noexcept
{
    std::string res = "[ ";
    for(size_t i = 0; i < _jsonValue->size(); i++)
    {
        if(i > 0)
            res += " , ";
        res += (*this)[i].serialize();
    }    
    res += " ]";
    return res;
}

//序列化对象，每个value单独调用serialize()
std::string Json::serializeObject() const noexcept
{
    std::string res = "{ ";
    bool first = true;
    for(auto&& it : _jsonValue->toObject())
    {
        if(first)
            first = false;
        else
            res += " , ";
        res += "\"" + it.first + "\"";
        res += " : ";
        res += it.second.serialize();
    }
    res += " }";
    return res;
}

bool operator==(const Json& lhs, const Json& rhs)
{
    if(lhs.getType() != rhs.getType())
        return false;
    switch(lhs.getType())
    {
        case JsonType::kNull: return true;
        case JsonType::kBool: return lhs.toBool() == rhs.toBool();
        case JsonType::kNumber: return lhs.toNumber() == rhs.toNumber();
        case JsonType::kString: return lhs.toString() == rhs.toString();
        case JsonType::kArray: return lhs.toArray() == rhs.toArray();
        case JsonType::kObject: return lhs.toObject() == rhs.toObject();
        default: return false;
    }
}
}//namespace LeptJson