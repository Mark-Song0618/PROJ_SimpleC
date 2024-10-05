#pragma once

#include <map>
#include <string>
#include <vector>
#include "Literal.hh"
namespace IR
{

class MemRef;

class ConstEntry 
{
public:
    ConstEntry(std::string literal, unsigned size) : _value(literal), _size(size) { _name = ".LC" + std::to_string(_cnt++);}

    std::string             getLiteral() { return _value; }

    void                    setMemRef(MemRef* memRef) { _memRef = memRef; }

    MemRef*                 getMemRef() { return _memRef;}

    void                    setAddr(IRSymbol sym) { _addr = sym; }

    IRSymbol                address() { return _addr; }

    std::string             dump();

    std::string             name() { return _name; }

    unsigned                size() { return _size; }

private:
    std::string             _value;
    std::string             _name;
    MemRef*                 _memRef;
    IRSymbol                _addr;
    unsigned                _size;
    static int              _cnt;
};

class ConstTable {
public:
    void                        clear() { _entries.clear(); }
    void                        addLiteral(ConstEntry* literal); 
    ConstEntry*                 getLiteral(const std::string&);
    std::vector<ConstEntry*>    getLiterals();

private:
    std::map<std::string, ConstEntry*>  
                                _entries;
};

}
