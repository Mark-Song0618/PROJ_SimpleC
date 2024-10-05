#include "IRVisitor.hh"
#include "ir.hh"

namespace IR
{

VISIT_IRNODE(IRExpr)
VISIT_IRNODE(IRVar)
VISIT_IRNODE(IRInt)
VISIT_IRNODE(IRStr)
VISIT_IRNODE(IRAddr)
VISIT_IRNODE(IRMem)
VISIT_IRNODE(IRCall)
VISIT_IRNODE(IRAssign)
VISIT_IRNODE(IRBinExpr)
VISIT_IRNODE(IRUniExpr)
VISIT_IRNODE(IRExprStmt)
VISIT_IRNODE(IRJmp)
VISIT_IRNODE(IRCjmp)
VISIT_IRNODE(IRRET)
VISIT_IRNODE(IRLabel)
VISIT_IRNODE(IR)


void
IRVisitor::visit(IRVar*) 
{
}
void
IRVisitor::visit(IRInt*) 
{
}
void
IRVisitor::visit(IRStr*) 
{
}
void
IRVisitor::visit(IRAddr* addr) 
{
    if (addr->getValue()) {
        addr->getValue()->accept(this);
    }
}
void
IRVisitor::visit(IRMem* expr) 
{
    expr->getPtr()->accept(this);
}
void
IRVisitor::visit(IRCall* call) 
{
    for (auto param : call->getParams()) {
        param->accept(this);
    }
}
void
IRVisitor::visit(IRAssign* assign) 
{
    assign->getLeft()->accept(this);
    assign->getRight()->accept(this);
}
void
IRVisitor::visit(IRBinExpr* expr) 
{
    expr->getLeft()->accept(this);
    expr->getRight()->accept(this);
}

void
IRVisitor::visit(IRUniExpr* expr) 
{
    expr->getExpr()->accept(this);
}
void
IRVisitor::visit(IRExprStmt* stmt) 
{
    stmt->getExpr()->accept(this);
}
void
IRVisitor::visit(IRJmp* jmp) 
{
}

void
IRVisitor::visit(IRCjmp* jmp) 
{
    jmp->getCond()->accept(this);
}
void
IRVisitor::visit(IRRET* ret) 
{
    ret->getRetValue()->accept(this);
}
void
IRVisitor::visit(IRLabel*) 
{
}
void
IRVisitor::visit(IR* ir) 
{
}

}
