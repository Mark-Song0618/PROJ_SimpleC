#pragma once
#include <vector>

namespace IR
{

class Variable;
class Constant;


// @Scope 
//    This class is only used for calculate the allocatd size of a scope.
//    It is not used for id resolver.
//    So, the parameters of a function are not stored in this class.
class Scope{
public:
    void                    addVariable(Variable* var) { _definedVar.push_back(var); }

    std::vector<Variable*>  getVariables() { return _definedVar; }

    void                    addConstant(Constant* c) { _definedConst.push_back(c); }

    std::vector<Constant*>  getConstants() { return _definedConst; }

    Variable*               getTmpVariable(unsigned size);

    void                    setParentScope(Scope* scope) { _parent = scope; }

    Scope*                  getParentScope() { return _parent; }

    void                    addChildScope(Scope* scope) { _children.push_back(scope); }

    Scope*                  addChildScope(); 

    std::vector<Scope*>     getChildScope() { return _children; }    

private:
    Scope*                  _parent;
    std::vector<Scope*>     _children;    
    std::vector<Variable*>  _definedVar;
    std::vector<Constant*>  _definedConst;

    static int             _tmpEntityID;
};

}
