#pragma once

#include "../AsmGenerator.hh"
#include "X86Register.hh"
#include "../Immediate.hh"
#include <set>

#define wordSize 4

namespace ASM
{
class X86Generator : public AsmGenerator 
{
public:
    Assemble*   generate(IR::IR* program);

private:
    virtual void visit(IR::IRExpr*) {}
    virtual void visit(IR::IRVar*) ;
    virtual void visit(IR::IRInt*) ;
    virtual void visit(IR::IRStr*) ;
    virtual void visit(IR::IRAddr*) ;
    virtual void visit(IR::IRMem*) ;
    virtual void visit(IR::IRCall*) ;
    virtual void visit(IR::IRAssign*) ;
    virtual void visit(IR::IRBinExpr*) ;
    virtual void visit(IR::IRUniExpr*) ;
    virtual void visit(IR::IRExprStmt*) ;
    virtual void visit(IR::IRJmp*) ;
    virtual void visit(IR::IRCjmp*) ;
    virtual void visit(IR::IRRET*) ;
    virtual void visit(IR::IRLabel*) ;

private:
    void        generateHeader(IR::IR* root);
    void        generateDataSection(IR::IR* root);
    void        generateRODataSection(IR::IR* root);
    void        generateTextSection(IR::IR* root);

    void        declareSection(std::string section);
    void        declareData(IR::Entity*);
    void        declareString(IR::ConstEntry*);
    void        declareFunc(IR::Function*);
    AsmCode*    directive(std::string str) { return new Directive(str); }

    template <typename T>
    void        locateGlob(T* v);

    void        transFunc(IR::Function* func);
    void        passParam(IR::IRExpr* param, int idx);
    void        initFuncData(IR::Function*);
    void        locateParams(IR::Function* func);
    unsigned    locateLocalVar(IR::Scope* scope, unsigned startPos);
    void        compileBody(IR::Function*);
    void        calleeSaveRegs();
    void        preProcess(IR::Function* func, unsigned localVarSize);
    void        redirect(IR::Scope* root, unsigned offset);
    void        postProcess(IR::Function* func);
    IR::IndirectMemRef*
                memRef(X86Register* base, unsigned offset, unsigned size);
    Immediate
                imm(unsigned val);

    void        loadVar(IR::IRVar*, X86Register*);
    void        loadValue(IR::IRInt*, X86Register*);
    void        loadString(IR::IRStr*, X86Register*);
    void        loadAddr(IR::IRAddr*);
   
    X86Register*    ax(unsigned byte = 4);
    X86Register*    bx(unsigned byte = 4);
    X86Register*    cx(unsigned byte = 4);
    X86Register*    dx(unsigned byte = 4);
    X86Register*    si(unsigned byte = 4);
    X86Register*    di(unsigned byte = 4);
    X86Register*    sp();
    X86Register*    bp();

    // instructions
    // inst src dest
    AsmCode*    mov(Oper, Oper);
    AsmCode*    lea(IR::MemRef*, X86Register*);
    AsmCode*    add(Oper, Oper);
    AsmCode*    sub(Oper, Oper);
    AsmCode*    mul(Oper, Oper);
    AsmCode*    div(Oper, Oper);
    AsmCode*    shl(Oper, Oper);
    AsmCode*    shr(Oper, Oper);
    AsmCode*    neg(Oper);
    AsmCode*    nt(Oper);
    AsmCode*    band(Oper, Oper);
    AsmCode*    bor(Oper, Oper);
    AsmCode*    bxor(Oper, Oper);
    AsmCode*    cmp(Oper, Oper);
    AsmCode*    test(Oper, Oper);
    AsmCode*    jnz(Label);
    AsmCode*    je(Label);
    AsmCode*    jmp(Label);
    AsmCode*    sete(X86Register*);
    AsmCode*    setne(X86Register*);
    AsmCode*    setl(X86Register*);
    AsmCode*    setle(X86Register*);
    AsmCode*    setg(X86Register*);
    AsmCode*    setge(X86Register*);
    AsmCode*    label(std::string label);
    AsmCode*    push(X86Register*, bool virt = true);
    AsmCode*    pop(X86Register*, bool virt = true);
    AsmCode*    call(IR::IRSymbol);
    AsmCode*    ret();
    
    AsmCode*    file(std::string fileName); 
    AsmCode*    text(); 
    AsmCode*    data();
    AsmCode*    section(std::string name); 
    AsmCode*    globl(std::string);
    AsmCode*    local(std::string sym);
    AsmCode*    align(long n);
    AsmCode*    type(std::string sym, std::string type);
    AsmCode*    size(std::string sym, std::string size);
    AsmCode*    byte(long val);    // 1byte
    AsmCode*    word(long val);   // 2byte
    AsmCode*    dw(long val);      // 4byte
    AsmCode*    qw(long val);      // 8byte
    AsmCode*    str(std::string);

private:
    class UsedRegs {
    public:
        bool                                isUsed(X86Register* reg); 
        void                                clear(); 
        void                                push(X86Register* reg);
        X86Register*                        pop(); 
        std::vector<X86Register*>           getUsedRegs() { return _regs; }
        unsigned                            totalSize();
    private:
        std::vector<X86Register*>              _regs;
        std::set<X86Register::RegisterType>    _usedTypes;
    };

    class VirtStack {
    public:
        AsmCode*    push(X86Register*);
        AsmCode*    pop(X86Register*);
        void        init();
        unsigned    getAlignedSize(unsigned startAddr);
        void        redirect(unsigned offset);
        
    private:
        IR::MemRef* currMemRef(unsigned size, bool pop = false);

    private:
        std::set<IR::MemRef*>   _memrefs;
        unsigned                _currSize;
        unsigned                _maxSize;
    };

private:
    Assemble*               _asmProg;
    std::vector<AsmCode*>   _funcAsm;
    unsigned                _paramInstCnt;
    UsedRegs                _usedRegs;
    Label                   _epilogue;
    static std::vector<X86Register::RegisterType> 
                            _paramRegs; 
    VirtStack               _stack;
};

template <typename T>
void
X86Generator::locateGlob(T* obj)
{
    IR::IRSymbol symbol(obj->name());
    IR::DirectMemRef* memRef = new IR::DirectMemRef(symbol, obj->size());
    obj->setMemRef(memRef);
    obj->setAddr(symbol);
}

}
