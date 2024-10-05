#pragma once

#include "../myIR/Literal.hh"
namespace ASM 
{

class Immediate 
{
public:
    Immediate() : _int(0), _isInt(true) {}
    Immediate(IR::IntLiteral val) : _int(val), _isInt(true) {}
    Immediate(IR::IRSymbol sym) : _sym(sym), _isInt(false) {}

    std::string         dump(); 
    unsigned            size();
private:
    IR::IntLiteral      _int;
    IR::IRSymbol        _sym;
    bool                _isInt;
};

}
