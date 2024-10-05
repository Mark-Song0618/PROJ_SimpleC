#pragma once

#include "Register.hh"
#include "Literal.hh"

namespace IR
{

class MemRef 
{
public:
    MemRef(unsigned size) : _size(size) {}

    virtual void        redirect (int offset);

    virtual std::string dump() = 0;

    unsigned            size() { return _size; }

private:
    unsigned            _size; // referred size
};

class DirectMemRef : public MemRef {
public:
    DirectMemRef(IRSymbol address, unsigned size) : MemRef(size), _address(address) {}

    void        setAddress(IRSymbol address) { _address = address; }

    IRSymbol    getAddress() { return _address; }
    
    std::string dump() override;

private:
    IRSymbol    _address;
};

class IndirectMemRef : public MemRef {
public:
    IndirectMemRef(Register* base, int offset, unsigned size) : MemRef(size), _base(base), _offset(offset) {}

    void        redirect(int offset) override;

    std::string dump() override;

private:
    Register*   _base;
    int         _offset;
};

}
