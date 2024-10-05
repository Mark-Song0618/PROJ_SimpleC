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
    for (auto p : _defFuncs) {
        if (p.first == name)
            return p.second;
    }
    if (_parent) {
        return _parent->resolveFunction(name);
    }
    return nullptr;

}

void              
Scope::defVar(VarDef* def)
{
    _defVars.insert({def->getId(), def});
}

void                
Scope::defFunc(FuncDef* def)
{
    _defFuncs.push_back({def->getName(), def});
}

std::vector<VarDef*>
Scope::getVariables()
{
    std::vector<VarDef*> rt;
    for (auto [_, def] : _defVars) {
            rt.push_back(def);
    }
    return rt;
}

std::vector<FuncDef*>
Scope::getFunctions()
{
    std::vector<FuncDef*> rt;
    for (auto [_, def] : _defFuncs) {
            rt.push_back(def);
    }
    return rt;
}

Scope*
Scope::addChild()
{
    Scope* child = new Scope();
    child->setParent(this);
    addChild(child);
    return child;
}

}
