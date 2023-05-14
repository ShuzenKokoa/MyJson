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


    Json j5;
    j5["ddd"] = "obj";
    Json j4;
    j4.append("arr");
    j4.append(j5);

    j3.append(j4);
    std::cout << j3.str();

    std::ifstream file("example.txt");
    std::ostringstream oss;
    oss << file.rdbuf();
    std::string str = oss.str();
    std::cout<<"_____"<<std::endl;

    std::cout<<str<<std::endl;
    std::cout<<"_____"<<std::endl;

    Json test1;
    test1.parse(str);
    std::cout<<test1.str();

}
