#pragma once

#include<memory>
#include<string>
#include<unordered_map>
#include<vector>

namespace LeptJson
{
enum class JsonType {kNull, kBool, kNumber, kString, kArray, kObject};
class JsonValue;

class Json final
{
public:
    //数组和对象类型
    using _array = std::vector<Json>;
    using _object = std::unordered_map<std::string, Json>;

public:
    //对json几种类型的构造
    Json() : Json(nullptr){}
    Json(std::nullptr_t);
    Json(bool);
    //int转double
    Json(int val) : Json(1.0 * val) {}
    Json(double);
    Json(const char* cstr) : Json(std::string(cstr)){}
    Json(const std::string&);
    Json(std::string&&);
    Json(const _array&);
    Json(_array&&);
    Json(const _object&);
    Json(_object&&);

    //防止指针意外转换
    Json(void*) = delete;

public:
    //对象的隐式构造
    template<class M, typename std::enable_if<
                std::is_constructible<
                    std::string,
                    decltype(std::declval<M>().begin()->first)>::value &&
                    std::is_constructible<
                        Json, decltype(std::declval<M>().begin()->second)>::value,
                    int>::type = 0>
    Json(const M& m) : Json(_object(m.begin(), m.end())) {}

    //数组的隐式构造
    template<class V, typename std::enable_if<
                std::is_constructible<
                    Json, decltype(*std::declval<V>().begin())>::value,
                int>::type = 0>
    Json(const V& v) : Json(_array(v.begin(), v.end())) {} 

public:
    //析构函数
    ~Json();

public:
    //拷贝和赋值构造函数
    Json(const Json&);
    Json& operator=(const Json&) noexcept;

public:
    //移动构造函数
    Json(Json&&) noexcept;
    Json& operator=(Json&&) noexcept;

public:
    //序列化和反序列化
    static Json parse(const std::string& content, std::string& errMsg) noexcept;
    std::string serialize() const noexcept;

public:
    //类型接口
    JsonType getType() const noexcept;
    bool isNull() const noexcept;
    bool isBool() const noexcept;
    bool isNumber() const noexcept;
    bool isString() const noexcept;
    bool isArray() const noexcept;
    bool isObject() const noexcept;

public:
    //把json类型转化为值
    bool toBool() const;
    double toNumber() const;
    const std::string& toString() const;
    const _array& toArray() const;
    const _object& toObject() const;

public:
    //数组和对象的接口
    size_t size() const;
    //重载[]索引数组
    Json& operator[](size_t);
    const Json& operator[](size_t) const;
    //重载[]索引对象
    Json& operator[](const std::string&);
    const Json& operator[](const std::string&) const; 

private:
    //辅助函数
    void swap(Json&) noexcept;
    std::string serializeString() const noexcept;
    std::string serializeArray() const noexcept;
    std::string serializeObject() const noexcept;

private:
    //智能指针管理json资源
    //实际数据封装在JsonValue对象里，pimpl
    std::unique_ptr<JsonValue> _jsonValue;
};

//非成员函数，重载运算符
bool operator==(const Json&, const Json&);
inline std::ostream& operator<<(std::ostream& os, const Json& json)
{
    return os << json.serialize();
}
inline bool operator!=(const Json& lhs, const Json& rhs)
{
    return !(lhs == rhs);   //利用!=实现
}
}//namespace LeptJson