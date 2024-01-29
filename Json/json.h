#ifndef MYJSON_JSON_H
#define MYJSON_JSON_H


#include <iostream>
#include <vector>
#include <map>


// 定义命名空间防止跟其他项目冲突
class Json {
public:
    enum Type {
        json_null = 0,
        json_bool,
        json_int,
        json_double, //使用起来double int c++更直观  实际上是没区别
        json_string,
        json_array,
        json_object
    };

    /*
     *  Json j = "1"
     *  Json j2 = 1  //implicit  隐式转换
     *
     */
    Json(); //null
    Json(int val); //Json j = 1;
    Json(bool val);//Json j = false;
    Json(double val);

    Json(const char *str);

    Json(const std::string &other);//深拷贝， 防止悬空指针
    Json(Type val);//根据类型来构造
    Json(const Json &other); // 这里采用深拷贝！！！
    Json(Json &&other);//移动构造器

    //转换为具体类型 bool b = j1;
    operator bool();

    operator int();

    operator double();

    operator std::string();

    operator std::string() const;

    bool is_null() const;
    bool is_bool() const;
    bool is_int() const;
    bool is_double() const;
    bool is_string() const;
    bool is_array() const;
    bool is_object() const;
    int size() const;
    // return true if empty array, empty object, or null, otherwise, false.
    bool empty() const;

private:
    bool has(int index) const;
    bool has(const char * key) const;
    bool has(const std::string & key) const;
public:
    Json get(int index) const;
    Json get(const char * key) const;
    Json get(const std::string & key) const;

    void remove(int index);
    void remove(const char * key);
    void remove(const std::string & key);



    // append value to array at the end.
    void append(const Json &value);

    void append(Json &&value);

    /*
       支持索引添加
       Json j;
       j["age"] = 1;
   */

    Json &operator=(const Json &other);

    Json &operator=(Json &&other);

    bool operator==(const Json &other);

    bool operator!=(const Json &other);

    //支持以数组索引的方式访问元素
    Json &operator[](int index);

    // Json j;
    //j["age"] = 1; //对象添加
    Json &operator[](const char *key);

    Json &operator[](const std::string &key);

    std::string str() const; //打印
    Type getType() const;

    void clear();

    //支持<<打印对象
    friend std::ostream &operator<<(std::ostream &os, const Json &other) {
        os << other.str();
        return os;
    }

    void parse(const std::string &str);

    typedef std::vector<Json>::iterator iterator;

    iterator begin() {
        return (m_value.m_vec)->begin();
    }

    iterator end() {
        return (m_value.m_vec)->end();
    }

    ~Json();


private:
    union Value {//空值就不放在这了
        bool m_bool;
        int m_int;
        double m_double;
        std::string *m_str;
        std::vector<Json> *m_vec; //存数组
        //存对象 键值对 {key1 : value1, key2 : value2, ... keyN : valueN }
        // "sites": [{ "name":"菜鸟教程" , "url":"www.runoob.com" }]
        std::map<std::string, Json> *m_map;
    };
    Type m_type;
    Value m_value;
private:
    void copy(const Json &other);

    void swap(Json &other);
};


#endif //MYJSON_JSON_H
