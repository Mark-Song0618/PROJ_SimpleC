#pragma once

#include <string>

namespace UTIL {

class MyException
{
public:
    MyException(const char* msg): _msg(msg) {}

    const std::string& what() { return _msg; }

private:
    std::string _msg;
};

}
