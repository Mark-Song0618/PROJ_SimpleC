#include "Immediate.hh"

namespace ASM
{

std::string
Immediate::dump()
{
    if (_isInt) {
        return _int.dump();
    } else {
        return _sym.dump();
    }
}

unsigned            
Immediate::size()
{
    if (_isInt) {
        return _int.size();
    } else {
        return _sym.size();
    }
}

}
