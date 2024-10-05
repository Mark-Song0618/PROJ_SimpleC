#include "X86Register.hh"

namespace ASM {

std::string     
X86Register::dump()
{
    std::string str = "%" + basicStr();    
    switch(_size) {
    case Oper::INT_8:
        str[str.size()-1] = 'l';
        return str;
    case Oper::INT_16:
        return str;
    case Oper::INT_32:
        str.insert(str.begin()+1, 'e');
        return str;
    case Oper::INT_64:
        str.insert(str.begin()+1, 'r');
        return str;
    }
}

unsigned
X86Register::byte()
{
    switch (_size) {
    case Oper::OperSize::INT_8:
        return 1;
    case Oper::OperSize::INT_16:
        return 2;
    case Oper::OperSize::INT_32:
        return 4;
    case Oper::OperSize::INT_64:
        return 8;
    }
}

std::string     
X86Register::basicStr()
{
    switch(_type) {
    case RegisterType::AX:
        return "ax";
    case RegisterType::BX:
        return "bx";
    case RegisterType::CX:
        return "cx";
    case RegisterType::DX:
        return "dx";
    case RegisterType::SI:
        return "si";
    case RegisterType::DI:
        return "di";
    case RegisterType::SP:
        return "sp";
    case RegisterType::BP:
        return "bp";
    case RegisterType::IP:
        return "ip";
    case RegisterType::R8:
        return "r8";
    case RegisterType::R9:
        return "r9";
    }    
}

}
