#include "Msg.hh"
#include <map>

namespace UTIL 
{

static std::map<ERRTYPE, const char*> _labelTab =
{
    {ERRTYPE::FILE_EXIST, "FATAL:FILE-IO"},
    {ERRTYPE::FILE_NOT_EXIST, "FATAL:FILE-IO"},
    {ERRTYPE::FILE_NOT_READABLE, "FATAL:FILE-IO"},
    {ERRTYPE::FILE_NOT_WRITABLE, "FATAL:FILE-IO"},
    {ERRTYPE::FILE_NOT_OPEN, "FATAL: FILE-IO"},

    {ERRTYPE::UNREC_TOKEN, "FATAL: UNREC-TOEKN"},
};

void
Msg::error(ERRTYPE err, const std::string& msg)
{
    printf("[%s]%s", _labelTab[err], msg.c_str());
}
    
void 
Msg::warn(ERRTYPE err, const std::string& msg)
{
    error(err, msg);
}
    
void
Msg::message(ERRTYPE err, const std::string& msg)
{
    error(err, msg);
}

}
