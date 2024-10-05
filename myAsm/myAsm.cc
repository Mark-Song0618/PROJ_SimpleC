#include <fstream>
#include "myAsm.hh"
#include "Register.hh"
#include "../utils/Exception.hh"

namespace ASM
{

std::string     
Oper::dump()
{
    if(_type == 0) {
        return _reg->dump();
    } else if (_type == 1) {
        return _ref->dump();
    } else if (_type == 2) {
        return _imm.dump();
    } else {
        return "";
    }
}

Oper::OperSize
Oper::getSize(unsigned size) 
{
    if (size == 1) {
        return INT_8;
    } else if (size == 2) {
        return INT_16;
    } else if (size <= 4) {
        return INT_32;
    } else if (size <= 8) {
        return INT_64;
    } else {
        throw UTIL::MyException("unsupported size");
    }
    return INT_8;
}

void            
Oper::setSize(OperSize size)
{
    if (_type == 0) {
        _reg->setSize(size);
    }
}

Oper::OperSize
Oper::size()
{
    if (_type == 0) return _reg->size();
    if (_type == 1) return getSize(_ref->size());
    if (_type == 2) return getSize(_imm.size());
}

std::string             
Inst::dump()
{
    std::string inst = _inst + suffix();
    if (!_oper.empty()) {
        inst = inst + "\t" + _oper[0].dump();
        if (_oper.size() == 2) {
            inst = inst + ",\t" + _oper[1].dump(); 
        }
    }
    return inst;
}

std::string             
Inst::suffix()
{
    if (_suffix != "") {
        return _suffix;
    } 

    //
    // get size from dest Oper
    if (_oper.size() == 1) {
        return suffix(_oper[0].size());
    } else if (_oper.size() == 2) {
        if (_oper[0].size() >= _oper[1].size()) {
            _oper[1].setSize(_oper[0].size());
            return suffix(_oper[0].size());
        } else {
            _oper[0].setSize(_oper[1].size());
            return suffix(_oper[1].size());
        }
    }
    return "";
}

std::string             
Inst::suffix(Oper::OperSize size)
{
    if (size == Oper::INT_8) {
        return "b";
    } else if (size == Oper::INT_16) {
        return "w";
    } else if (size == Oper::INT_32) {
        return "l";
    } else if (size == Oper::INT_64) {
        return "q";
    }
}

void    
Assemble::dump(std::string file)
{
    std::ofstream output(file);
    for (auto code : _assemblies) {
       output << code->dump(); 
    }
    output.close();
}

}
