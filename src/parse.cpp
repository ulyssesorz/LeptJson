#include<cassert>
#include<cmath>
#include<cstdio>
#include<cstring>
#include<stdexcept>
#include"parse.h"

namespace LeptJson
{
//去除空白字符
void Parser::parseWhitespace() noexcept
{
    while(*_curr == ' ' || *_curr == '\t' || *_curr == '\r' || *_curr == '\n')
    {
        _curr++;
    }
    _start = _curr;
}

//把四组4位十六进制数转换为二进制
unsigned Parser::parse4hex()
{
    unsigned u = 0;
    for(size_t i = 0; i < 4; i++)
    {
        auto ch = static_cast<unsigned>(toupper(*++_curr));
        u <<= 4;
        if(ch >= '0' && ch <= '9')
            u |= (ch - '0');
        else if(ch >= 'A' && ch <= 'F')
            u |= (ch - 'A' + 10);
        else
            error("INVALID UNICODE HEX");
    }
    return u;
}

//utf8编码
std::string Parser::encodeUTF8(unsigned u) noexcept
{
    std::string utf8;
    if(u <= 0x7F)
    {
        utf8.push_back(static_cast<char>(u & 0xFF));
    }
    else if(u <= 0x7FF)
    {
        utf8.push_back(static_cast<char>(0xC0 | ((u >> 6) & 0xFF)));
        utf8.push_back(static_cast<char>(0x80 | (u & 0x3F)));
    }
    else if(u <= 0xFFFF)
    {
        utf8.push_back(static_cast<char>(0xE0 | ((u >> 12) & 0xFF)));
        utf8.push_back(static_cast<char>(0x80 | ((u >> 6) & 0x3F)));
        utf8.push_back(static_cast<char>(0x80 | (u & 0x3F)));
    }
    else
    {
        assert(u <= 0x10FFFF);
        utf8.push_back(static_cast<char>(0xF0 | ((u >> 18) & 0xFF)));
        utf8.push_back(static_cast<char>(0x80 | ((u >> 12) & 0x3F)));
        utf8.push_back(static_cast<char>(0x80 | ((u >> 6) & 0x3F)));
        utf8.push_back(static_cast<char>(0x80 | (u & 0x3F)));
    }
    return utf8;
}

std::string Parser::parseRawString()
{
    std::string str;
    while(1)
    {
        switch(*++_curr)
        {
            case '\"':
                _start = ++_curr;
                return str;
            case '\0':
                error("MISS QUOTATION MARK");
            case '\\':
                switch(*++_curr)
                {
                    case '\"': str.push_back('\"');break;
                    case '\\': str.push_back('\\');break;
                    case '/': str.push_back('/');break;
                    case 'b': str.push_back('\b');break;
                    case 'n': str.push_back('\n');break;
                    case 'f': str.push_back('\f');break;
                    case 't': str.push_back('\t');break;
                    case 'r': str.push_back('\r');break;
                    case 'u':
                    {
                        unsigned u1 = parse4hex();
                        if(u1 >= 0xD800 && u1 <= 0xDBFF)
                        {
                            if(*++_curr != '\\')
                                error("INVALID UNICODE SURROGATE");
                            if(*++_curr != 'u')
                                error("INVALID UNICODE SURROGATE");
                            unsigned u2 = parse4hex();
                            if(u2 < 0xDC00 || u2 > 0xDFFF)
                                error("INVALID UNICODE SURROGATE");
                            u1 = (((u1 - 0xD800) << 10) | (u2 - 0xDC00)) + 0x10000;
                        }
                        str += encodeUTF8(u1);
                    }break;
                    default: error("INVALID STRING ESCAPE");
                }
                break;
            default:
                if(static_cast<unsigned char>(*_curr) < 0x20)
                    error("INVALID STRING CHAR");
                str.push_back(*_curr);
                break;
        }
    }
}

void Parser::error(const std::string& msg) const
{
    throw JsonException(msg + ": " + _start);
}

Json Parser::parseValue()
{
    switch(*_curr)
    {
        case 'n':  return parseLiteral("null");
        case 't':  return parseLiteral("true");
        case 'f':  return parseLiteral("false");
        case '\"': return parseString();
        case '[':  return parseArray();
        case '{':  return parseObject();
        case '\0': error("EXPECT VALUE");
        default:   return parseNumber();
    }
}

Json Parser::parseLiteral(const std::string& literal)
{
    if(strncmp(_curr, literal.c_str(), literal.size()) != 0)
        error("INVALID VALUE");
    _curr += literal.size();
    _start = _curr;
    switch(literal[0])
    {
        case 't': return Json(true);
        case 'f': return Json(false);
        default:  return Json(nullptr);
    }
}

Json Parser::parseNumber()
{
    if(*_curr == '-')
        ++_curr;
    if(*_curr == '0')
    {
        ++_curr;
    }
    else
    {
        if(!is1to9(*_curr))
            error("INVALID VALUE");
        while(is0to9(*++_curr))
            ;
    }
    if(*_curr == '.')
    {
        if(!is0to9(*++_curr))
            error("INVALID VALUE");
        while(is0to9(*++_curr))
            ; 
    }
    if(toupper(*_curr) == 'E')
    {
        ++_curr;
        if(*_curr == '+' || *_curr == '-')
            ++_curr;
        if(!is0to9(*_curr))
            error("INVALID VALUE");
        while(is0to9(*++_curr))
            ; 
    }
    double n = strtod(_start, nullptr);
    if(fabs(n) == HUGE_VAL)
        error("NUMBER TOO BIG");
    _start = _curr;
    return Json(n);
}

Json Parser::parseString()
{
    return Json(parseRawString());
}

Json Parser::parseArray()
{
    Json::_array arr;
    ++_curr;
    parseWhitespace();
    if(*_curr == ']')
    {
        _start = ++_curr;
        return Json(arr);
    }
    while(1)
    {
        parseWhitespace();
        arr.push_back(parseValue());
        parseWhitespace();
        if(*_curr == ',')
        {
            ++_curr;
        }
        else if(*_curr == ']')
        {
            _start = ++_curr;
            return Json(arr);
        }
        else
        {
            error("MISS COMMA OR SQUARE BRACKET");
        }
    }
}

Json Parser::parseObject()
{
    Json::_object obj;
    ++_curr;
    parseWhitespace();
    if(*_curr == '}')
    {
        _start = ++_curr;
        return Json(obj);
    }
    while(1)
    {
        parseWhitespace();
        if(*_curr != '"')
            error("MISS KEY");
        std::string key = parseRawString();
        parseWhitespace();
        if(*_curr++ != ':')
            error("MISS COLON");
        parseWhitespace();
        Json val = parseValue();
        obj.insert({key, val});
        parseWhitespace();
        if(*_curr == ',')
        {
            ++_curr;
        }
        else if(*_curr == '}')
        {
            _start = ++_curr;
            return Json(obj);
        }
        else
        {
            error("MISS COMMA OR CURLY BRACKET");
        }
    }   
}

Json Parser::parse()
{
    parseWhitespace();
    Json json = parseValue();
    parseWhitespace();
    if(*_curr)
        error("ROOT NOT SINGULAR");
    return json;
}
}//namespace LeptJson