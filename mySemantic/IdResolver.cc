#include "IdResolver.hh"


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
    SYNTAX::AstVisitor::visit(n);
    postAction(n);
}

void    
IdResolver::visit(SYNTAX::VarDef* n)
{
    preAction(n);
    SYNTAX::AstVisitor::visit(n);
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
IdResolver::preAction(SYNTAX::FuncDef* def) 
{
    SYNTAX::Scope* scope = new SYNTAX::Scope();
    auto parentScope = _scopeStack.top();
    parentScope->addChild(scope);
    scope->setParent(parentScope);
    _scopeStack.push(scope);
    def->setScope(scope);

    parentScope->defFunc(def);
}

template<>
void    
IdResolver::preAction(SYNTAX::VarDef* def) 
{
    auto scope = _scopeStack.top();
    scope->defVar(def);
}

template<>
void    
IdResolver::preAction(SYNTAX::ForStmt* fStmt) 
{
    auto parentScope= _scopeStack.top();
    SYNTAX::Scope* scope = new SYNTAX::Scope;
    parentScope->addChild(scope);
    scope->setParent(parentScope);
    fStmt->setScope(scope);
}

template<>
void    
IdResolver::postAction(SYNTAX::Program* prog) 
{
    _scopeStack.pop();
}

template<>
void    
IdResolver::postAction(SYNTAX::FuncDef*) 
{
    _scopeStack.pop();
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
        SYNTAX::Scope* scope1 = new SYNTAX::Scope;
        parentScope->addChild(scope1);
        scope1->setParent(parentScope);
        _scopeStack.push(scope1);
        ifStmt->setIfScope(scope1);
        for (auto stmt : ifStmt->getStatements()) {
            stmt->accept(this);
        }
        _scopeStack.pop();
    }
    if (!ifStmt->getElseStmts().empty()) {
        SYNTAX::Scope* scope2 = new SYNTAX::Scope;
        parentScope->addChild(scope2);
        scope2->setParent(parentScope);
        _scopeStack.push(scope2);
        ifStmt->setElseScope(scope2);
        for (auto stmt : ifStmt->getElseStmts()) {
            stmt->accept(this);
        }
        _scopeStack.pop();
    }
}

void
IdResolver::visit(SYNTAX::AtomExpr* expr)
{
    SYNTAX::Scope* currScope = _scopeStack.top();
    using Type = SYNTAX::AtomExpr::AtomType;
    switch(expr->getAtomType()) {
    case Type::Variable: 
    {
        auto def = currScope->resolveVar(expr->getIdName());
        if (def)
            expr->setVarDef(def);
        break;
    }
    case Type::FuncCall:
    {
        auto def = currScope->resolveFunction(expr->getFuncCall()->getFuncName());
        expr->setFuncDef(def);
        SYNTAX::AstVisitor::visit(expr);
        break;
    }
    case Type::StrLiteral:
    case Type::FloatLiteral:
    case Type::IntLiteral:
        currScope->defLiteral(expr);
        break;
    case Type::Parenthesed:
        SYNTAX::AstVisitor::visit(expr);
        break;
    default:
        break;
    } 
}

}
