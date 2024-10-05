#include "Literal.hh"
#include "../myParser/type.hh"

namespace IR
{

unsigned    
IRSymbol::size()
{
    SYNTAX::Type* type = SYNTAX::TypeTable::getType("char");
    return SYNTAX::TypeTable::getPointerType(type)->size();
}

unsigned    
IntLiteral::size()
{
    SYNTAX::Type* type = SYNTAX::TypeTable::getType("int");
    return type->size();
}

}
