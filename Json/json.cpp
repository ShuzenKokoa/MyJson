//
// Created by admin on 5/8/2023.
//
#include "json.h"
#include "Parser.h"
#include <cassert>
#include <strstream>
#include <sstream>

//深度拷贝
Json::Json() : m_type(json_null){}//nullptr
//根据类型构造
Json::Json( Type  val):m_type(val) {
    switch (m_type) {
        case json_null:{
            break;
        }
        case json_bool:{
            m_value.m_bool = false;
            break;
        }
        case json_int:{
            m_value.m_int = 0;
            break;
        }
        case json_double:{
            m_value.m_double = 0.0;
            break;
        }
        case json_string:{
            m_value.m_str = new std::string("");
            break;
        }
        case json_array:{
            m_value.m_vec = new std::vector<Json>();
            break;
        }
        case json_object:{
            m_value.m_map = new std::map<std::string,Json>();
            break;
        }
        default:
            throw std::logic_error("Json(const Type&) error");
    }
}

void Json::copy(const Json &other) {// 这样传递就表示这个值是存在的
    clear();
    m_type = other.m_type;
    switch (m_type) {
        //非指针成员不用操作 因为默认就是浅拷贝
        case json_null:
        case json_bool:
        case json_int:
        case json_double:
            m_value = other.m_value;
            break;

        case json_string:{
            if(other.m_value.m_str != nullptr){
                m_value.m_str = new std::string(*(other.m_value.m_str));
            }
            //如果没有就不用把m_value.m_str置为空 因为之前已经clear过了
            break;
        }
        case json_array :{
            if(other.m_value.m_vec != nullptr){
                m_value.m_vec = new std::vector<Json>();
                auto i = other.m_value.m_vec->begin();
                auto j = other.m_value.m_vec->end();
                for(;i!=j;++i){
                    m_value.m_vec->push_back(*(i));
                }
            }
            break;
        }
        case json_object:{
            if(other.m_value.m_map != nullptr){
                m_value.m_map = new std::map<std::string,Json>();
                auto i = other.m_value.m_map->begin();
                auto j = other.m_value.m_map->end();
                for(;i!=j;++i){
                    (*(m_value.m_map))[i->first] = i->second;
                }
            }
            break;
        }
    }
}

Json::Json(const Json &other) :m_type(json_null){ //先清空自己本身的东西
    copy(other); //里面有clear了
}

void Json::swap( Json& other){
    Json::Type type = m_type;
    Json::Value value = m_value;
    m_type = other.m_type;
    m_value = other.m_value;
    other.m_type = type; //交换生命周期，把自己的换过去 然后结束.
    other.m_value = value;
}

Json::Json( Json&& other){
    //移动构造
    swap(other);
}

    /*
             *  Json j = "1"
             *  Json j2 = 1  //implicit  隐式转换
             *
             */


Json::Json(int val) : m_type(json_int) {
    m_value.m_int = val;
}

Json::Json(bool val) : m_type(json_bool) {
    m_value.m_bool = val;
}

Json::Json(double val) : m_type(json_double){
    m_value.m_double = val;
}

Json::Json(const char *str) : m_type(json_string){

    m_value.m_str = new std::string(str);
}


Json::Json(const std::string &other) : m_type(json_string){
    m_value.m_str = new std::string (other);
}

/*---------------提取值到常规类型
 * bool b = j1;
 * int a = j2;
 *
 */

Json::operator bool() {
    if(m_type  != json_bool){
        throw std::logic_error("operator bool() type must be bool");
    }
    return m_value.m_bool;
}

Json::operator int(){
    if(m_type  != json_bool){
        throw std::logic_error("operator int() type must be int");
    }
    return m_value.m_int;
}
Json::operator double(){
    if(m_type  != json_double){
        throw std::logic_error("operator double() type must be double");
    }
    return m_value.m_double;
}
Json::operator std::string(){
    if(m_type != json_string){
        throw std::logic_error("operator string() type must be string");
    }
    return *(m_value.m_str); //容易造成悬垂指针
}
Json::operator std::string() const{
    if(m_type != json_string){
        throw std::logic_error("operator string() type must be string");
    }
    return *(m_value.m_str);
}

bool Json::is_null() const{
    return m_type == json_null;
}
bool Json::is_bool() const{
    return m_type == json_bool;
}
bool Json::is_int() const{
    return m_type == json_int;
}
bool Json::is_double() const{
    return m_type == json_double;
}
bool Json::is_string() const{
    return m_type == json_string;
}
bool Json::is_array() const{
    return m_type == json_array;
}
bool Json::is_object() const{
    return m_type == json_object;
}


int Json::size() const{
    if(m_type == json_array) return m_value.m_vec->size();
    if(m_type == json_object){
        return m_value.m_map->size();
    }
    else{
        return -1;
    }
}
// return true if empty array, empty object, or null, otherwise, false.
bool Json::empty() const{

    if(m_type == json_array){
        if(m_value.m_vec == nullptr){
            return true;
        }else{
            return m_value.m_vec->empty();
        }
    }
    if(m_type == json_object){
        if(m_value.m_map == nullptr){
            return true;
        }else{
            return m_value.m_map->empty();
        }
    }
    return true;
}

bool Json::has(int index) const{

    if(m_type == json_array){
        if(index > m_value.m_vec->size()){
            std::cout<<"index out of bound"<<std::endl;
            return false;
        }
        return index >= 0;
    }
    return false;
}
bool Json::has(const char * key) const{
    if(!key) return false;
    return has(std::string(key));
}
bool Json::has(const std::string & key) const{
    if(m_type == json_object){
        if(key.empty()) return false;
        return m_value.m_map->find(key) != m_value.m_map->end();
    }
    return false;
}

Json Json::get(int index) const{
    if(has(index)){
        return Json();//返回空
    }
    return (*(m_value.m_vec))[index];
}
Json Json::get(const char * key) const{
    if(key == nullptr || !has(key)){
        std::cout<<"no key"<<std::endl;
        return Json();
    }
    return get(std::string(key));
}
Json Json::get(const std::string & key) const{
    if(!has(key)){
        return Json();
    }
    return (*m_value.m_map)[key];
}

void Json::remove(int index){
    if(!has(index)) return ;

    auto i = (*m_value.m_vec);
    i[index].clear();
    i.erase(i.begin()+index);
}
void Json::remove(const char * key){
    if(!has(key)) return;
    remove(std::string(key));
}
void Json::remove(const std::string & key){
    if(!has(key) || m_type != json_object){
        return ;
    }
    auto i = (*m_value.m_map).find(key);
    if(i != (*m_value.m_map).end()){
        i->second.clear();
        (m_value.m_map)->erase(i);
    }
}


void Json::clear() {
    switch (m_type) {
        //其他类型就不管了 主要是分配内存的类型
        case json_string: {
            delete m_value.m_str;
            m_value.m_str = nullptr;
            break;
        }
        case json_array:{
            if(m_value.m_vec != nullptr){
                auto i = begin();
                auto j = end();
                for(;i!=j;++i){
                    (*i).clear();//递归调用删除
                }
                delete m_value.m_vec;
                m_value.m_vec = nullptr;
            }
            break;
        }
        case json_object:{ //map
            auto i = m_value.m_map->begin();//map 自动排序
            auto j = m_value.m_map->end();
            for(;i!=j;++i){
                (*i).second.clear();
            }
            delete m_value.m_map;
            m_value.m_map = nullptr;
            break;
        }
        default:
            break;
    }
    m_type = json_null;
}
void Json::append(const Json &value) { //注意拷贝构造

    if(m_type != json_array) {
        ////在添加之前要把本身的值都清空 要不然内存泄露.
        //因为你本身不是数组 所以自身可能开辟了空间  然后再重新开辟空间.
        clear();
        m_type = json_array;
        m_value.m_vec = new std::vector<Json>();
    }
    (m_value.m_vec)->push_back(value);
}

void Json::append(Json &&value) {
    if(m_type != json_array){
        clear();
        std::cout<<"append(Json &&value)" << std::endl;
        m_type = json_array;
        m_value.m_vec = new std::vector<Json>();
    }
    (m_value.m_vec)->push_back(std::move(value));// 把右值转左值
}

std::string Json::str() const {
    std::stringstream ss;
    switch (m_type) {
        case json_null:{
            ss<<"null";
            break;
        }
        case json_int:{

            ss<<m_value.m_int;
            break;
        }
        case json_bool:{
            ss<<((m_value.m_bool) ? ("true") : ("false"));
            break;
        }
        case json_double:{
            ss<<m_value.m_double;
            break;
        }
        case json_string:{
            ss<<"\""<<(*(m_value.m_str))<<"\"";
            break;
        }
        case json_array:{
            auto i = m_value.m_vec->begin();
            auto j = m_value.m_vec->end();
            ss<<"[";

            for(;i!=j;++i){
                if(i!=m_value.m_vec->begin()){
                    ss<<",";
                }
                ss<<((*i).str());
            }
            ss<<"]";
        }
            break;
        case json_object:{
            auto i =m_value.m_map->begin();
            auto j = m_value.m_map->end();

            //因为value是 Json类型 所以让value去递归就行
            ss<<"{";
            for(;i!=j;++i){
                if(i != m_value.m_map->begin()) ss<<",";
                ss<<"\""<<(i->first) <<"\" : "<< ((*i).second.str());
            }
            ss<<"}";
        }
            break;
        default:
            break;
    }

    return ss.str();
}


Json & Json::operator = (const Json & other){
    clear();
    copy(other);
    return *this;
}
Json & Json::operator = (Json && other) {
    swap(other);
    return *this;
}
bool Json::operator != (const Json & other){
    if(m_type != other.getType()) return true;


    switch (other.m_type) {
        case json_bool:{
            return m_value.m_bool != other.m_value.m_bool;
        }
        case json_string:{
            return *(m_value.m_str) != *(other.m_value.m_str);
        }
        case json_int :{
            return m_value.m_int != other.m_value.m_int;
        }
        case json_double:{
            return m_value.m_double != other.m_value.m_double;
        }
        case json_array:{
            size_t m = m_value.m_vec->size();
            size_t n = other.m_value.m_vec->size();
            if(m!=n) return true;
            for(int i =0;i<m;++i){
                //只要发现一个不相等 直接认为是不相等
                if((((*m_value.m_vec))[i] != (*(other.m_value.m_vec))[i])){//递归重载
                    return true;
                }
            }
            return false;
        }
        case json_object:{
            size_t m = m_value.m_map->size();
            size_t n = other.m_value.m_map->size();
            if(m!=n) return true;

            auto i = m_value.m_map->begin();
            auto j = m_value.m_map->end();

            auto k = other.m_value.m_map->begin();
            auto l = other.m_value.m_map->end();
            for(;i!=j && k!=l;++i,++k){
                if(i->first != k->first) return true;
                else{
                    if((*(m_value.m_map))[i->first] != (*(other.m_value.m_map))[k->first]){
                        return true;
                    }
                }
            }
            return false;
        }
        default:
            break;
    }
    return false;//剩余null
}
bool Json::operator == (const Json & other){

    if(m_type != other.getType()) return false;

    switch (other.m_type) {
        case json_bool:{
            return m_value.m_bool == other.m_value.m_bool;
        }
        case json_string:{
            return *(m_value.m_str) == *(other.m_value.m_str);
        }
        case json_int :{
            return m_value.m_int == other.m_value.m_int;
        }
        case json_double:{
            return m_value.m_double == other.m_value.m_double;
        }
        case json_array:{
            size_t m = m_value.m_vec->size();
            size_t n = other.m_value.m_vec->size();
            if(m!=n) return false;

            for(int i =0;i<m;++i){
                if((((*m_value.m_vec))[i] != (*(other.m_value.m_vec))[i])){//递归重载
                    return false;
                }
            }
            return true;
        }
        case json_object:{
            size_t m = m_value.m_map->size();
            size_t n = other.m_value.m_map->size();
            if(m!=n) return false;

            auto i = m_value.m_map->begin();
            auto j = m_value.m_map->end();

            auto k = other.m_value.m_map->begin();
            auto l = other.m_value.m_map->end();
            for(;i!=j && k!=l;++i,++k){
                if(i->first != k->first) return false;
                else{
                    if((*(m_value.m_map))[i->first] != (*(other.m_value.m_map))[k->first]){
                        return false;
                    }
                }
            }
            return true;
        }
        default:
            break;
    }
    return true;
}

//支持以数组索引的方式访问元素
Json & Json::operator [] (int index){
    assert(index>=0);

    if(m_type != json_array){
        throw std::logic_error(" operator[] error ");
    }
    return (*(m_value.m_vec))[index];
}
Json & Json::operator [] (const std::string & key){

    if(m_type != json_object){
        clear();
        m_type = json_object;
        m_value.m_map = new std::map<std::string,Json>();
    }

    return (*m_value.m_map)[key];
}

Json &Json::operator[](const char *key) {

    std::string s(key);
    return (*this)[s];
}



Json::Type Json::getType() const {
    return m_type;
}


void Json::parse(const std::string &str) {
    Parser p;
    p.load(str);
    try{
        *this = p.parse();
    }catch (std::invalid_argument& ia){
        std::cerr<<ia.what()<<std::endl;
        return ;
    }
}



Json::~Json() {
    clear();
}
