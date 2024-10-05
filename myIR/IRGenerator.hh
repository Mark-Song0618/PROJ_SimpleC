#pragma once

#include "../myParser/astPub.hh"
#include "../myParser/AstVisitor.hh"
#include "ir.hh"
#include "entity.hh"
#include <stack>
#include <map>

namespace IR
{

class IRGenerator : public SYNTAX::AstVisitor
{
public:
    IR*                     generate(SYNTAX::Program* program);

private:
    Constant*               transConst(SYNTAX::VarDef*, bool = false);
    Variable*               transVariable(SYNTAX::VarDef*, bool isStatic = false);
    Function*               transFunc(SYNTAX::FuncDef*);

    bool                    isConstant(SYNTAX::VarDef*);

    void                    transExpr(SYNTAX::Expr*);
    void                    transStmt(SYNTAX::Statement*);
    IRExpr*                 fetchExpr();
    void                    transIncDecExpr(SYNTAX::UniOpExpr* expr);
    void                    transLogicExpr(SYNTAX::BinOpExpr* expr);

    IRAssign*               assignExpr(IRExpr* left, IRExpr* right);
    IRExprStmt*             assignStmt(IRExpr* left, IRExpr* right);
    Variable*               defineTmpVar(unsigned size);
    IRAddr*                 addressOf(IRExpr* expr); // get address of expr
    IRMem*                  mem(unsigned, IRExpr*); // access to mem[expr], size: the byte size of the object in memory
    IRVar*                  ref(Entity*); // ref to a entity
    void                    label(Label*);

    IRBinExpr::BinOpType    getOpType(SYNTAX::BinOpExpr::BinOpType type);
    IRUniExpr::UniOpType    getOpType(SYNTAX::UniOpExpr::UniOpType type);
    IRExpr*                 getMemberAddr(SYNTAX::MemberExpr* expr);


    virtual void            visit(SYNTAX::ReturnStmt*);
    virtual void            visit(SYNTAX::VarDef*);
    virtual void            visit(SYNTAX::IfStmt*);
    virtual void            visit(SYNTAX::ForStmt*);
    virtual void            visit(SYNTAX::BreakStmt*);
    virtual void            visit(SYNTAX::ContinueStmt*);
    virtual void            visit(SYNTAX::ExprStmt*);
    virtual void            visit(SYNTAX::Variable*);
    virtual void            visit(SYNTAX::FuncCall*);
    virtual void            visit(SYNTAX::StrLiteral*);
    virtual void            visit(SYNTAX::IntLiteral*);
    virtual void            visit(SYNTAX::UniOpExpr*);
    virtual void            visit(SYNTAX::BinOpExpr*);
    virtual void            visit(SYNTAX::MemberExpr*);

private:
    IR*                                 _ir;
    IRExpr*                             _ExprBuffer;
    std::vector<IRStmt*>*               _StmtBuffer;
    std::stack<Label*>                  _break;
    std::stack<Label*>                  _continue;
    std::map<std::string, IRLabel*>     _jmpMap;
    std::map<SYNTAX::VarDef*, Entity*>  _entityMap;
    std::map<std::string, ConstEntry*>  _literals;
    std::map<SYNTAX::FuncDef*,Function*>_funcMap;
    Scope*                              _currScope;
    bool                                _processExpr;
};

}
