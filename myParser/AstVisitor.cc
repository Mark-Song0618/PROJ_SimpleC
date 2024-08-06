#include "AstVisitor.hh"
#include "astPub.hh"

namespace SYNTAX {

VISITNODE(Program)
VISITNODE(ReturnStmt)
VISITNODE(FuncDef)
VISITNODE(StructDef)
VISITNODE(VarDef)
VISITNODE(TypeDef)
VISITNODE(IfStmt)
VISITNODE(ForStmt)
VISITNODE(AssignStmt)
VISITNODE(FuncCall)
VISITNODE(InclStmt)
VISITNODE(AtomExpr)
VISITNODE(UniOpExpr)
VISITNODE(BinOpExpr)
VISITNODE(TypeNode)

void 
AstVisitor::visit(Program* prog)
{
    for (auto stmt : prog->getStatements()) {
        stmt->accept(this);
    }
}

void
AstVisitor::visit(ReturnStmt* rt)
{
    if (rt->getRetExpr())
        rt->getRetExpr()->accept(this);
}

void 
AstVisitor::visit(FuncDef* func)
{
    if (func->getRtType()) {
        func->getRtType()->accept(this);
    }
    if (func->getId()) {
        func->getId()->accept(this);
    }
    for (auto param : func->getParams()) {
        param->accept(this);
    }
    for (auto stmt : func->getStmts()) {
        stmt->accept(this);
    }
}

void 
AstVisitor::visit(StructDef* def)
{
    if (def->getId()) def->getId()->accept(this);
    for (auto mem: def->getmembers()) {
        mem->accept(this);
    }
}

void 
AstVisitor::visit(VarDef* var)
{
    if (var->getTypeNode()) {
        var->getTypeNode()->accept(this);
    }

    if (var->getId()) {
        var->getId()->accept(this);
    }

    if (var->getInit()) {
        var->getInit()->accept(this);
    }
}

void 
AstVisitor::visit(TypeDef* tdef)
{
    if (tdef->getOrigType()) {
        tdef->getOrigType()->accept(this);
    }

    if (tdef->getDefinedType()) {
        tdef->getDefinedType()->accept(this);
    }

}

void 
AstVisitor::visit(IfStmt* node)
{
    if (node->getCondition()) {
        node->getCondition()->accept(this);
    }
    for (auto stmt : node->getStatements()) {
        stmt->accept(this);
    }
    for (auto stmt : node->getElseStmts()) {
        stmt->accept(this);
    }
}

void 
AstVisitor::visit(ForStmt* node)
{
    for (auto init : node->getInit()) {
        init->accept(this);
    }
    if (node->getCondition()) {
        node->getCondition()->accept(this);
    }
    for (auto stmt: node->getSteps()) {
        stmt->accept(this);
    }
    for (auto stmt: node->getBody()) {
        stmt->accept(this);
    }
}

void 
AstVisitor::visit(AssignStmt* node)
{
    if (node->getLhs()) node->getLhs()->accept(this);
    if (node->getRhs()) node->getRhs()->accept(this);
}

void 
AstVisitor::visit(FuncCall* node)
{
    for (auto expr : node->getParams()) {
        expr->accept(this);
    }
}

void 
AstVisitor::visit(InclStmt* node)
{
}

void 
AstVisitor::visit(AtomExpr* expr)
{
    if (expr->getAtomType() == AtomExpr::AtomType::FuncCall) {
        expr->getFuncCall()->accept(this); 
    } else if (expr->getAtomType() == AtomExpr::AtomType::Parenthesed) {
        expr->getParenthesed()->accept(this); 
    } 
}

void 
AstVisitor::visit(UniOpExpr* expr)
{
    if (expr->getFactor()) {
        expr->getFactor()->accept(this);
    }
}

void 
AstVisitor::visit(BinOpExpr* expr)
{
    if(expr->getLhs()) expr->getLhs()->accept(this);  
    if(expr->getRhs()) expr->getRhs()->accept(this);  
}

void 
AstVisitor::visit(TypeNode* node)
{
    if (!node->isBasic())
        node->getBasicType()->accept(this);
}


}

