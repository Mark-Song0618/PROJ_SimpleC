#include "X86Generator.hh"
#include "../myAsm.hh"
#include "../../utils/Exception.hh"
#include "../../myIR/Evaluator.hh"

#define SYSV
#define StackWordSize 16   /*byte*/
namespace ASM
{

Assemble*   
X86Generator::generate(IR::IR* program)
{
    _asmProg = new Assemble;
    generateHeader(program);
    generateDataSection(program);
    generateRODataSection(program);
    generateTextSection(program);
    return _asmProg;
}

void
X86Generator::generateHeader(IR::IR* program)
{
    _asmProg->addAsmCode(directive(".file\t" + std::string("\"") + program->srcFile() + "\""));
}

void
X86Generator::generateDataSection(IR::IR* root)
{
   declareSection(".data"); 
   for (auto variable : root->getGlobVariable()) {
        locateGlob(variable);
        declareData(variable);
   } 
}

void        
X86Generator::generateRODataSection(IR::IR* root)
{
   declareSection(".section .rodata"); 
   for (auto constVar: root->getGlobConst()) {
        locateGlob(constVar);
        declareData(constVar);
   } 
   for (auto str : root->getLiterals()) {
        locateGlob(str);
        declareString(str);
   } 
}

void        
X86Generator::generateTextSection(IR::IR* root)
{
    declareSection(".text"); 
    for (auto func : root->getFunctions()) {
        locateGlob(func);
        if (!func->isDefined()) {
            continue;
        }
        declareFunc(func);
        transFunc(func);
        _asmProg->addAsmCode(std::move(_funcAsm));
    }
}

void        
X86Generator::declareSection(std::string section)
{
    _asmProg->addAsmCode(directive("\t" + section));
}

void        
X86Generator::declareData(IR::Entity* entity)
{
    _asmProg->addAsmCode(directive("\t.globl\t" + entity->name()));
    _asmProg->addAsmCode(directive("\t.align\t" + std::to_string(entity->size())));
    _asmProg->addAsmCode(directive("\t.type\t" + entity->name() + ", @object"));
    _asmProg->addAsmCode(directive("\t.size\t" + entity->name() + "," + std::to_string(entity->size())));
    _asmProg->addAsmCode(directive(entity->name() + ":"));
    int init = 0;
    IR::Evaluator eval;
    if (entity->isConst()) {
        auto expr = dynamic_cast<IR::Constant*>(entity)->getInitVal();
        if (expr) 
            init = eval.evaluate(expr);
    } else {
        auto expr = dynamic_cast<IR::Variable*>(entity)->getInitVal();
        if (expr)
            init = eval.evaluate(expr);
    }
    _asmProg->addAsmCode(directive("\t.long\t" + std::to_string(init)));
}

void        
X86Generator::declareString(IR::ConstEntry* entry)
{
    _asmProg->addAsmCode(directive(entry->name() + ":"));
    _asmProg->addAsmCode(directive("\t.string\t" + entry->getLiteral()));
}

void        
X86Generator::declareFunc(IR::Function* func)
{
    _asmProg->addAsmCode(directive("\t.globl\t" + func->name()));
    _asmProg->addAsmCode(directive("\t.type\t" + func->name() + ",@function"));
    _asmProg->addAsmCode(directive(func->name() +":"));
}

void        
X86Generator::transFunc(IR::Function* func)
{
    initFuncData(func);
    locateParams(func);
    unsigned localVarSize = locateLocalVar(func->getScope(), 0);
    compileBody(func);
    preProcess(func, localVarSize);
    postProcess(func);
    redirect(func->getScope(), _usedRegs.totalSize() + func->getParamsSize());
}

std::vector<X86Register::RegisterType> 
X86Generator::_paramRegs = 
{
    X86Register::DI,
    X86Register::SI,
    X86Register::DX,
    X86Register::CX,
    X86Register::R8,
    X86Register::R9,
};

void        
X86Generator::passParam(IR::IRExpr* param, int idx)
{
    if (idx < 6) {
        auto reg = new X86Register(_paramRegs[idx], param->size());
        _funcAsm.push_back(mov(ax(param->size()), reg));
    } else {

    }
}

void 
X86Generator::initFuncData(IR::Function* func)
{
    _epilogue = Label(".Lend_" + func->name());
    _usedRegs.clear();
    _stack.init();
    _paramInstCnt = 0;
}

void
X86Generator::locateParams(IR::Function* func)
{
    unsigned startPos = 0;
    for (int idx = 0; idx != func->getParams().size(); ++idx) {
        auto param = func->getParams()[idx];
        startPos += param->size();
        IR::IndirectMemRef* ref = memRef(bp(), -startPos, param->size());
        param->setMemRef(ref);
        if (idx < 6) {
            auto reg = new X86Register(_paramRegs[idx], param->size()); 
            _funcAsm.push_back(mov(reg, ref));
        } else {

        }
    }
    _paramInstCnt = _funcAsm.size();
}

unsigned    
X86Generator::locateLocalVar(IR::Scope* scope, unsigned parentLen)
{
    unsigned currLen = parentLen;
    for (auto var : scope->getVariables()) {
        currLen += var->size();
        auto ref = memRef(bp(), -currLen, var->size());
        var->setMemRef(ref);
    }
    for (auto constVar: scope->getConstants()) {
        currLen += constVar->size();
        auto ref = memRef(bp(), -currLen, constVar->size());
        constVar->setMemRef(ref);
    }
    unsigned maxLen = currLen;
    for (auto child : scope->getChildScope()) {
        unsigned childLen = locateLocalVar(child, maxLen); 
        if (childLen > maxLen) maxLen = childLen;
    }
    return maxLen;
}

void        
X86Generator::compileBody(IR::Function* func)
{
    for (auto stmt : func->getStmt()) {
        stmt->accept(this);
    }
}

void        
X86Generator::preProcess(IR::Function* func, unsigned localVarSize)
{
    std::vector<AsmCode*> preProcess;
    unsigned reserveSize = func->getParamsSize() + localVarSize + _usedRegs.totalSize(); 
    reserveSize = _stack.getAlignedSize(reserveSize);
    _funcAsm.insert(_funcAsm.begin(), sub(imm(reserveSize), sp()));
    _funcAsm.insert(_funcAsm.begin(), mov(sp(), bp()));
    _funcAsm.insert(_funcAsm.begin(), push(bp(), false));

    unsigned calleeSavedPos = func->getParamsSize();
    for (int idx = 0; idx != _usedRegs.getUsedRegs().size(); ++idx) {
        auto reg = _usedRegs.getUsedRegs()[idx];
        calleeSavedPos += reg->byte();
        auto memRef = new IR::IndirectMemRef(bp(), -calleeSavedPos, reg->byte());
        preProcess.push_back(mov(reg, memRef));
    }
    _funcAsm.insert(_funcAsm.begin() + _paramInstCnt + 3, preProcess.begin(), preProcess.end());
    _stack.redirect(func->getParamsSize() + _usedRegs.totalSize() + localVarSize);
}

void        
X86Generator::postProcess(IR::Function* func)
{
    _funcAsm.push_back(label(_epilogue.symbol()));
    // restore callee saved registers
    unsigned savedRegSize = _usedRegs.totalSize();
    if (savedRegSize) {
        unsigned calleeSavedPos = savedRegSize + func->getParamsSize();
#ifdef dbg
        for (auto idx = _usedRegs.getUsedRegs().size()-1; idx != -1; --idx) {
            auto reg = _usedRegs.getUsedRegs()[idx];
            auto memRef = new IR::IndirectMemRef(bp(), -calleeSavedPos, reg->byte());
            _funcAsm.push_back(mov(memRef, reg));
            calleeSavedPos -= reg->byte();
        }
#endif
    }
    // restore stack frame [bp : sp]
    _funcAsm.push_back(mov(bp(), sp()));
    _funcAsm.push_back(pop(bp(), false));
    _funcAsm.push_back(ret());
}

void        
X86Generator::redirect(IR::Scope* root, unsigned offset)
{
    for (auto var : root->getVariables()) {
        auto ref = var->getMemRef();
        ref->redirect(-(int)offset);
    }
    for (auto constVar: root->getConstants()) {
        auto ref = constVar->getMemRef();
        ref->redirect(-(int)offset);
    }
    for (auto child : root->getChildScope()) {
        redirect(child, offset);
    }
}

IR::IndirectMemRef*
X86Generator::memRef(X86Register* base, unsigned offset, unsigned size)
{
    return new IR::IndirectMemRef(base, offset, size);
}

Immediate
X86Generator::imm(unsigned val)
{
    return Immediate(IR::IntLiteral(val));
}

void        
X86Generator::loadVar(IR::IRVar* var, X86Register* dest)
{
    auto entity = var->getEntity();
    unsigned varSize = var->size();
    if (entity->address().valid()) {
        _funcAsm.push_back(mov(Immediate(entity->address()), ax()));
        _funcAsm.push_back(mov(memRef(ax(), 0, varSize), dest));
    } else if (entity->getMemRef()) {
        auto src = entity->getMemRef();
        _funcAsm.push_back(mov(src, dest));
    } else {
        throw UTIL::MyException("variable has no address");
    }
}

void        
X86Generator::loadValue(IR::IRInt* val, X86Register* dest)
{
    _funcAsm.push_back(mov(Immediate(val->getValue()), dest));
}

void        
X86Generator::loadString(IR::IRStr *str, X86Register* dest)
{
    auto entry = str->getEntry();
    //if (entry->address().valid()) {
    //    _funcAsm.push_back(mov(Immediate(entry->address()), dest));   
    //} 
    if (entry->getMemRef()) {
        _funcAsm.push_back(lea(entry->getMemRef(), dest));
    } else {
        throw UTIL::MyException("String has no address");
    } 
}

void        
X86Generator::loadAddr(IR::IRAddr* addr)
{
    if (addr->getValue()) {
        addr->getValue()->accept(this); // will store the pointer at ax 
    } else if (addr->getEntity()) {
        auto entity = addr->getEntity();
        if (entity->address().valid()) {
            _funcAsm.push_back(mov(Immediate(entity->address()), ax(8))); 
        } else {
            _funcAsm.push_back(lea(entity->getMemRef(), ax(8))); 
        }
    }
}

X86Register*
X86Generator::ax(unsigned byte)
{
    auto reg = new X86Register(X86Register::RegisterType::AX, byte);
    _usedRegs.push(reg);
    return reg;
}
X86Register*
X86Generator::bx(unsigned byte)
{
    auto reg = new X86Register(X86Register::RegisterType::BX, byte);
    _usedRegs.push(reg);
    return reg;
}
X86Register*
X86Generator::cx(unsigned byte)
{
    auto reg = new X86Register(X86Register::RegisterType::CX, byte);
    _usedRegs.push(reg);
    return reg;
}
X86Register*
X86Generator::dx(unsigned byte)
{
    auto reg = new X86Register(X86Register::RegisterType::DX, byte);
    _usedRegs.push(reg);
    return reg;
}
X86Register*
X86Generator::si(unsigned byte)
{
    auto reg = new X86Register(X86Register::RegisterType::SI, byte);
    _usedRegs.push(reg);
    return reg;
}
X86Register*
X86Generator::di(unsigned byte)
{
    auto reg = new X86Register(X86Register::RegisterType::DI, byte);
    _usedRegs.push(reg);
    return reg;
}
X86Register*
X86Generator::sp()
{
    return new X86Register(X86Register::RegisterType::SP, 8);
}
X86Register*
X86Generator::bp()
{
    return new X86Register(X86Register::RegisterType::BP, 8);
}

AsmCode*
X86Generator::mov(Oper src, Oper dst)
{
    Inst* inst = new Inst("mov");
    inst->addOper(src);
    inst->addOper(dst);
    return inst;
}

AsmCode*
X86Generator::lea(IR::MemRef* ref, X86Register* dst)
{
    auto inst = new Inst("lea", "");
    inst->addOper(ref);
    inst->addOper(dst);
    return inst;
}

AsmCode*
X86Generator::add(Oper src, Oper dest)
{
    auto inst = new Inst("add");
    inst->addOper(src);
    inst->addOper(dest);
    return inst;
}

AsmCode*
X86Generator::sub(Oper src, Oper dest)
{
    auto inst = new Inst("sub");
    inst->addOper(src);
    inst->addOper(dest);
    return inst;
}

AsmCode*
X86Generator::mul(Oper src, Oper dest)
{
    auto inst = new Inst("mul");
    inst->addOper(src);
    inst->addOper(dest);
    return inst;
}

AsmCode*
X86Generator::div(Oper src, Oper dest)
{
    auto inst = new Inst("div");
    inst->addOper(src);
    inst->addOper(dest);
    return inst;
}

AsmCode*
X86Generator::shl(Oper src, Oper dest)
{
    auto inst = new Inst("shl");
    inst->addOper(src);
    inst->addOper(dest);
    return inst;

}

AsmCode*
X86Generator::shr(Oper src, Oper dest)
{
    auto inst = new Inst("shr");
    inst->addOper(src);
    inst->addOper(dest);
    return inst;
}

AsmCode*
X86Generator::neg(Oper oper)
{
    auto inst = new Inst("neg");
    inst->addOper(oper);
    return inst;
}

AsmCode*
X86Generator::nt(Oper oper)
{
    auto inst = new Inst("not");
    inst->addOper(oper);
    return inst;
}

AsmCode*
X86Generator::band(Oper src, Oper dest)
{
    auto inst = new Inst("and");
    inst->addOper(src);
    inst->addOper(dest);
    return inst;
}

AsmCode*
X86Generator::bor(Oper src, Oper dest)
{
    auto inst = new Inst("or");
    inst->addOper(src);
    inst->addOper(dest);
    return inst;
}

AsmCode*
X86Generator::bxor(Oper src, Oper dest)
{
    auto inst = new Inst("xor");
    inst->addOper(src);
    inst->addOper(dest);
    return inst;
}

AsmCode*
X86Generator::cmp(Oper src, Oper dest)
{
    auto inst = new Inst("cmp");
    inst->addOper(src);
    inst->addOper(dest);
    return inst;
}

AsmCode*
X86Generator::test(Oper src, Oper dest)
{
    auto inst = new Inst("test");
    inst->addOper(src);
    inst->addOper(dest);
    return inst;
}

AsmCode*
X86Generator::jnz(Label label)
{
    auto inst = new Inst("jnz");
    inst->addOper(Immediate(label.symbol()));
    return inst;
}

AsmCode*
X86Generator::je(Label label)
{
    auto inst = new Inst("je");
    inst->addOper(Immediate(label.symbol()));
    return inst;
}

AsmCode*
X86Generator::jmp(Label label)
{
    auto inst = new Inst("jmp", " ");
    inst->addOper(Immediate(label.symbol()));
    return inst;
}

AsmCode*
X86Generator::sete(X86Register* r)
{
    auto inst = new Inst("sete");
    inst->addOper(r);
    return inst;
}

AsmCode*
X86Generator::setne(X86Register* r)
{
    auto inst = new Inst("setne");
    inst->addOper(r);
    return inst;

}

AsmCode*
X86Generator::setl(X86Register* r)
{
    auto inst = new Inst("setl");
    inst->addOper(r);
    return inst;

}

AsmCode*
X86Generator::setle(X86Register* r)
{
    auto inst = new Inst("setle");
    inst->addOper(r);
    return inst;

}

AsmCode*
X86Generator::setg(X86Register* r)
{
    auto inst = new Inst("setg");
    inst->addOper(r);
    return inst;

}

AsmCode*
X86Generator::setge(X86Register* r)
{
    auto inst = new Inst("setge");
    inst->addOper(r);
    return inst;
}

AsmCode*
X86Generator::label(std::string label)
{
    return new Label(label);
}

AsmCode*
X86Generator::push(X86Register* r, bool virt)
{
    if (virt) {
        return _stack.push(r);
    } else {
        auto inst = new Inst("push");
        inst->addOper(r);
        r->setSize(Oper::OperSize::INT_64);
        return inst;
    }
}

AsmCode*
X86Generator::pop(X86Register* r, bool virt)
{
    if (virt) {
        return _stack.pop(r);
    } else {
        auto inst = new Inst("pop");
        inst->addOper(r);
        r->setSize(Oper::OperSize::INT_64);
        return inst;
    }
}

AsmCode*
X86Generator::call(IR::IRSymbol sym)
{
    auto inst = new Inst("call", " ");
    inst->addOper(Immediate(sym));
    return inst;
}

AsmCode*
X86Generator::ret()
{
    return new Inst("ret");
}

AsmCode* 
X86Generator::file(std::string fileName) {
    return directive(".file\t" + fileName);
}

AsmCode*
X86Generator::text() {
    return directive("\t.text");
}

AsmCode*
X86Generator::data() {
    return directive("\t.data");
}

AsmCode*
X86Generator::section(std::string name) {
    return directive("\t.section\t" + name);
}

AsmCode*
X86Generator::globl(std::string sym) {
    return directive(".globl " + sym);
}

AsmCode*
X86Generator::local(std::string sym) {
    return directive(".local " + sym);
}

AsmCode*
X86Generator::align(long n) {
    return directive("\t.align\t" + std::to_string(n));
}

AsmCode*
X86Generator::type(std::string sym, std::string type) {
    return directive("\t.type\t" + sym + "," + type);
}

AsmCode*
X86Generator::size(std::string sym, std::string size) {
    return directive("\t.size\t" + sym + "," + size);
}

AsmCode*
X86Generator::byte(long val) {
    return directive(".byte\t" + std::to_string(val));
}

AsmCode*
X86Generator::word(long val) {
    return directive(".value\t" + std::to_string(val));
}

AsmCode*
X86Generator::dw(long val) {
    return directive(".long\t" + std::to_string(val));
}

AsmCode*
X86Generator::qw(long val) {
    return directive(".quad\t" + std::to_string(val));
}

AsmCode*
X86Generator::str(std::string s) {
    return directive("\t.string\t" + s);
}
void
X86Generator::visit(IR::IRVar* var)
{
    loadVar(var, ax(var->size())); 
}

void
X86Generator::visit(IR::IRInt* val)
{
    loadValue(val, ax(val->size()));
}

void
X86Generator::visit(IR::IRStr* str)
{
    loadString(str, ax());
}

void
X86Generator::visit(IR::IRAddr* addr)
{
    loadAddr(addr);
}

void
X86Generator::visit(IR::IRMem* mem)
{
    auto expr = mem->getPtr();
    expr->accept(this); // the address expr is stored at ax
    _funcAsm.push_back(mov(memRef(ax(), 0, expr->size()), ax()));
}

void
X86Generator::visit(IR::IRCall* funcCall)
{
    // 1. System V. 
    for (int idx = 0; idx != funcCall->getParams().size(); ++idx) {
        funcCall->getParams()[idx]->accept(this);     // the param will be stored at ax
        passParam(funcCall->getParams()[idx], idx);
    }

    // 2. call funcName. The return address will be pushed automaticlly
    _funcAsm.push_back(call(funcCall->getFunc()->address()));

    // 3. after returned, the stack should be cleand 
    /*
    for (int i = 6; i < funcCall->getParams().size(); ++i) {
        _funcAsm.push_back(sub(imm(funcCall->getParams()[i]->size()), sp()));
    }
    */
}

void
X86Generator::visit(IR::IRAssign* assign)
{
    assign->getLeft()->accept(this); // will store the address at ax
    _funcAsm.push_back(push(ax(assign->getLeft()->size())));
    assign->getRight()->accept(this); // will store the rvalue at ax
    _funcAsm.push_back(pop(cx(assign->getLeft()->size())));
    _funcAsm.push_back(mov(ax(assign->size()), memRef(cx(assign->getLeft()->size()), 0, assign->size())));
}

void
X86Generator::visit(IR::IRBinExpr* expr)
{
    expr->getLeft()->accept(this);
    _funcAsm.push_back(push(ax()));
    expr->getRight()->accept(this);
    _funcAsm.push_back(pop(cx()));
    switch(expr->getOpType()) {
    case IR::IRBinExpr::BinOpType::ADD:
        _funcAsm.push_back(add((cx()), ax()));
        break;
    case IR::IRBinExpr::BinOpType::SUB:
        _funcAsm.push_back(sub((cx()), ax()));
        break;
    case IR::IRBinExpr::BinOpType::MUL:
        _funcAsm.push_back(mul((cx()), ax()));
        break;
    case IR::IRBinExpr::BinOpType::DIV:
        _funcAsm.push_back(div((cx()), ax()));
        break;
    case IR::IRBinExpr::BinOpType::MOD:
        _funcAsm.push_back(mov(Oper(Immediate(0)), dx()));
        _funcAsm.push_back(div(cx(), ax()));
        _funcAsm.push_back(mov(dx(), ax()));
        break;
    case IR::IRBinExpr::BinOpType::SHL:
        _funcAsm.push_back(shl((cx()), ax()));
        break;
    case IR::IRBinExpr::BinOpType::SHR:
        _funcAsm.push_back(shr((cx()), ax()));
        break;
    case IR::IRBinExpr::BinOpType::BITAND:
        _funcAsm.push_back(band((cx()), ax()));
        break;
    case IR::IRBinExpr::BinOpType::BITOR:
        _funcAsm.push_back(bor((cx()), ax()));
        break;
    case IR::IRBinExpr::BinOpType::BITXOR:
        _funcAsm.push_back(bxor((cx()), ax()));
        break;
    case IR::IRBinExpr::BinOpType::EQ:
        _funcAsm.push_back(cmp((cx()), ax()));
        _funcAsm.push_back(sete(ax()));
        break;
    case IR::IRBinExpr::BinOpType::NE:
        _funcAsm.push_back(cmp((cx()), ax()));
        _funcAsm.push_back(setne(ax()));
        break;
    case IR::IRBinExpr::BinOpType::LT:
        _funcAsm.push_back(cmp((cx()), ax()));
        _funcAsm.push_back(setl(ax()));
        break;
    case IR::IRBinExpr::BinOpType::LE:
        _funcAsm.push_back(cmp((cx()), ax()));
        _funcAsm.push_back(setle(ax()));
        break;
    case IR::IRBinExpr::BinOpType::GT:
        _funcAsm.push_back(cmp((cx()), ax()));
        _funcAsm.push_back(setg(ax()));
        break;
    case IR::IRBinExpr::BinOpType::GE:
        _funcAsm.push_back(cmp((cx()), ax()));
        _funcAsm.push_back(setge(ax()));
        break;
    default:
        throw UTIL::MyException("Unsupported Binary Oper");
    }
}

void
X86Generator::visit(IR::IRUniExpr* expr)
{
    expr->getExpr()->accept(this); // will store result at ax
    switch (expr->getOpType()) {
    case IR::IRUniExpr::UniOpType::NEG:
        _funcAsm.push_back(neg(ax()));
        break;
    case IR::IRUniExpr::UniOpType::BITNOT:
        _funcAsm.push_back(nt(ax()));
        break;
    case IR::IRUniExpr::UniOpType::LOGICNOT:
        _funcAsm.push_back(test(ax(), ax())); // set zf == 1, if ax == 0;
        _funcAsm.push_back(sete(ax()));
        break;
    default:
        throw UTIL::MyException("Unsupported type");
    }
}

void
X86Generator::visit(IR::IRExprStmt* stmt)
{
    stmt->getExpr()->accept(this);
}

void
X86Generator::visit(IR::IRJmp* stmt)
{
    _funcAsm.push_back(jmp(stmt->getDest()->symbol()));
}

void
X86Generator::visit(IR::IRCjmp* cjmp)
{
    cjmp->getCond()->accept(this); // will store condition value at ax
    _funcAsm.push_back(test(ax(), ax()));
    _funcAsm.push_back(jnz(cjmp->getThen()->symbol()));
    _funcAsm.push_back(jmp(cjmp->getElse()->symbol()));
}

void
X86Generator::visit(IR::IRRET* ret)
{
    if (ret->getRetValue()) {
        // if there is return value, it will be stored at ax
        ret->getRetValue()->accept(this);
    }
    _funcAsm.push_back(jmp(_epilogue));
}

void
X86Generator::visit(IR::IRLabel* l)
{
    _funcAsm.push_back(label(l->getLabel()->symbol()));
}

bool                                
X86Generator::UsedRegs::isUsed(X86Register* reg) 
{ 
    return _usedTypes.find(reg->type()) != _usedTypes.end(); 
}

void                                
X86Generator::UsedRegs::clear() 
{ 
    _regs.clear();
    _usedTypes.clear();
}

void                                
X86Generator::UsedRegs::push(X86Register* reg)
{ 
    if (!isUsed(reg)) { 
        _regs.push_back(reg);
        _usedTypes.insert(reg->type());
    }
}

X86Register*                           
X86Generator::UsedRegs::pop()
{ 
    X86Register* ret = _regs.back();
    _regs.pop_back();
    return ret;
}

unsigned
X86Generator::UsedRegs::totalSize()
{
    unsigned ret = 0;
    for (auto reg : _regs) {
        ret += reg->byte();
    }
    return ret;
}

AsmCode*    
X86Generator::VirtStack::push(X86Register* reg)
{
    auto memRef = currMemRef(reg->byte());
    auto inst = new Inst("mov");
    inst->addOper(reg);
    inst->addOper(memRef);
    _currSize += reg->byte();
    if (_currSize > _maxSize) {
        _maxSize = _currSize;
    }
    _memrefs.insert(memRef);
    return inst;
}

AsmCode*
X86Generator::VirtStack::pop(X86Register* reg)
{
    auto memRef = currMemRef(reg->byte(), true);
    auto inst = new Inst("mov");
    inst->addOper(memRef);
    inst->addOper(reg);
    _currSize -= reg->byte();
    _memrefs.insert(memRef);
    return inst;
}

void        
X86Generator::VirtStack::init()
{
    _currSize = 0;
    _maxSize = 0;
    _memrefs.clear();

}

unsigned
X86Generator::VirtStack::getAlignedSize(unsigned startAddr)
{
    unsigned rt = (startAddr + _maxSize) / StackWordSize * StackWordSize; 
    if ((startAddr + _maxSize) % StackWordSize) 
        rt += StackWordSize;
    return rt;
}

void        
X86Generator::VirtStack::redirect(unsigned offset)
{
    for (auto ref : _memrefs) {
        ref->redirect(-offset);
    }
}

IR::MemRef*
X86Generator::VirtStack::currMemRef(unsigned size, bool pop)
{
    auto baseReg = new X86Register(X86Register::RegisterType::BP, 8);
    if (pop) {
        return new IR::IndirectMemRef(baseReg, -(_currSize - size), size);
    } else {
       return new IR::IndirectMemRef(baseReg, -_currSize, size);
    }
}

}
