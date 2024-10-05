#pragma once

#include "ir.hh"

#define VISIT_IRNODE(T) \
    void T::accept(IRVisitor* visitor) { visitor->visit(this); }

namespace IR
{

class IRVisitor
{
public:
    virtual void visit(IRExpr*) {}
    virtual void visit(IRVar*) ;
    virtual void visit(IRInt*) ;
    virtual void visit(IRStr*) ;
    virtual void visit(IRAddr*) ;
    virtual void visit(IRMem*) ;
    virtual void visit(IRCall*) ;
    virtual void visit(IRAssign*) ;
    virtual void visit(IRBinExpr*) ;
    virtual void visit(IRUniExpr*) ;
    virtual void visit(IRExprStmt*) ;
    virtual void visit(IRJmp*) ;
    virtual void visit(IRCjmp*) ;
    virtual void visit(IRRET*) ;
    virtual void visit(IRLabel*) ;
    virtual void visit(IR*) ;
};

}

