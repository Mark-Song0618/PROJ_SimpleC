#include "IRGenerator.hh"
#include "ir.hh"
#include "../utils/Exception.hh"

namespace IR
{

IR*     
IRGenerator::generate(SYNTAX::Program* program)
{
    _ir = new IR();
    _ir->setSrcFile(program->srcFile());
    _currScope = _ir->getScope();
    
    for (auto definedVar: program->getGlobalVars()) {
        if (isConstant(definedVar)) {
            _currScope->addConstant(transConst(definedVar, true));
        } else {
            _currScope->addVariable(transVariable(definedVar, true));
        }
    }
    for (auto func: program->getFuncs()) {
        _ir->addFunc(transFunc(func));
    }
    return _ir;
}

Constant*
IRGenerator::transConst(SYNTAX::VarDef* var, bool isStatic)
{
    // todo: create constant by var
    Constant* constVar = new Constant(var->size());
    if (var->getInit()) {
        // parameter may has no initval
        transExpr(var->getInit());
        if (isStatic) {
            constVar->setInitVal(fetchExpr());
        } else {
            IRVar* tmp = new IRVar(constVar);
            IRExprStmt* stmt = assignStmt(tmp, fetchExpr());
            _StmtBuffer->push_back(stmt);
        }
    }
    constVar->setName(var->getId());
    _entityMap.insert({var, constVar});
    return constVar;
}

Variable*
IRGenerator::transVariable(SYNTAX::VarDef* var, bool isStatic)
{
    Variable* variable = new Variable(var->size());
    variable->setName(var->getId());
    if (var->getInit()) {
        transExpr(var->getInit());
        if (isStatic) {
            variable->setInitVal(fetchExpr());
        } else {
            IRVar* tmp = new IRVar(variable);
            IRExprStmt* stmt = assignStmt(tmp, fetchExpr());
            _StmtBuffer->push_back(stmt);
        }
    }
    _entityMap.insert({var, variable});
    return variable;
}

bool                    
IRGenerator::isConstant(SYNTAX::VarDef* def)
{
    return def->getTypeNode()->getType()->isConst();
}

Function*       
IRGenerator::transFunc(SYNTAX::FuncDef* func)
{
    Function* IRFunc = new Function();
    IRFunc->setName(func->getName());
    IRFunc->setDefined(func->isDefined());
    if (func->getRetType()) {
        unsigned retSize = func->getRetType()->getType()->size();
        IRFunc->setRetsize(retSize);
    }
    Entity* entity = nullptr;
    _currScope = _currScope->addChildScope(); 
    IRFunc->setScope(_currScope);
    _StmtBuffer = &(IRFunc->fetchStmt());
    for (auto param : func->getParams()) {
        if (isConstant(param)) {
            entity = transConst(param);
        } else {
            entity = transVariable(param);
        }
        IRFunc->addParam(entity);
        _entityMap.insert({param, entity});
    }
    for (auto stmt : func->getStmts()) {
        transStmt(stmt);
    }
    _currScope = _currScope->getParentScope();
    _funcMap.insert({func, IRFunc});

    return IRFunc;
}

void
IRGenerator::transStmt(SYNTAX::Statement* stmt)
{
    stmt->accept(this);
}

void
IRGenerator::transExpr(SYNTAX::Expr* expr)
{
    expr->accept(this);
}

IRExpr*
IRGenerator::fetchExpr()
{
    IRExpr* rt = _ExprBuffer;
    _ExprBuffer = nullptr;
    return rt;
}

IRAssign*
IRGenerator::assignExpr(IRExpr* left, IRExpr* right)
{
    return new IRAssign(addressOf(left), right);
}

IRExprStmt*
IRGenerator::assignStmt(IRExpr* left, IRExpr* right)
{
    return new IRExprStmt(assignExpr(left, right));
}

Variable*
IRGenerator::defineTmpVar(unsigned size)
{
    return _currScope->getTmpVariable(size);
}

IRAddr*                 
IRGenerator::addressOf(IRExpr* expr)
{
    if (expr->isVar()) {
        return new IRAddr(dynamic_cast<IRVar*>(expr)->getEntity());
    } else if (expr->isMemRef()) {
        // *p = xxx
        return new IRAddr(dynamic_cast<IRMem*>(expr)->getPtr());
    } else {
        throw UTIL::MyException("expect a lhs");
    }
    return new IRAddr(expr);
}

IRMem*
IRGenerator::mem(unsigned objSize, IRExpr* ptr)
{
    return new IRMem(objSize, ptr);
}

IRVar*                  
IRGenerator::ref(Entity* ent)
{
    return new IRVar(ent);
}

void 
IRGenerator::label(Label* label)
{
    _StmtBuffer->push_back(new IRLabel(label));
}

void                    
IRGenerator::transIncDecExpr(SYNTAX::UniOpExpr* expr)
{
    using namespace SYNTAX;
    transExpr(expr->getExpr());
    IRExpr* uniExpr = fetchExpr();
    switch(expr->getUniType()) {
    case UniOpExpr::UniOpType::PREINC: 
    {
        auto addExpr = new IRBinExpr(uniExpr->size(), IRBinExpr::BinOpType::ADD, uniExpr, new IRInt(uniExpr->size(), 1));
        _StmtBuffer->push_back((assignStmt(uniExpr, addExpr)));
        _ExprBuffer = uniExpr;
        return;
    }
    case UniOpExpr::UniOpType::POSTINC: 
    {
        auto tmpVar = _currScope->getTmpVariable(uniExpr->size());
        _StmtBuffer->push_back(assignStmt(ref(tmpVar), uniExpr));
        auto addExpr = new IRBinExpr(uniExpr->size(), IRBinExpr::BinOpType::ADD, uniExpr, new IRInt(uniExpr->size(), 1));
        _StmtBuffer->push_back((assignStmt(uniExpr, addExpr)));
        _ExprBuffer = ref(tmpVar);
        return;
    }
    case UniOpExpr::UniOpType::PREDEC:
    {
        auto addExpr = new IRBinExpr(uniExpr->size(), IRBinExpr::BinOpType::SUB, uniExpr, new IRInt(uniExpr->size(), 1));
        _StmtBuffer->push_back((assignStmt(uniExpr, addExpr)));
        _ExprBuffer = uniExpr;
        return;
    
    }
    case UniOpExpr::UniOpType::POSTDEC:
    {
        auto tmpVar = _currScope->getTmpVariable(uniExpr->size());
        _StmtBuffer->push_back(assignStmt(ref(tmpVar), uniExpr));
        auto addExpr = new IRBinExpr(uniExpr->size(), IRBinExpr::BinOpType::SUB, uniExpr, new IRInt(uniExpr->size(), 1));
        _StmtBuffer->push_back((assignStmt(uniExpr, addExpr)));
        _ExprBuffer = ref(tmpVar);
        return;
    }
    defualt:
        throw UTIL::MyException("unsupported unary operator");
    }
}

void                    
IRGenerator::transLogicExpr(SYNTAX::BinOpExpr* expr)
{
    auto type = expr->getBinType();
    if (type != SYNTAX::BinOpExpr::BinOpType::AND && type != SYNTAX::BinOpExpr::BinOpType::OR) {
        throw UTIL::MyException("unsupported binary operator");
    }
    transExpr(expr->getLhs());
    IRExpr* lhs = fetchExpr();
    transExpr(expr->getRhs());
    IRExpr* rhs = fetchExpr();
    Variable* tmp = defineTmpVar(lhs->size());
    Label* l1 = new Label();
    Label* l2 = new Label();
    IRExprStmt* stmt1 = assignStmt(ref(tmp), lhs); 
    IRCjmp* stmt2 = nullptr; 
    IRLabel* stmt3 = new IRLabel(l1);
    IRExprStmt* stmt4 = assignStmt(ref(tmp), rhs);
    IRLabel* stmt5 = new IRLabel(l2);
    if (type == SYNTAX::BinOpExpr::BinOpType::AND) {
        stmt2 = new IRCjmp(lhs, l1, l2); 
    } else if (type == SYNTAX::BinOpExpr::BinOpType::OR) {
        stmt2 = new IRCjmp(lhs, l2, l1); 
    }  
    _StmtBuffer->push_back(stmt1);
    _StmtBuffer->push_back(stmt2);
    _StmtBuffer->push_back(stmt3);
    _StmtBuffer->push_back(stmt4);
    _StmtBuffer->push_back(stmt5);
    _ExprBuffer = ref(tmp);
}

void
IRGenerator::visit(SYNTAX::ReturnStmt* stmt)
{
    IRRET* ret = new IRRET();
    if (stmt->getRetExpr()) {
        transExpr(stmt->getRetExpr());
        ret->addRetValue(fetchExpr());
    }
    _StmtBuffer->push_back(ret);
}

void
IRGenerator::visit(SYNTAX::VarDef* def)
{
    if (isConstant(def)) {
        _currScope->addConstant(transConst(def));
    } else {
        _currScope->addVariable(transVariable(def));
    }
}

void
IRGenerator::visit(SYNTAX::IfStmt* stmt)
{
    transExpr(stmt->getCondition());
    IRExpr* cond = fetchExpr();
    Label* lthen = new Label();
    Label* lend = new Label();
    if (stmt->getElseStmts().empty()) {
        IRCjmp* cjmp = new IRCjmp(cond, lthen, lend);
        _StmtBuffer->push_back(cjmp);
        label(lthen);
        _currScope = _currScope->addChildScope();
        for (auto thenStmt : stmt->getStatements()) {
            transStmt(thenStmt);
        }
        _currScope = _currScope->getParentScope();
        label(lend);
    } else {
        Label* lelse = new Label();
        IRCjmp* cjmp = new IRCjmp(cond, lthen, lelse);
        _StmtBuffer->push_back(cjmp);
        label(lthen);
        _currScope =_currScope->addChildScope();
        for (auto thenStmt : stmt->getStatements()) {
            transStmt(thenStmt);
        }
        IRJmp* jmp = new IRJmp(lend);
        _StmtBuffer->push_back(jmp);
        label(lelse);
        _currScope = _currScope->getParentScope()->addChildScope();
        for (auto elseStmt : stmt->getElseStmts()) {
            transStmt(elseStmt);
        }
        _currScope =_currScope->getParentScope();
        label(lend);
    }
}

void
IRGenerator::visit(SYNTAX::ForStmt* stmt)
{
    _currScope = _currScope->addChildScope(); // to store loop vars
    for (auto init : stmt->getInit()) {
        transStmt(init);
    }
    transExpr(stmt->getCondition());
    IRExpr* cond = fetchExpr();
    Label* lbegin = new Label();
    Label* lend = new Label();
    Label* lcont = new Label();
    label(lcont);
    IRCjmp* cjmp = new IRCjmp(cond, lbegin, lend);
    _StmtBuffer->push_back(cjmp);
    label(lbegin);
    _break.push(lend);
    _continue.push(lcont);
    for (auto body : stmt->getBody()) {
        transStmt(body);
    }
    for (auto step : stmt->getSteps()) {
        transStmt(step);
    }
    IRJmp* jmp = new IRJmp(lcont);
    _StmtBuffer->push_back(jmp);
    _break.pop();
    _continue.pop();
    label(lend);
    _currScope = _currScope->getParentScope();
}

void            
IRGenerator::visit(SYNTAX::BreakStmt*)
{
    IRJmp* jmp = new IRJmp(_break.top()); 
    _StmtBuffer->push_back(jmp);
}

void
IRGenerator::visit(SYNTAX::ContinueStmt*)
{
    IRJmp* jmp = new IRJmp(_continue.top()); 
    _StmtBuffer->push_back(jmp);
}

void
IRGenerator::visit(SYNTAX::FuncCall* call)
{
    std::vector<IRExpr*> params;
    for (auto param : call->getParams()) {
        transExpr(param);
        params.push_back(fetchExpr());
    }
    auto funcCall = new IRCall(_funcMap[call->getFuncDef()], params);
    auto tmpVar = _currScope->getTmpVariable(call->getFuncDef()->getRetType()->getType()->size());
    _StmtBuffer->push_back(assignStmt(ref(tmpVar), funcCall));
    _ExprBuffer = ref(tmpVar);
}

void            
IRGenerator::visit(SYNTAX::StrLiteral* str)
{
    // new constEntry;
    ConstEntry* entry = nullptr;
    if (_literals.find(str->getLiteral()) == _literals.end()) {
        entry = new ConstEntry(str->getLiteral(), str->size());
        _literals.insert({str->getLiteral(), entry});
        _ir->addLiteral(entry);
    } else {
        entry = _literals[str->getLiteral()];
    }
    // add constEntry to constTable
    _ExprBuffer = new IRStr(entry);
}

void            
IRGenerator::visit(SYNTAX::IntLiteral* literal)
{
    _ExprBuffer = new IRInt(literal->getType()->size(), literal->getLiteral());
}

void            
IRGenerator::visit(SYNTAX::Variable* var)
{
    _ExprBuffer = new IRVar(_entityMap[var->getDefine()]);
}

void
IRGenerator::visit(SYNTAX::UniOpExpr* expr)
{
    using namespace SYNTAX;
    switch (expr->getUniType()) {
    case UniOpExpr::UniOpType::PREINC: 
    case UniOpExpr::UniOpType::POSTINC:
    case UniOpExpr::UniOpType::PREDEC:
    case UniOpExpr::UniOpType::POSTDEC:
        transIncDecExpr(expr);
        break;
    case UniOpExpr::UniOpType::NEG:
    case UniOpExpr::UniOpType::BITNOT:         // ~
        transExpr(expr->getExpr());
        _ExprBuffer = new IRUniExpr(expr->getType()->size(), getOpType(expr->getUniType()), fetchExpr());
        break;
    case UniOpExpr::UniOpType::LOGICNOT:       // !
        transExpr(expr->getExpr());
        _ExprBuffer = new IRUniExpr(TypeTable::getType("int")->size(), IRUniExpr::UniOpType::LOGICNOT, fetchExpr());
        break;
    case UniOpExpr::UniOpType::GETADDR:
        transExpr(expr->getExpr());
        _ExprBuffer = addressOf(fetchExpr());
        return;
    case UniOpExpr::UniOpType::RESOLVEADDR: 
    {
        Type* type = expr->getType();
        if (!type->isPointerType()) {
            throw UTIL::MyException("expr is not a pointer");
        }
        transExpr(expr->getExpr());
        _ExprBuffer = mem(type->getBasicType()->size(), fetchExpr());
        return;
    }
    default:
        throw UTIL::MyException("unsupported unary operator");
    }
}

void
IRGenerator::visit(SYNTAX::BinOpExpr* expr)
{
    using namespace SYNTAX;
    switch (expr->getBinType()) {
    case BinOpExpr::BinOpType::LESS:
    case BinOpExpr::BinOpType::LARGER:
    case BinOpExpr::BinOpType::LESSE:
    case BinOpExpr::BinOpType::LARGERE:
    case BinOpExpr::BinOpType::EQUAL:
    case BinOpExpr::BinOpType::NOTEQUAL:
    {
        transExpr(expr->getLhs());
        IRExpr* lhs = fetchExpr();
        transExpr(expr->getRhs());
        IRExpr* rhs = fetchExpr();
        _ExprBuffer = new IRBinExpr(TypeTable::getType("int")->size(), getOpType(expr->getBinType()), lhs, rhs); 
        break;
    }
    case BinOpExpr::BinOpType::AND:
    case BinOpExpr::BinOpType::OR:
        transLogicExpr(expr);
        break;
    case BinOpExpr::BinOpType::BITOR:
    case BinOpExpr::BinOpType::BITAND:
    case BinOpExpr::BinOpType::BITXOR:
    case BinOpExpr::BinOpType::SHR:
    case BinOpExpr::BinOpType::SHL:
    case BinOpExpr::BinOpType::ADD:
    case BinOpExpr::BinOpType::MINUS:
    case BinOpExpr::BinOpType::MULTI:
    case BinOpExpr::BinOpType::DIV:
    case BinOpExpr::BinOpType::MOD:
    {
        unsigned size = expr->getLhs()->getType()->size();
        transExpr(expr->getLhs());
        IRExpr* lhs = fetchExpr();
        transExpr(expr->getRhs());
        IRExpr* rhs = fetchExpr();
        _ExprBuffer = new IRBinExpr(size, getOpType(expr->getBinType()), lhs, rhs);
        break;
    }
    case BinOpExpr::BinOpType::ASSIGN:
    {
        transExpr(expr->getLhs());
        IRExpr* lhs = fetchExpr();
        transExpr(expr->getRhs());
        IRExpr* rhs = fetchExpr();
        _StmtBuffer->push_back(assignStmt(lhs, rhs));
        Variable* tmp = defineTmpVar(lhs->size());
        _ExprBuffer = assignExpr(ref(tmp), rhs);
        break;
    }
    default:
        throw UTIL::MyException("unsupported binary operator");
    }

}

void            
IRGenerator::visit(SYNTAX::MemberExpr* expr)
{
    transExpr(expr->getExpr());
    IRExpr* basic = fetchExpr(); // the basic expr value has been stored here. It could be a pointer or a obj.
    SYNTAX::Type* basicType = expr->getBasicType();
    // for pointer, load the address first.
    if (basicType->isPointerType()) {
        basic = mem(PTR_SIZE, basic);
        basicType = dynamic_cast<SYNTAX::PointerType*>(basicType)->getBasicType();
        // now: basic is the struct object, and basicType is the type of the struct obj
    }
    SYNTAX::StructType* structType = dynamic_cast<SYNTAX::StructType*>(basicType);
    unsigned offset = structType->getMemberOffset(expr->getMember());
    SYNTAX::Type* memberType = structType->memberType(expr->getMember());
    IRExpr* addr = new IRBinExpr(PTR_SIZE, IRBinExpr::BinOpType::ADD, new IRInt(PTR_SIZE, offset), addressOf(basic));
    _ExprBuffer = mem(memberType->size(), addr); // load the member value 
}

void            
IRGenerator::visit(SYNTAX::ExprStmt* stmt)
{
    // the expr will be processed in transExpr
    transExpr(stmt->getExpr());
}

IRBinExpr::BinOpType    
IRGenerator::getOpType(SYNTAX::BinOpExpr::BinOpType type)
{
    using namespace SYNTAX;
    switch (type) {
    case BinOpExpr::BinOpType::LESS:
        return IRBinExpr::BinOpType::LT;
    case BinOpExpr::BinOpType::LARGER:
        return IRBinExpr::BinOpType::GT;
    case BinOpExpr::BinOpType::LESSE:
        return IRBinExpr::BinOpType::LE;
    case BinOpExpr::BinOpType::LARGERE:
        return IRBinExpr::BinOpType::GE;
    case BinOpExpr::BinOpType::EQUAL:
        return IRBinExpr::BinOpType::EQ;
    case BinOpExpr::BinOpType::NOTEQUAL:
        return IRBinExpr::BinOpType::NE;
    case BinOpExpr::BinOpType::BITOR:
        return IRBinExpr::BinOpType::BITOR;
    case BinOpExpr::BinOpType::BITAND:
        return IRBinExpr::BinOpType::BITAND;
    case BinOpExpr::BinOpType::BITXOR:
        return IRBinExpr::BinOpType::BITXOR;
    case BinOpExpr::BinOpType::SHR:
        return IRBinExpr::BinOpType::SHR;
    case BinOpExpr::BinOpType::SHL:
        return IRBinExpr::BinOpType::SHL;
    case BinOpExpr::BinOpType::ADD:
        return IRBinExpr::BinOpType::ADD;
    case BinOpExpr::BinOpType::MINUS:
        return IRBinExpr::BinOpType::SUB;
    case BinOpExpr::BinOpType::MULTI:
        return IRBinExpr::BinOpType::MUL;
    case BinOpExpr::BinOpType::DIV:
        return IRBinExpr::BinOpType::DIV;
    case BinOpExpr::BinOpType::MOD:
        return IRBinExpr::BinOpType::MOD;
    default:
        throw UTIL::MyException("unsupported BinOp Type");
    }
    return IRBinExpr::BinOpType::UNSUPPORTED;
}

IRUniExpr::UniOpType    
IRGenerator::getOpType(SYNTAX::UniOpExpr::UniOpType type)
{
    using namespace SYNTAX;
    switch(type) {
    case UniOpExpr::UniOpType::NEG:
        return IRUniExpr::UniOpType::NEG;
    case UniOpExpr::UniOpType::BITNOT:
        return IRUniExpr::UniOpType::BITNOT;
    case UniOpExpr::UniOpType::LOGICNOT:
        return IRUniExpr::UniOpType::LOGICNOT;
    default:
        throw UTIL::MyException("unsupported UniOp Type");
    }
    return IRUniExpr::UniOpType::UNSUPPORTED;
}

IRExpr*
IRGenerator::getMemberAddr(SYNTAX::MemberExpr* expr)
{
    unsigned offset = 0;
    SYNTAX::MemberExpr* currExpr = expr;
    while(currExpr) {
        SYNTAX::StructType* currType = dynamic_cast<SYNTAX::StructType*>(currExpr->getBasicType());
        std::string currMember = currExpr->getMember();
        offset += currType->getMemberOffset(currMember);
        SYNTAX::Variable* var = dynamic_cast<SYNTAX::Variable*>(currExpr->getExpr()); 
        if (var) {
            Entity* def = _entityMap[var->getDefine()]; 
            IRExpr* basic = new IRAddr(def);
            return new IRBinExpr(4, IRBinExpr::BinOpType::ADD, basic, new IRInt(4, offset));
        } else {
            currExpr = dynamic_cast<SYNTAX::MemberExpr*>(currExpr->getExpr());
        }
    }
    return nullptr; // wont get here
}

}
