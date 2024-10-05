#pragma once

#include <map> 
#include <vector>
#include <string>

namespace SYNTAX{

class VarDef;
class FuncDef;
class AtomExpr;

class Scope{
public:
    Scope() : _parent(nullptr) {}
    VarDef*                         resolveVar(std::string name);
    FuncDef*                        resolveFunction(std::string name);

    void                            defVar(VarDef* def);
    std::vector<VarDef*>            getVariables();

    void                            defFunc(FuncDef* def);
    std::vector<FuncDef*>           getFunctions();

    Scope*                          getParent() { return _parent; }
    void                            setParent(Scope* s) { _parent = s; }
    void                            addChild(Scope* child) { _children.push_back(child); }
    Scope*                          addChild();

private:
    Scope*                          _parent;
    std::vector<Scope*>             _children;
    std::map<std::string, VarDef*>  _defVars;
    std::vector<std::pair<std::string, FuncDef*>> _defFuncs;
};

}
