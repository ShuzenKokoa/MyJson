#ifndef MYJSON_PARSER_H
#define MYJSON_PARSER_H

#include "json.h"
#include <string>




class Parser {
public:
    Parser();

    void load(const std::string &str);//加载字符串
    Json parse();//解析完后返回给Json

//------------

    ~Parser();

private:
    void skip_whitespace(); // 遇到空格 回车符 换行符 等都跳过
    char getChar(); //获取当前索引的char 然后++索引
    bool in_range(char x, size_t lower_bound, size_t upper_bound);//考虑到unicode的范围 这里比较用size_t 而不是int

    Json parse_null();

    Json parse_bool();

    Json parse_number();

    std::string parse_string();

    Json parse_array();

    Json parse_object();

    void parameter_checker();
    bool syntax_checker();

private:
    size_t m_idx = 0; //解析的时候当前的指针
    std::string *m_str = nullptr;//解析后保存的字符串
    bool isObj = false;
};

#endif //MYJSON_PARSER_H
