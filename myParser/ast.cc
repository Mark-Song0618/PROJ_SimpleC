#include "astPub.hh"

namespace SYNTAX
{

SyntaxType
TreeNode::type() { return _type; } 

Program::Program()
    :TreeNode(SyntaxType::Program)
{
    _parent = nullptr;
}

Program::~Program()
{
    for (auto stmt : _stmts) {
        delete stmt;
    }
}

StmtVec
Program::fetchStatements()
{
    StmtVec rt = std::move(_stmts);
    _stmts.clear();
    return std::move(rt); 
}

void
Program::addStatement(Statement* stmt)
{
    _stmts.push_back(stmt);
}

FuncDef::FuncDef()
    :Statement(SyntaxType::FuncDef), _defined(false) {}

FuncDef::~FuncDef()
{
    delete _rtType;
    delete _id;
    for (auto param : _params) delete param;
    for (auto stmt : _stmts) delete stmt;
}

void                    
FuncDef::addParam(VarDef* paramDecl)
{
    _params.push_back(paramDecl);
}

void
FuncDef::addStatement(Statement* stmt)
{
    _stmts.push_back(stmt);
}

StructDef::StructDef()
    :Statement(SyntaxType::StructDef), _defined(false) {}

StructDef::~StructDef()
{
    delete _id;
    for (auto mem : _members) delete mem;
}

void                    
StructDef::addMember(VarDef* member)
{
    _members.push_back(member);
}

VarDef::VarDef() 
    :Statement(SyntaxType::VarDef), _type(nullptr), _id(nullptr), _initVal(nullptr) {}

VarDef::~VarDef()
{
    delete _type;
    delete _id;
    if (_initVal) delete _initVal;
}

void                    
VarDef::addInitVal(Expr* initVal)
{
    _initVal = initVal;
}

TypeDef::TypeDef()
    :Statement(SyntaxType::TypeDef) {}

void                    
TypeDef::setOrigType(TypeNode* origType)
{
    _type = origType;
}

TypeDef::~TypeDef()
{
    if (_type) delete _type;
    if (_id) delete _id;
}

IfStmt::IfStmt()
    :Statement(SyntaxType::IfStmt) {}

IfStmt::~IfStmt()
{
    delete _condition;
    for (auto stmt : _stmts) delete stmt;
    for (auto estmt : _elseStmts) delete estmt;

    if (_iScope) delete _iScope;
    if (_eScope) delete _eScope;
}

void
IfStmt::setCondition(Expr* condition)
{
    _condition = condition;
}

void                    
IfStmt::addThenStmt(Statement* stmt)
{
    _stmts.push_back(stmt);
}

void                    
IfStmt::addElseStmt(Statement* stmt)
{
    _elseStmts.push_back(stmt);
}

ForStmt::ForStmt()
    :Statement(SyntaxType::ForStmt) {}

ForStmt::~ForStmt()
{
    for (auto init : _initDefs) delete init;
    if (_condition) delete _condition;
    for (auto step : _steps) delete step;
    for (auto body : _body) delete body;
}

void                    
ForStmt::addInit(VarDef* def)
{
    _initDefs.push_back(def);
}

void
ForStmt::addCondition(Expr* expr)
{
    _condition = expr;
}

void                    
ForStmt::addStep(Statement* step)
{
    _steps.push_back(step);
}

void                
ForStmt::addBody(Statement* stmt)
{
    _body.push_back(stmt);
}

AssignStmt::AssignStmt()
    :Statement(SyntaxType::Assignment) {}

AssignStmt::~AssignStmt()
{
    delete _left;
    if (_right) delete _right;
}

void                    
AssignStmt::addLhs(Expr* lhs)
{
    _left = lhs;
}

void
AssignStmt::addRhs(Expr* rhs)
{
    _right = rhs;
}

FuncCall::FuncCall()
    :Statement(SyntaxType::FuncCall) {}

FuncCall::~FuncCall()
{
    for (auto param : _params) delete param;
}

void                    
FuncCall::setName(const std::string& funcName)
{
    _funcName = funcName;
}

void                    
FuncCall::addParam(Expr* param)
{
    _params.push_back(param);
}

AtomExpr::~AtomExpr()
{
    if (_parenthesedExpr) delete _parenthesedExpr;
    if (_func) delete _func;
}

UniOpExpr::UniOpExpr(UniOpType type)
    :Expr(SyntaxType::UniOpExpr), _type(type) {}

UniOpExpr::~UniOpExpr()
{
    if (_expr) delete _expr;
}

BinOpExpr::~BinOpExpr()
{
   if (_left) delete _left;
   if (_right) delete _right;
}

TypeNode*
TypeNode::getBasicType(bool resolveOnce)
{
    TypeNode* baseType = nullptr;
    if (isBasic()) {
        return baseType;
    }
    
    if (resolveOnce) {
        return _baseType;
    }

    baseType = _baseType;
    while (baseType && !baseType->isBasic()) {
        baseType = baseType->_baseType;
    }

    return baseType;
}

TypeNode*               
TypeNode::getReferedType(bool resolveOnce)
{
    TypeNode* baseType = this;
    if (getTypeId() != TypeId::Ref) {
        return baseType;
    }
   
    // todo: baseType = Semantic::getType(_typename);
    if (resolveOnce) {
        return baseType;
    }

    while (baseType && (baseType->getTypeId() == TypeId::Ref)) {
        // todo: baseType = Semantic::getType(_typename);
    }
    return baseType;
}

TypeNode::TypeId
TypeNode::getTypeId()
{ 
    return _typeid;
}

void
TypeNode::setConst(bool top)
{
    if (top) {
        _topConst = true;
    } else {
        _lowConst = true;
    }
}

}
