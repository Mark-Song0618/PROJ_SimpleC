#pragma once
#include <string>
namespace IR
{

class Register
{
    public:
        virtual std::string dump() = 0;
};

}
