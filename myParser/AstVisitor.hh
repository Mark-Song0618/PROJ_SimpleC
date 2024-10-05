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
    virtual void    visit(ContinueStmt*);
    virtual void    visit(BreakStmt*);
    virtual void    visit(ExprStmt*);
    virtual void    visit(InclStmt*);
    virtual void    visit(Variable*);
    virtual void    visit(FuncCall*);
    virtual void    visit(StrLiteral*);
    virtual void    visit(IntLiteral*);
    virtual void    visit(FloatLiteral*);
    virtual void    visit(Parenthesed*);
    virtual void    visit(Keyword*);
    virtual void    visit(UniOpExpr*);
    virtual void    visit(BinOpExpr*);
    virtual void    visit(MemberExpr*);
    virtual void    visit(TypeNode*);
};


}
