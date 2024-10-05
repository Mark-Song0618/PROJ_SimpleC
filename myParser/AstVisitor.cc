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
VISITNODE(BreakStmt)
VISITNODE(ContinueStmt)
VISITNODE(ExprStmt)
VISITNODE(Variable)
VISITNODE(FuncCall)
VISITNODE(StrLiteral)
VISITNODE(IntLiteral)
VISITNODE(FloatLiteral)
VISITNODE(Parenthesed)
VISITNODE(Keyword)
VISITNODE(InclStmt)
VISITNODE(UniOpExpr)
VISITNODE(BinOpExpr)
VISITNODE(MemberExpr)
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
    if (func->getRetType()) {
        func->getRetType()->accept(this);
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
AstVisitor::visit(ContinueStmt*) {}
    
void    
AstVisitor::visit(BreakStmt*) {}

void 
AstVisitor::visit(ExprStmt* node)
{
    if (node->getExpr())
        node->getExpr()->accept(this);
}

void 
AstVisitor::visit(InclStmt* node)
{
}

void
AstVisitor::visit(Variable* var)
{
}

void 
AstVisitor::visit(FuncCall* node)
{
    for (auto expr : node->getParams()) {
        expr->accept(this);
    }
}

void
AstVisitor::visit(StrLiteral* str)
{
}

void
AstVisitor::visit(IntLiteral* literal)
{
}

void
AstVisitor::visit(FloatLiteral* literal)
{
}

void
AstVisitor::visit(Parenthesed* expr)
{
    if (expr->getExpr()) {
        expr->getExpr()->accept(this);
    }
}

void
AstVisitor::visit(Keyword* kw)
{
}

void 
AstVisitor::visit(UniOpExpr* expr)
{
    if (expr->getExpr()) {
        expr->getExpr()->accept(this);
    }
}

void 
AstVisitor::visit(BinOpExpr* expr)
{
    if(expr->getLhs()) expr->getLhs()->accept(this);  
    if(expr->getRhs()) expr->getRhs()->accept(this);  
}

void 
AstVisitor::visit(MemberExpr* node)
{
    if(node->getExpr()) {
        node->getExpr()->accept(this);
    }
}

void 
AstVisitor::visit(TypeNode* node)
{
}

}

