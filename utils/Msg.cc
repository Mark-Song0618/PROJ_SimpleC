#include "Msg.hh"
#include <map>

namespace UTIL 
{

static std::map<MSGTYPE, const char*> _labelTab =
{
    {MSGTYPE::DEFUALT_ERR,          "FATAL:"},
    {MSGTYPE::FILE_EXIST,           "FATAL: FILE-IO"},
    {MSGTYPE::FILE_NOT_EXIST,       "FATAL: FILE-IO"},
    {MSGTYPE::FILE_NOT_READABLE,    "FATAL: FILE-IO"},
    {MSGTYPE::FILE_NOT_WRITABLE,    "FATAL: FILE-IO"},
    {MSGTYPE::FILE_NOT_OPEN,        "FATAL: FILE-IO"},
    {MSGTYPE::UNREC_TOKEN,          "FATAL: UNREC-TOEKN"},
    {MSGTYPE::PARSE_FAIL,           "FATAL: PARSE_FAIL"},

    {MSGTYPE::DEFUALT_WARN,         "WARN:"},


    {MSGTYPE::DEFUALT_MSG,          "MSG:"},
};

void
Msg::message(MSGTYPE type, const std::string& msg)
{
    printf("[%s]%s\n", _labelTab[type], msg.c_str());
}

}
