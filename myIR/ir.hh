#pragma once
#include "entity.hh"
#include "constEntry.hh"
#include "label.hh"
#include <vector>

#define PTR_SIZE 8 //x86_64

namespace IR
{

class IRNode;
class IRExpr;
class IRVar;
class IRStr;
class IRInt;
class IRMem;
class IRAddr;
class IRCall;
class IRBinExpr;
class IRUniExpr;
class IRAssign;
class IRCjmp;
class IRStmt;
class IRJmp;
class IRRET;
class IRLabel;
class IRTree;

class IRVisitor;

class IRNode {
public:
    virtual void accept(IRVisitor*) = 0;
};

class IRExpr : public IRNode {
public:
    IRExpr(unsigned size) : _size(size) {}

    virtual void        accept(IRVisitor*);

    virtual bool        isVar() { return false; }

    virtual bool        isMemRef() { return false; }

    virtual bool        isAddr() { return false; }

    unsigned            size() { return _size; }

private:
    unsigned            _size; // in bytes
};

class IRVar : public IRExpr
{
public:
    IRVar(Entity* entity) : IRExpr(entity->size()), _entity(entity) {}

    void                accept(IRVisitor*) override;

    bool                isVar() override { return true; }

    Entity*             getEntity() { return _entity; }

private:
    Entity*             _entity;
};

class IRStr : public IRExpr
{
public:
    IRStr(ConstEntry* entry) : IRExpr(PTR_SIZE), _entry(entry) {}

    void accept(IRVisitor*) override;

    ConstEntry*     getEntry() { return _entry; }

private:
    ConstEntry*     _entry;

};

class IRInt : public IRExpr 
{
public:
    IRInt(unsigned size, int val) : IRExpr(size), _value(val) {}

    void        accept(IRVisitor*) override;

    int         getValue() { return _value; }

private:
    int         _value;
};

// represent the address semantic of an entity
class IRAddr : public IRExpr 
{
public:
    IRAddr(Entity* entity) : IRExpr(PTR_SIZE), _entity(entity), _addrValue(nullptr) {}

    IRAddr(IRExpr* addrValue) : IRExpr(PTR_SIZE), _entity(nullptr), _addrValue(addrValue) {}

    void accept(IRVisitor*) override;

    bool        isAddr() override { return true; }

    IRExpr*     getValue() { return _addrValue; }

    Entity*     getEntity() { return _entity; }

private:
    Entity*     _entity;

    IRExpr*     _addrValue;
};

class IRMem : public IRExpr 
{
public:
    IRMem(unsigned size, IRExpr* ptr): IRExpr(size), _ptr(ptr) {}

    void        accept(IRVisitor*) override;

    bool        isMemRef() override { return true; }

    IRExpr*     getPtr() { return _ptr; }

private:
    IRExpr*     _ptr;
};

class IRCall : public IRExpr 
{
public:
    IRCall(Function* func, const std::vector<IRExpr*>& params) : IRExpr(func->size()), _func(func), _params(params) {}

    void                    accept(IRVisitor*) override;

    std::vector<IRExpr*>    getParams() { return _params; }

    Function*               getFunc() { return _func; }
    
private:
    Function*               _func;  // refer to the funcdef
    std::vector<IRExpr*>    _params;
};

// the _right will be loaded and moved to the _left, which represent the address
class IRAssign : public IRExpr 
{
public:
    IRAssign(IRExpr* left, IRExpr* right) : IRExpr(right->size()), _left(left), _right(right) {}

    void            accept(IRVisitor*) override;

    IRExpr*         getLeft() { return _left; }

    IRExpr*         getRight() { return _right; }

private:
    IRExpr*         _left;     // the address
    IRExpr*         _right;    // the value
};

class IRBinExpr : public IRExpr 
{
public:
    enum class BinOpType {
        ADD,
        SUB,
        MUL,
        DIV,
        MOD,
        SHL,
        SHR,
        BITAND,
        BITOR,
        BITXOR,
        EQ,
        NE,
        LT,
        LE,
        GT,
        GE,
        UNSUPPORTED
    };

    IRBinExpr(unsigned size, BinOpType op, IRExpr* left, IRExpr* right)
        : IRExpr(size), _opType(op), _left(left), _right(right) {}

    void accept(IRVisitor*) override;

    static unsigned getSize(BinOpType, unsigned leftSize);

    IRExpr*         getLeft() { return _left; }

    IRExpr*         getRight() { return _right; }

    std::string     getOpStr();

    BinOpType       getOpType() { return _opType; }

private:
    BinOpType   _opType;
    IRExpr*     _left;
    IRExpr*     _right;
};

class IRUniExpr : public IRExpr 
{
public:
    enum UniOpType {
        NEG,
        BITNOT,
        LOGICNOT,
        UNSUPPORTED,
    };

    IRUniExpr(unsigned size, UniOpType op, IRExpr* expr)
        : IRExpr(size), _opType(op), _expr(expr) {}

    void accept(IRVisitor*) override;

    IRExpr*         getExpr() { return _expr; }

    std::string     getOpStr();

    UniOpType       getOpType() { return _opType; }

    static unsigned getSize(UniOpType, unsigned leftSize);

private:
    UniOpType       _opType;
    IRExpr*         _expr;
};

class IRStmt : public IRNode {};

class IRCjmp : public IRStmt 
{
public:
    IRCjmp(IRExpr* cond, Label* l1, Label* l2)
        : _cond(cond), _then(l1), _else(l2) {}

    void            accept(IRVisitor*) override;

    IRExpr*         getCond() { return _cond; }

    Label*          getThen() { return _then; }

    Label*          getElse() { return _else; }

private:
    IRExpr*         _cond;
    Label*          _then;
    Label*          _else;
};

class IRExprStmt : public IRStmt
{
public:
    IRExprStmt(IRExpr* expr) : _expr(expr) {}

    IRExpr*     getExpr() { return _expr; }

    void        accept(IRVisitor*) override;

private:
    IRExpr*         _expr;
};

class IRJmp : public IRStmt 
{
public:
    IRJmp(Label* dest) : _dest(dest) {}

    void        accept(IRVisitor*) override;

    Label*      getDest() { return _dest; }

private:
    Label*      _dest;
};

// the retValue will be stored at the givin place: a register, a memref .etc
class IRRET : public IRStmt
{
public:
    void        addRetValue(IRExpr* value) { _retValue = value; }

    void        accept(IRVisitor*) override;

    IRExpr*     getRetValue() { return _retValue; }

private:
    IRExpr*     _retValue;
};

// will be dumped as a label in asm
class IRLabel : public IRStmt 
{
public:
    IRLabel(Label* label) : _label(label) {}

    void        accept(IRVisitor*) override;

    Label*      getLabel() { return _label; }

    std::string getSymbol() { return _label->symbol(); }

private:
    Label*  _label;
};


// the literals and (const) Variables will be dumped as a labeled entity.
// the functions will be dumped as .text instructions
class IR: public IRNode 
{
public:
    IR();
    void                    addVar(Variable* var) { _topLevelScope->addVariable(var); }

    void                    accept(IRVisitor*) override;

    void                    addConst(Constant* c) { _topLevelScope->addConstant(c); }
    void                    addFunc(Function* stmt) { _definedFunc.push_back(stmt); }
    void                    addLiteral(ConstEntry* literal) { _literals.addLiteral(literal); }

    Scope*                  getScope() { return _topLevelScope; }

    ConstTable&             getConstTable() { return _literals; }

    std::vector<Function*>  getFunctions() { return _definedFunc; }

    std::vector<Variable*>  getGlobVariable();

    std::vector<Constant*>  getGlobConst();

    std::vector<ConstEntry*>   getLiterals() { return _literals.getLiterals(); }

    void                    setSrcFile(std::string srcFile) { _srcFile = srcFile; }
    std::string             srcFile() { return _srcFile; }

private:
    std::string             _srcFile;
    Scope*                  _topLevelScope;
    ConstTable              _literals;
    std::vector<Function*>  _definedFunc;
};

}
