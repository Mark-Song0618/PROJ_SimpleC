#include "astPub.hh"
#include "../utils/Exception.hh"

namespace SYNTAX
{

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

FuncVec                 
Program::getFuncs()
{
    return _scope->getFunctions();
}

VarDefVec               
Program::getGlobalVars()
{
    return _scope->getVariables();
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

void                    
Program::addStatements(std::vector<Statement*> stmts)
{
    _stmts.insert(_stmts.end(), stmts.begin(), stmts.end());
}

FuncDef::FuncDef()
    :Statement(SyntaxType::FuncDef), _defined(false), _varArg(false) {}

FuncDef::~FuncDef()
{
    delete _rtType;
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
    for (auto mem : _members) delete mem;
}

void                    
StructDef::addMember(VarDef* member)
{
    _members.push_back(member);
}

StructType*
StructDef::extractType()
{
    if (!_defined) {
        return nullptr;
    }
    StructType* type = new StructType(_name);
    for (auto& member :_members) {
        type->addMember(member->getId(), member->getTypeNode()->getType()); // the type of a struct member is definitely not a typeref.
    }
    return type;
}

VarDef::VarDef() 
    :Statement(SyntaxType::VarDef), _type(nullptr), _initVal(nullptr) {}

VarDef::~VarDef()
{
    delete _type;
    if (_initVal) delete _initVal;
}

void                    
VarDef::addInitVal(Expr* initVal)
{
    _initVal = initVal;
}

unsigned
VarDef::size()
{
    return _type->getType()->size();
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

bool
Variable::isLoadable()
{
    return !getType()->isArrayType();
}

bool 
Variable::isAssignale()
{
    return !getType()->isConst();
}

Type* 
Variable::getType()
{
    if (!_defVar) {
        return nullptr;
    }

    return _defVar->getTypeNode()->getType();
}

FuncCall::~FuncCall()
{
    for (auto param : _params) delete param;
}

void                    
FuncCall::addParam(Expr* param)
{
    _params.push_back(param);
}

Type*
FuncCall::getType()
{
    if (!_defFunc) {
        return nullptr;
    }
    return _defFunc->getRetType()->getType();
}

unsigned                
FuncCall::size()
{
    return _defFunc->getRetType()->getType()->size();
}

Type*
StrLiteral::getType()
{
    Type* charType = TypeTable::getType("char");
    return TypeTable::getPointerType(charType);
}

UniOpExpr::UniOpExpr(UniOpType type)
    :Expr(SyntaxType::UniOpExpr), _opType(type) {}

UniOpExpr::~UniOpExpr()
{
    if (_expr) delete _expr;
}

bool                    
UniOpExpr::isLhs()
{
    return _opType == RESOLVEADDR;
}
   
bool
UniOpExpr::isAssignale()
{
    if (isLoadable()) {
        return !getType()->isConst(); 
    }
    return false;
}

bool                    
UniOpExpr::isLoadable()
{
    if (isLhs() && !getType()->isArrayType()) {
        return true;
    }
    return false;
}

Type*                   
UniOpExpr::getType()
{
    if (_opType == UniOpExpr::GETADDR) {
        return TypeTable::getPointerType(_expr->getType());
    } else if (_opType == UniOpExpr::RESOLVEADDR) {
        if (!_expr->getType()->isPointerType() && !_expr->getType()->isArrayType()) {
            throw UTIL::MyException("resolveAddr on non pointer type");
        } else {
            PointerType* ptrType = dynamic_cast<PointerType*>(_expr->getType());
            return ptrType->getBasicType();
        }
    } else {
        return _expr->getType();
    }
}

BinOpExpr::~BinOpExpr()
{
   if (_left) delete _left;
   if (_right) delete _right;
}
    
Type*  
BinOpExpr::getType()
{
    switch (_opType) {
    case REL:
    case LESS:
    case LARGER:
    case LESSE:
    case LARGERE:
    case EQUAL:
    case NOTEQUAL:
    case AND:
    case OR:
        return TypeTable::getType("int");
    case BITOR:
    case BITAND:
    case BITXOR:
    case SHIFT:
    case SHR:
    case SHL:
    case MULTI:
    case DIV:
    case MOD:
    case ASSIGN:
        return _left->getType();
    case ADD:
    case MINUS:
        break;
    case BAD:
        return nullptr;
    }

    if (_right->getType()->isPointerType()) {
        return _right->getType();
    } else {
        return _left->getType();
    }
}

MemberExpr::~MemberExpr()
{
    if (_expr) delete _expr;
}

Type*            
MemberExpr::getBasicType()
{
    if (_expr->syntaxType() == SyntaxType::MemberExpr) {
        MemberExpr* expr = dynamic_cast<MemberExpr*>(_expr);
        return expr->getMemberType();
    } else if (_expr->syntaxType() == SyntaxType::Variable) {
        VarDef* def = dynamic_cast<Variable*>(_expr)->getDefine();
        if (def) {
            return def->getTypeNode()->getType();
        }
    }
    return nullptr;
}

Type*                
MemberExpr::getMemberType()
{
    Type* type = getBasicType();
    if (type && type->isStructType()) {
        return dynamic_cast<StructType*>(type)->memberType(_member);
    }
    return nullptr;
}

Type*
MemberExpr::getType()
{
   return getMemberType(); 
}

bool
TypeNode::isResolved()
{
    if (_type == nullptr) {
        return false;
    }
    Type* type = _type;
    while (type->isPointerType() || type->isRefType()) {
        type = type->getBasicType();
        if (!type) {
            return false;
        }
    }
    return true;
}

}
