#pragma once

#include <vector>
#include <string>
#include "../myIR/memRef.hh"
#include "Immediate.hh"
#include "AsmVisitor.hh"

namespace ASM
{
class Register;
class Oper 
{
public:
    enum OperSize {
        INT_8 = 0,
        INT_16,
        INT_32,
        INT_64
    };

public:
    Oper(Register* reg) : _reg(reg), _type(0) {}
    Oper(IR::MemRef* ref) : _ref(ref), _type(1) {}
    Oper(Immediate val) : _imm(val), _type(2)  {}
    bool            isReg() { return _type == 0; }
    bool            isMemRef() { return _type == 1; }
    bool            isImm() { return _type == 2; }
    void            setSize(OperSize size);
    OperSize        size();
    std::string     dump(); 

    static OperSize getSize(unsigned size); 
private:
    Register*       _reg;
    IR::MemRef*     _ref;
    Immediate       _imm;
    unsigned        _type;
};

/**
 *  @class AsmCode represent for an asm code.
 *      (an instruction, a phony code, a label, .etc)
 * */
class AsmCode
{
public:
    virtual void            accept(AsmVisitor* visitor) { visitor->visit(this); } 
    virtual std::string     dump() = 0;        
};

class Label: public AsmCode
{
public:
    virtual void            accept(AsmVisitor* visitor) { visitor->visit(this); } 
    Label(std::string str) : _symbol(str) {}
    Label() {}
    std::string             dump() { return _symbol; }
    std::string             symbol() { return _symbol; }
private:
    std::string             _symbol;
};

class Directive : public AsmCode {
public:
    virtual void            accept(AsmVisitor* visitor) { visitor->visit(this); } 
    Directive(std::string str) : _direcive(str) {}
    std::string             dump() { return _direcive; }
private:
    std::string _direcive;
};

class Inst: public AsmCode 
{
public:
    virtual void            accept(AsmVisitor* visitor) { visitor->visit(this); } 
    Inst(std::string str, std::string suffix = "") : _inst(str), _suffix(suffix) {}
    void                    addOper(Oper oper) {_oper.push_back(oper);}
    std::vector<Oper>       getOpers() { return _oper; }
    std::string             dump(); 
    std::string             suffix();
    std::string             suffix(Oper::OperSize size);
private:
    std::string             _inst;
    std::string             _suffix;
    std::vector<Oper>       _oper;
};

/**
 *  @class Assemble: represent for the program
 * */
class Assemble {
public:
    void                    dump(std::string file);
    
    void                    addAsmCode(AsmCode* code) { _assemblies.push_back(code); }

    void                    addAsmCode(std::vector<AsmCode*> code) { _assemblies.insert(_assemblies.end(), code.begin(), code.end()); }

    std::vector<AsmCode*>   getAsmCodes() { return _assemblies; }
private:
    std::vector<AsmCode*>   _assemblies;
};

}
