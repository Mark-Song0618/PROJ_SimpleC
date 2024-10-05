#pragma once

#include <string>
#include "memRef.hh"
#include "Scope.hh"
namespace IR {

class IRExpr;
class IRStmt;

class Entity {
public:
    Entity() : _memRef(nullptr) {}
    virtual unsigned        size() { return 0; } 

    virtual bool            isConst() { return false; }

    void                    setName(std::string name) { _name = name; }

    std::string             name() { return _name; }

    std::string             dump();

    void                    setMemRef(MemRef* memref) { _memRef = memref; }

    MemRef*                 getMemRef() { return _memRef;}

    void                    setAddr(IRSymbol sym) { _addr = sym; }

    IRSymbol                address() { return _addr; }

private:
    std::string             _name;
    
    IRSymbol                _addr;

    MemRef*                 _memRef;
};

class Constant : public Entity
{
public:
    Constant(unsigned size) : _size(size), _init(nullptr) {}

    bool                    isConst() override { return true; }

    void                    setInitVal(IRExpr* init) { _init = init; }

    IRExpr*                 getInitVal() { return _init; }

    unsigned                size() override { return _size; }

private:
    unsigned                _size;
    IRExpr*                 _init;
};

class Variable : public Entity 
{
public:
    Variable(unsigned size) : _size(size), _init(nullptr) {}

    void                    setInitVal(IRExpr* init) { _init = init; }

    IRExpr*                 getInitVal() { return _init; }

    unsigned                size() override { return _size; }

private:
    unsigned                _size;
    IRExpr*                 _init;

};

class Function : public Entity
{
public:
    Function() : _size(0) {}

    Scope*                  getScope() { return _scope; }

    void                    setScope(Scope* scope) { _scope = scope; } 

    void                    addStmts(std::vector<IRStmt*> body) { _stmts = body; }

    void                    addStmt(IRStmt* stmt) { _stmts.push_back(stmt); }

    std::vector<IRStmt*>    getStmt() { return _stmts; }

    std::vector<IRStmt*>&   fetchStmt() { return _stmts; }

    void                    addParam(Entity* param) { _params.push_back(param); }
    std::vector<Entity*>    getParams() { return _params; }

    unsigned                size() override { return _size; }

    void                    setRetsize(unsigned size) { _size = size; }

    bool                    isDefined() { return _defined; }

    void                    setDefined(bool b) { _defined = b; }

    unsigned                getParamsSize();

private:
    Scope*                  _scope;

    std::vector<Entity*>    _params;

    std::vector<IRStmt*>    _stmts;

    unsigned                _size;

    bool                    _defined;
};

}
