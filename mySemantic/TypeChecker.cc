#include "TypeChecker.hh"
#include "../utils/Exception.hh"


namespace SEMANTIC
{

void    
TypeChecker::visit(SYNTAX::BinOpExpr* expr)
{
    if (expr->getRhs()) {
        checkTypeCompatible(expr->getLhs(), expr->getRhs());
    }
}

void    
TypeChecker::visit(SYNTAX::FuncDef* def)
{
    if (def->getRetType()) {
        _retType = def->getRetType()->getType();
    }
}

void            
TypeChecker::visit(SYNTAX::ReturnStmt* ret)
{
    checkTypeCompatible(ret->getRetExpr(), _retType);

}

void    
TypeChecker::visit(SYNTAX::FuncCall* func)
{
    SYNTAX::FuncDef* def = func->getFuncDef();
    if (def) {
        auto params = func->getParams();
        auto paraDefs = def->getParams();
        for (int idx = 0; idx != params.size(); ++idx) {
            checkTypeCompatible(params[idx], paraDefs[idx]->getTypeNode()->getType()); 
        }
    }
}

void
TypeChecker::checkTypeCompatible(SYNTAX::Expr* expr1, SYNTAX::Expr* expr2)
{
    SYNTAX::Type* type1 = expr1->getType();
    SYNTAX::Type* type2 = expr2->getType();
    if (type1 != type2) {
        throw UTIL::MyException("type not compatible");
    }
}

void    
TypeChecker::checkTypeCompatible(SYNTAX::Expr* expr, SYNTAX::Type* type)
{
    SYNTAX::Type* exprType = expr->getType();
    if (exprType != type) {
        throw UTIL::MyException("type not compatible");
    }
}

}
