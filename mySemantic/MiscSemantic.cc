#include "MiscSemantic.hh"
#include "../utils/Exception.hh"

namespace SEMANTIC 
{


void    
MiscSemanChecker::visit(SYNTAX::MemberExpr* expr)
{
    checkMember(expr);
    SYNTAX::AstVisitor::visit(expr); 
}

void    
MiscSemanChecker::visit(SYNTAX::BinOpExpr* expr)
{
    if(expr->getBinType() == SYNTAX::BinOpExpr::BinOpType::ASSIGN) {
        checkAssignable(expr->getLhs());
    }
}

void    
MiscSemanChecker::visit(SYNTAX::UniOpExpr* expr)
{
    auto isAssign = [expr]()->bool {
        switch (expr->getUniType()) {
        case SYNTAX::UniOpExpr::UniOpType::POSTINC:
        case SYNTAX::UniOpExpr::UniOpType::POSTDEC:
        case SYNTAX::UniOpExpr::UniOpType::PREINC:
        case SYNTAX::UniOpExpr::UniOpType::PREDEC:
            return true; 
        default:
            return false;
        }
    };
    if (isAssign()) {
        checkAssignable(expr->getExpr());
    }
    if (expr->getUniType() == SYNTAX::UniOpExpr::UniOpType::GETADDR) {
        checkLhs(expr->getExpr());
    }
    if (expr->getUniType() == SYNTAX::UniOpExpr::UniOpType::RESOLVEADDR) {
        checkIsPointer(expr->getExpr());
    }
}

void    
MiscSemanChecker::checkMember(SYNTAX::MemberExpr* expr) 
{
    SYNTAX::Type* basicType = expr->getBasicType();
    if (!basicType) {
        throw UTIL::MyException("No Type Defined for Member Expr"); 
    }
    if (!expr->getMemberType()) {
        std::string error = expr->getMember()+ "is not a member of Type: " + basicType->typeName();
        throw UTIL::MyException(error.c_str()); 
    } 
}

void
MiscSemanChecker::checkLhs(SYNTAX::Expr* expr)
{
    if (!expr->isLhs()) {
        throw UTIL::MyException("get address is only valid for lhs"); 
    }
}

void    
MiscSemanChecker::checkIsPointer(SYNTAX::Expr* expr)
{
    if (!expr->getType()->isPointerType()) {
        throw UTIL::MyException("express is not a pointer"); 
    }
}

void    
MiscSemanChecker::checkAssignable(SYNTAX::Expr* expr)
{
    // todo: const check
    //if (!expr->isLhs() || expr->getType()->isConst()) {
    if (!expr->isLhs()) {
        throw UTIL::MyException("expression is not assignable");
    }
}

}
