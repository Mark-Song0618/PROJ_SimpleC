#pragma once

#include "../myParser/AstVisitor.hh"

namespace SEMANTIC
{

/**
 *
 *  This class Check the type consistance of assignment.
 *  Function Parameters are also a kind of assignment.
 *
 *  The (implicit and explicit) type cast is not supported yet.
 *
 *
 * */

class TypeChecker :public SYNTAX::AstVisitor
{
public:
    void            check(SYNTAX::Program* root) { root->accept(this); }

private:
    void            visit(SYNTAX::BinOpExpr*);
    void            visit(SYNTAX::FuncDef*);
    void            visit(SYNTAX::FuncCall*);
    void            visit(SYNTAX::ReturnStmt*);

    void            checkTypeCompatible(SYNTAX::Expr* expr1, SYNTAX::Expr* expr2);
    void            checkTypeCompatible(SYNTAX::Expr* expr, SYNTAX::Type* type);

private:
    SYNTAX::Type*   _retType;
};

}
