#pragma once

#include "../myParser/AstVisitor.hh"
#include "../myParser/astPub.hh"

namespace SEMANTIC 
{

// 1. check Member Expr 
// 2. check get Address
// 3. check address dereference
// 4. check expression assignable
// 5. 

class MiscSemanChecker: public SYNTAX::AstVisitor
{
public:
    void    check(SYNTAX::Program* root) { root->accept(this); } 

private:
    void    visit(SYNTAX::UniOpExpr*);
    void    visit(SYNTAX::BinOpExpr*);
    void    visit(SYNTAX::MemberExpr*);

    void    checkMember(SYNTAX::MemberExpr* expr);
    void    checkAssignable(SYNTAX::Expr* expr);
    void    checkLhs(SYNTAX::Expr* expr);
    void    checkIsPointer(SYNTAX::Expr* expr);

private:
    SYNTAX::Type*       _currType;
    unsigned            _currOffset;
};


}
