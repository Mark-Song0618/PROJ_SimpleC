#include "memRef.hh"
#include "../utils/Exception.hh"
namespace IR
{

std::string 
DirectMemRef::dump()
{
    return getAddress().dump() + "(%rip)";
}

std::string 
IndirectMemRef::dump()
{
    return std::to_string(_offset) + "(" + _base->dump() + ")";
}
void 
MemRef::redirect(int offset)
{
    throw UTIL::MyException("MemRef cannot be redirect");
}

void
IndirectMemRef::redirect(int offset)
{
    _offset += offset;
}

}
