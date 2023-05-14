//
// Created by admin on 5/11/2023.
//


#include "Parser.h"
void Parser::parameter_checker() {
    if (m_str == nullptr) {
        std::cout << __LINE__ << std::endl;
        throw std::invalid_argument("m_str must not be null");
    }
    if (m_idx >= m_str->size()) {
        std::cout << __LINE__ << std::endl;
        throw std::out_of_range("字符串可能为空 或索引超过字符串范围");
    }
}

Parser::Parser() : m_idx(0) {}

void Parser::load(const std::string &str) {
    auto tmp = m_str;
    delete tmp;//小心内存泄露
    try {
        m_str = new std::string(str);
    } catch (std::bad_alloc &bad) {
        std::cerr << bad.what();
        throw bad;
    }
}

void Parser::skip_whitespace() {

    try {
        parameter_checker();
    } catch (std::invalid_argument &ia) {
        std::cerr << ia.what();
        throw ia;
    } catch (std::out_of_range &oor) {
        std::cerr << oor.what();
        throw oor;
    }


    std::string &str = (*(m_str));

    while (str[m_idx] == ' ' || str[m_idx] == '\r' || str[m_idx] == '\n'
           || str[m_idx] == '\t' && m_str->size())
        ++m_idx;
}

char Parser::getChar() {
    skip_whitespace();
//    try {
//        parameter_checker();
//    } catch (std::invalid_argument &ia) {
//        std::cerr << ia.what();
//        throw ia;
//    } catch (std::out_of_range &oor) {
//        std::cerr << oor.what();
//        throw oor;
//    }//尽管skip 里检查 但是这里还要检查 因为里面移动了指针
    if (m_idx == m_str->size()) throw std::logic_error("getchar() m_idx out of bound error");
    return (*(m_str))[m_idx++];
}

Json Parser::parse() {
    char c = getChar();
    //这里不在外面--m_idx统一操作是因为万一是异常呢 ,
    switch (c) {
        //case 'N':
        case 'n': {
            if(!isObj){
                std::cout<<"json语法有误"<<std::endl;
                throw std::invalid_argument("json syntax error");
            }
            --m_idx;//从头解析,因为解析 利用到getChar 左闭右开
            return parse_null();
        }
        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': {
            if(!isObj){
                std::cout<<"json语法有误"<<std::endl;
                throw std::invalid_argument("json syntax error");
            }
            --m_idx;
            return parse_number();
        }
        case 't':
        case 'f': {
            if(!isObj){
                std::cout<<"json语法有误"<<std::endl;
                throw std::invalid_argument("json syntax error");
            }
            --m_idx;
            return parse_bool();
        }
        case '"': {
            if(!isObj){
                std::cout<<"json语法有误"<<std::endl;
                throw std::invalid_argument("json syntax error");
            }
            //因为 "在 json构造里面添加了 所以跳过自己的["] 递归 是否内存泄露?
            return Json(parse_string());
        }
        case '[': {
            if(!isObj){
                std::cout<<"json语法有误"<<std::endl;
                throw std::invalid_argument("json syntax error");
            }
            return parse_array();
        }
        case '{': {
            isObj = true;
            return parse_object();
        }
        default:
            std::cout << "illegal char  , index =" << m_idx;
            throw std::logic_error("syntex error ,parse()");
    }
}

bool Parser::in_range(char x, size_t lower_bound, size_t upper_bound) {
    return (x >= lower_bound && x <= upper_bound);
}
Json Parser::parse_null() {
    try {
        parameter_checker();
    } catch (std::invalid_argument &ia) {
        std::cerr << ia.what();
        throw ia;
    } catch (std::out_of_range &oa) {
        std::cerr << oa.what();
        throw oa;
    }
    std::string &str = (*m_str);
    if (str.compare(m_idx, 4, "null") == 0) {
        m_idx += 4;
        return Json(); //拷贝的时候 指针问题?
    }
    if(str.compare(m_idx,4,"Null") == 0){
        m_idx+=4;
        return Json();
    }
    throw std::logic_error("json null error");
}

Json Parser::parse_bool() {
    try {
        parameter_checker();
    } catch (std::invalid_argument &ia) {
        std::cerr << ia.what();
        throw ia;
    } catch (std::out_of_range &oor) {
        std::cerr << oor.what();
        throw oor;
    }

    if (m_str->compare(m_idx, 4, "true") == 0) {
        m_idx += 4;
        return Json(true);
    }

    if (m_str->compare(m_idx, 5, "false") == 0) {
        m_idx += 5;
        return Json(false);
    }
    throw std::invalid_argument("invalid arguments parse bool error");
}

Json Parser::parse_number() {
    try {
        parameter_checker();
    } catch (std::invalid_argument &ia) {
        std::cerr << ia.what();
        throw ia;
    } catch (std::out_of_range &oor) {
        std::cerr << oor.what();
        throw oor;
    }

    //解析 负数 整数 正负小数
    size_t oldidx = m_idx;

    /*
        前面两个处理这种情况
        -0.1
        0111
        1.xx
    */
    std::string &str = *(m_str);
    if (str[m_idx] == '-') ++m_idx;
    if (str[m_idx] == '0') { ++m_idx; } //处理-0.xxx
        //如果两个都不是 那就是integer 或 111.111
    else if (in_range(str[m_idx], '1', '9')) {
        ++m_idx;
        while (in_range(str[m_idx], '0', '9')) {
            ++m_idx;
        }
    } else {
        throw std::invalid_argument("parser number error invalid argument");
    }
    if (str[m_idx] != '.') {
        //atoi转换为数字 一旦遇到转换不了的就结束 转不了小数
        //c_str() 转换为c风格的char*
        return Json(std::atoi(str.c_str() + oldidx));
    }
    //构造小数点后面的整数
    ++m_idx;//越过.
    if (!in_range(str[m_idx], '0', '9')) {
        throw std::invalid_argument("parser number error invalid argument");
    }
    while (in_range(str[m_idx], '0', '9')) { ++m_idx; }
    return Json(atof(str.c_str() + oldidx));
}

std::string Parser::parse_string() {
    try {
        parameter_checker();
    } catch (std::invalid_argument &ia) {
        std::cerr << ia.what();
        throw ia;
    } catch (std::out_of_range &oor) {
        std::cerr << oor.what();
        throw oor;
    }

    size_t oldidx = m_idx;
    std::string &str = *(m_str);
    while (true) {
//        if(m_idx == str.size())
        char c = str[m_idx++];
        if (c == '"') break;

        if (c == '\\') {//可能遇到转义字符
            c = str[m_idx++];//走下一步 看看这个转移字符是什么
            switch (c) {
                case 'b':
                case 't':
                case 'n':
                case 'f':
                case 'r':
                case '"':
                case '\\':
                    break;
                case 'u': {//unicode linux 4 字节
                    m_idx += 4;
                    break;
                }
                default:
                    break;
            }
        }
    }
    return str.substr(oldidx, m_idx - oldidx - 1);//遇到非法字符了 -1就是减去非法字符的
}

Json Parser::parse_array() {
//    try {
//        parameter_checker();
//    } catch (std::invalid_argument &ia) {
//        std::cerr << ia.what();
//        throw ia;
//    } catch (std::out_of_range &oor) {
//        std::cerr << oor.what();
//        throw oor;
//    }

    //数组 递归结构 递归构造.
    Json arr(Json::Type::json_array);
    char c = getChar();
    if (c == ']') {
        return arr;//返回构造空数组[]
    }
    //说明探测到逗号 有下一个元素 递归去处理
    --m_idx;
    while (true) {
        arr.append(parse());//下一个元素是什么  去递归解析探测;
        c = getChar();//递归处理完  走下一步
        if (c == ']') break;

        if (c != ',') throw std::logic_error("expected , array");
        //skip_whitespace();
    }
    return arr;
}

Json Parser::parse_object() {

    Json obj(Json::json_object);
    char c = getChar();
    if (c == '}') return obj;//空对象

    //探测到 有key
    --m_idx;
    while (true) {
        c = getChar();
        if (c != '"') { //若不是key的格式 "key" : value
            throw std::invalid_argument("json syntax key error ");
        }
        std::string key = parse_string();
        c = getChar();
        if (c != ':') throw std::invalid_argument("json syntax error , \"key\" : value ");
        obj[key] = parse();//递归去解析
        c = getChar();
        if (c == '}') break;//如果下一个元素是} 那就结束

        if (c != ',') { //既然没结束 还不是, 那就是语法错误
            throw std::invalid_argument("json object syntax error");
        }
    }
    return obj;
}

Parser::~Parser() {
    //if(m_str != nullptr) //deleting null pointer has no effect
    delete m_str;
    m_str = nullptr;
}


