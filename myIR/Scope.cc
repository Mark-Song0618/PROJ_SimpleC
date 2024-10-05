#include "Scope.hh"
#include "entity.hh"

namespace IR
{

int Scope::_tmpEntityID = 0;

Scope*
Scope::addChildScope()
{
    Scope* scope = new Scope();
    scope->setParentScope(this);
    addChildScope(scope);
    return scope;
}

Variable*
Scope::getTmpVariable(unsigned size)
{
    Variable* tmp = new Variable(size);
    tmp->setName("tmpVar" + std::to_string(_tmpEntityID++));
    addVariable(tmp);
    return tmp;
}

}
