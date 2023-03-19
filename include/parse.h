#pragma once

#include"json.h"
#include"jsonException.h"

namespace LeptJson
{
constexpr bool is1to9(char ch) {return ch >= '1' && ch <= '9';}
constexpr bool is0to9(char ch) {return ch >= '0' && ch <= '9';}

class Parser
{
public:
    //构造函数
    explicit Parser(const char* cstr) noexcept : _start(cstr), _curr(cstr){}
    explicit Parser(const std::string& content) noexcept : _start(content.c_str()), _curr(content.c_str()) {}

public:
    //禁用拷贝，只能有一个解析器
    Parser(const Parser&) = delete;
    Parser& operator=(const Parser&) = delete;

private:
    //辅助函数
    void parseWhitespace() noexcept;
    unsigned parse4hex();
    std::string encodeUTF8(unsigned u) noexcept;
    std::string parseRawString();
    void error(const std::string& msg) const;

private:
    //解析不同类型的值
    Json parseValue();
    Json parseLiteral(const std::string& literal);
    Json parseNumber();
    Json parseString();
    Json parseArray();
    Json parseObject();

public:
    //唯一的调用接口
    Json parse();

private:
    const char* _start; //开始解析的位置
    const char* _curr;  //当前的解析位置
};
}//namespace LeptJson