#include "Scope.hh"
#include "astPub.hh"

namespace SYNTAX
{


VarDef*
Scope::resolveVar(std::string name)
{
    if (_defVars.find(name) != _defVars.end()) {
        return _defVars[name];
    } else if (_parent) {
        return _parent->resolveVar(name);
    }
    return nullptr;
}

FuncDef*      
Scope::resolveFunction(std::string name)
{
    if (_defFuncs.find(name) != _defFuncs.end()) {
        return _defFuncs[name];
    } else if (_parent) {
        return _parent->resolveFunction(name);
    }
    return nullptr;

}

void              
Scope::defVar(VarDef* def)
{
    _defVars.insert({def->getId()->getIdName(), def});
}

void                
Scope::defFunc(FuncDef* def)
{
    _defFuncs.insert({def->getId()->getIdName(), def});
}

void 
Scope::defLiteral(AtomExpr* litExpr)
{
    _literals.push_back(litExpr);
}

}
