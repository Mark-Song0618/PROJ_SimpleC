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
    VarDef*             resolveVar(std::string name);
    FuncDef*            resolveFunction(std::string name);

    void                defVar(VarDef* def);
    void                defFunc(FuncDef* def);
    void                defLiteral(AtomExpr* litExpr);
    void                setParent(Scope* s) { _parent = s; }
    void                addChild(Scope* child) { _children.push_back(child); }
private:
    Scope*                          _parent;
    std::vector<Scope*>             _children;
    std::vector<AtomExpr*>          _literals;
    std::map<std::string, VarDef*>  _defVars;
    std::map<std::string, FuncDef*> _defFuncs;
};

}
