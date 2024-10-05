#pragma once

#include "IRVisitor.hh"
namespace IR
{

class Evaluator : public IRVisitor
{
public:
    int evaluate(IRExpr* root);
private:
    virtual void visit(IRExpr*) {}
    virtual void visit(IRVar*) ;
    virtual void visit(IRInt*) ;
    virtual void visit(IRStr*) {}
    virtual void visit(IRAddr*) {}
    virtual void visit(IRMem*) {}
    virtual void visit(IRCall*) {}
    virtual void visit(IRAssign*) ;
    virtual void visit(IRBinExpr*) ;
    virtual void visit(IRUniExpr*) ;
    virtual void visit(IRExprStmt*) {}
    virtual void visit(IRJmp*) {}
    virtual void visit(IRCjmp*) {}
    virtual void visit(IRRET*) {} 
    virtual void visit(IRLabel*) {} 
    virtual void visit(IR*) {} 
private:
    int _value;
};

}
