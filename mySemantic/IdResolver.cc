#include "IdResolver.hh"
#include "../utils/Exception.hh"


namespace SEMANTIC
{

void    
IdResolver::visit(SYNTAX::Program* n)
{
    preAction(n);
    SYNTAX::AstVisitor::visit(n);
    postAction(n);
}

void    
IdResolver::visit(SYNTAX::FuncDef* n) 
{
    preAction(n);
    for (auto param : n->getParams()) {
        param->accept(this);
    }
    for (auto stmt : n->getStmts()) {
        stmt->accept(this);
    }
    postAction(n);
}

void    
IdResolver::visit(SYNTAX::ForStmt* n)
{
    preAction(n);
    SYNTAX::AstVisitor::visit(n);
    postAction(n);
}

template<>
void    
IdResolver::preAction(SYNTAX::Program* prog) 
{
    SYNTAX::Scope* scope = new SYNTAX::Scope();
    _scopeStack.push(scope);
    prog->setScope(scope);
}

template<>
void    
IdResolver::postAction(SYNTAX::Program* prog) 
{
    _scopeStack.pop();
}

template<>
void    
IdResolver::preAction(SYNTAX::FuncDef* def) 
{
    auto parentScope = _scopeStack.top();
    auto scope = parentScope->addChild();
    _scopeStack.push(scope);
    def->setScope(scope);

    parentScope->defFunc(def);
}

template<>
void    
IdResolver::postAction(SYNTAX::FuncDef*) 
{
    _scopeStack.pop();
}

template<>
void    
IdResolver::preAction(SYNTAX::ForStmt* fStmt) 
{
    auto parentScope= _scopeStack.top();
    auto scope = parentScope->addChild();
    fStmt->setScope(scope);
    _scopeStack.push(scope);
}

template<>
void    
IdResolver::postAction(SYNTAX::ForStmt*) 
{
    _scopeStack.pop();
}

void    
IdResolver::visit(SYNTAX::IfStmt* ifStmt)
{
    ifStmt->getCondition()->accept(this);
    SYNTAX::Scope* parentScope = _scopeStack.top();
    if (!ifStmt->getStatements().empty()) {
        SYNTAX::Scope* scope1 = parentScope->addChild();
        _scopeStack.push(scope1);
        ifStmt->setIfScope(scope1);
        for (auto stmt : ifStmt->getStatements()) {
            stmt->accept(this);
        }
        _scopeStack.pop();
    }
    if (!ifStmt->getElseStmts().empty()) {
        SYNTAX::Scope* scope2 = parentScope->addChild();
        _scopeStack.push(scope2);
        ifStmt->setElseScope(scope2);
        for (auto stmt : ifStmt->getElseStmts()) {
            stmt->accept(this);
        }
        _scopeStack.pop();
    }
}

void    
IdResolver::visit(SYNTAX::VarDef* def) 
{
    auto scope = _scopeStack.top();
    scope->defVar(def);
    SYNTAX::AstVisitor::visit(def);
}

void    
IdResolver::visit(SYNTAX::Variable* var)
{
    SYNTAX::Scope* currScope = _scopeStack.top();
    auto def = currScope->resolveVar(var->getId());
    if (def) {
        var->setVarDef(def);
    } else {
        std::string error = "Undefined Variable: " + var->getId();
        throw UTIL::MyException(error.c_str());
    }
}

void    
IdResolver::visit(SYNTAX::FuncCall* func)
{
    SYNTAX::Scope* currScope = _scopeStack.top();
    auto def = currScope->resolveFunction(func->funcName());
    if (!def) {
        std::string error = "Undefined Function: " + func->funcName();
        throw UTIL::MyException(error.c_str());
    }
    func->setFuncDef(def);
    for (auto& param : func->getParams()) {
        param->accept(this);
    }
}

}
