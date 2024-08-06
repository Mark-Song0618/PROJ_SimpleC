#pragma once
#include "astPub.hh"

#define VISITNODE(T) \
void T::accept(AstVisitor* v) { \
    v->visit(this); \
}

namespace SYNTAX {

class AstVisitor {
public:
    virtual void    visit(Program*);
    virtual void    visit(ReturnStmt*);
    virtual void    visit(FuncDef*);
    virtual void    visit(StructDef*);
    virtual void    visit(VarDef*);
    virtual void    visit(TypeDef*);
    virtual void    visit(IfStmt*);
    virtual void    visit(ForStmt*);
    virtual void    visit(AssignStmt*);
    virtual void    visit(FuncCall*);
    virtual void    visit(InclStmt*);
    virtual void    visit(AtomExpr*);
    virtual void    visit(UniOpExpr*);
    virtual void    visit(BinOpExpr*);
    virtual void    visit(TypeNode*);
};


}
