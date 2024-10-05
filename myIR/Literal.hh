#pragma once
#include <string>
namespace IR
{

class Literal 
{
public:
    virtual std::string dump() = 0;
};

class IRSymbol : public Literal {
public:
    IRSymbol() {}
    
    IRSymbol(std::string symbol) : _symbol(symbol) {}

    void        setSymbol(std::string str) { _symbol = str; }

    std::string dump() override { return _symbol; }

    bool        valid() { return !_symbol.empty(); }
    
    unsigned    size(); 

private:
    std::string _symbol;
};

class IntLiteral : public Literal {
public:
    IntLiteral() {};
    
    IntLiteral(int val) : _value(val) {}

    void        setValue(int value) { _value = value; }

    std::string dump() override { return "$" + std::to_string(_value); }

    unsigned    size(); 

private:
    int         _value;
};

}
