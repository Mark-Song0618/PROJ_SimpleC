#pragma once

#include <stdio.h>
#include <string>

namespace UTIL
{

enum class ERRTYPE
{
    FILE_EXIST,
    FILE_NOT_EXIST,
    FILE_NOT_READABLE,
    FILE_NOT_WRITABLE,
    FILE_NOT_OPEN,

    UNREC_TOKEN,
};

class Msg {
public:
    static Msg& instance() {static Msg msg; return msg; }
    void error(ERRTYPE, const std::string&);
    void warn(ERRTYPE, const std::string&);
    void message(ERRTYPE, const std::string&);
private:
    Msg() {}
private:
    FILE* _output;
};

}
