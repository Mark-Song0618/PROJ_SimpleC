#pragma once

#include "../myParser/astPub.hh"
#include "../myParser/AstVisitor.hh"
#include <cwctype>
#include <stack>


namespace SEMANTIC {

class IdResolver final : public SYNTAX::AstVisitor 
{
public:
    void resolve(SYNTAX::TreeNode* root) { root->accept(this); }

private:
    virtual void    visit(SYNTAX::Program* n); 

    virtual void    visit(SYNTAX::FuncDef* n); 

    virtual void    visit(SYNTAX::VarDef*);

    virtual void    visit(SYNTAX::ForStmt*);

    virtual void    visit(SYNTAX::IfStmt*);

    virtual void    visit(SYNTAX::AtomExpr*);

    // create a scope when enter a block;
    // gather symbols to currScope
    template<typename T>
    void    preAction(T*) {}

    // pop a scope when exit a block
    template <typename T>
    void    postAction(T*) {}
private:
    std::stack<SYNTAX::Scope*> _scopeStack;
};

template<>
void    
IdResolver::preAction(SYNTAX::Program* prog); 

template<>
void    
IdResolver::preAction(SYNTAX::FuncDef* def); 

template<>
void    
IdResolver::preAction(SYNTAX::VarDef* def); 

template<>
void    
IdResolver::preAction(SYNTAX::ForStmt* fStmt); 

template<>
void    
IdResolver::postAction(SYNTAX::Program* prog); 

template<>
void    
IdResolver::postAction(SYNTAX::FuncDef*);

template<>
void    
IdResolver::postAction(SYNTAX::ForStmt*); 

}
