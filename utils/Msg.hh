#pragma once

#include <stdio.h>
#include <string>

namespace UTIL
{

enum class MSGTYPE
{
    DEFUALT_ERR,
    FILE_EXIST,
    FILE_NOT_EXIST,
    FILE_NOT_READABLE,
    FILE_NOT_WRITABLE,
    FILE_NOT_OPEN,
    UNREC_TOKEN,
    PARSE_FAIL, 

    DEFUALT_WARN,
    DEFUALT_MSG,
};

class Msg {
public:
    static Msg& instance() {static Msg msg; return msg; }
    void message(MSGTYPE, const std::string&);
private:
    Msg() {}
private:
    FILE* _output;
};

}
