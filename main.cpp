#include <iostream>
#include "./Json/json.h"
#include <fstream>
#include <sstream>
int main() {
    Json j1;
    j1["aaa"] = "bbb";

    Json j2;
    j2["obj"] = j1;

    Json j3;
    j3.append("1");
    j3.append(j2);

    std::cout<<j3.str()<<'\n';
//
    Json j5;
    j5["ddd"] = "obj";

    std::cout<<j5.str()<<'\n';
//    j4.append(j5);
//
//    j3.append(j4);
//    std::cout << j3.str();
//
    std::ifstream file("./example.txt");
    std::stringstream ss;
    ss << file.rdbuf();
    const std::string& s = ss.str();
    std::cout<<"_____"<<std::endl;
//    std::cout<<ss<<'\n';
//
//    std::cout<<str<<std::endl;
//    std::cout<<"_____"<<std::endl;
//
    Json test1;
    test1.parse(s);
    std::cout<<test1.str();
    system("pause");
}
