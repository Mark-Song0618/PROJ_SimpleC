#pragma once

#include <vector>
#include <map>
#include <string>
#include "Scope.hh"

/* supported grammers :
 *	 
 * Program		:=	(Statement)*
 * 
 * Statement	:=	VarDef <;>		|
 *					FuncDef <;>		|
 *					StructDef <;>   |
 *                  TypeDef         |
 *					FuncCall <;>	|
 *					Assignment <;>	|
 *					IfStmt <;>      |
 *					ForStmt <;>		|
 *					InclStmt
 *
 * FuncDef		:=	TypeNode <id> <(> [VarDef (<,> VarDef)*] <)> <{> [Statement]* <}>
 * 
 * StructDef	:=	<Struct> <id> <{> [VarDef]* <}>
 * 
 * VarDef		:=	TypeNode <id> [ <=> Expr ]
 *
 * TypeDef      :=  <typedef> TypeNode <id>
 * 
 * IfStmt       :=	<if> <(> Expr <)> <{> [Statement]* <}>
 * 
 * ForStmt      :=	<for> <(> (VarDef) <;> Expr <;> [Statement]* <)> <{> [Statement]* <}>
 *
 * FuncCall		:= <id> <(> [Expr [,Expr]*]<)>
 *
 * InclStmt     :=  INCLUDE "<" PATH ">" 
 * 
 * AssignExpr   := Assignment
 *
 * Assignment	:= Expr9 [ <=> AssignExpr]
 *
 *  we donot support ? expr.
 *  It is easy to convert ?expr to if stmt
 * (Expr10       := Expr9 [<?> Expr9 <:> Expr9])
 *
 * Expr9        := Expr8 [<||> Expr9]
 *
 * Expr8        := Expr7 [<&&> Expr8]*
 *
 * Expr7        := Expr6 [<RelOp> Expr7]
 *
 * Expr6        := Expr5 [<|> Expr6]*
 *
 * Expr5        := Expr4 [<^> Expr5]*
 * 
 * Expr4        := Expr3 [<&> Expr4]*
 *
 * Expr3        := Expr2 [shiftOp Expr3]*
 *
 * Expr2        := Expr1 [AddOp Expr2]*
 *
 * Expr1        := Expr0 [MulOp Expr1]*
 *
 * Expr0        := [preOp] Factor 
 *               | Factor[postOp] 
 *
 * Factor       := Atom [MembOp Factor]*
 *
 * Atom         :=	<id> | <Literal> | FuncCallExpr | <(> Expr <)> | <kwyword>  
 *
 * RelOp        := ">" | "<" | ">=" | <<=> | "==" | "!="
 * 
 * shiftOp      := "<<" | ">>"
 *
 * AddOp        := <+> | <->
 *
 * MulOp        := <*> | </> | <%>]
 *
 * PreOp        := <++> | <--> | <!> | <~> | <-> | <*> | <&>
 *
 * PostOp       := <++> | <-->
 *
 * MembOp       := "->" | <.>
 *
 * TypeNode     := BaseType | TypeRef | TypePointer | ConstType | Reference 
 *
 * BaseType     := <void> | <char> | <short> | <int> | <float> | <double> | StructDef 
 *
 * TypeRef      := [struct] <id>
 *
 * TypePointer  := <*> TypeNode 
 *
 * ConstType    := <const> TypeNode 
 *
 * Reference    := <&> TypeNode 
 *
 */

namespace SYNTAX
{

enum class SyntaxType
{
    Program,    // represent for a compile unit

    // Statements
    RetStmt,
    FuncDef,
    StructDef,
    VarDef, 
    TypeDef,
    IfStmt,
    ForStmt,
    Assignment,
    FuncCall,
    InclStmt,

    // type 
    TypeNode,

    // Expr 
    AtomExpr,
    UniOpExpr,
    BinOpExpr,
    //SelectExpr,

    Dummy
};

class Program;
class Statement;
class ReturnStmt;
class FuncDef;
class StructDef;
class VarDef;
class TypeDef;
class IfStmt;
class ForStmt;
class Assignment;
class FuncCall;
class InclStmt;
class Expr;
class AtomExpr;
class UniOpExpr;
class BinOpExpr;
//class SelectExpr;
class TypeNode;     // the node contains type info
class AstVisitor;

using StmtVec   = std::vector<Statement*>;
using ExprVec   = std::vector<Expr*>;
using VarDefVec = std::vector<VarDef*>;
using InclStmts = std::map<std::string, StmtVec>;

class TreeNode {
public:
    virtual void            accept(AstVisitor*) = 0;

    virtual std::string     getClassInfo() = 0;

    TreeNode(SyntaxType type) : _type(type), _parent(nullptr), _scope(nullptr), _stage(0) {}

    virtual ~TreeNode() { if (_scope) delete _scope; } 

    SyntaxType              type(); 

    virtual bool            isStatement() { return false; }

    virtual bool            isExpr() { return false; }

    int                     currStage() { return _stage; }

    void                    setStage(int stage) { _stage = stage; }

    void                    setScope(Scope* s) { _scope = s; }

protected:
    SyntaxType              _type;

    TreeNode*               _parent;

    Scope*                  _scope;

    int                     _stage;
};

class Program final: public TreeNode
{
public:
    void                    accept(AstVisitor*) override;

    std::string             getClassInfo() override { return "Program"; }

    Program();

    virtual ~Program();

    void                    addStatement(Statement* stmt);

    StmtVec                 getStatements() { return _stmts; }

    StmtVec                 fetchStatements(); 

private:
    StmtVec                 _stmts;

    InclStmts               _includeFiles; 
};

/********************************************************************************
                        Statements Syntax Nodes
********************************************************************************/
class Statement : public TreeNode
{
public:
    Statement(SyntaxType type) : TreeNode(type), _line(0), _col(0) {};

    virtual ~Statement() {}

    bool                    isStatement() override { return true; }

    void                    setLine(int line, int col) { _line = line; _col = col;}

    size_t                  getLine() { return _line; }

    size_t                  getCol() { return _col; }

private:
    size_t                  _line;
    size_t                  _col;
};

// InclStmt will be process by preporcessor before syntax check
class InclStmt : public Statement 
{
public:
    virtual void            accept(AstVisitor*);

    std::string             getClassInfo() override { return "InclStmt"; }

    InclStmt() : Statement(SyntaxType::InclStmt), _path("") {}

    void                    setFilePath(const std::string path) { _path = path; }

    std::string             getFilePath() { return _path;}


private:
    std::string             _path;
};

class ReturnStmt : public Statement
{
public:
    virtual void            accept(AstVisitor*);

    std::string             getClassInfo() override { return "ReturnStmt"; }

    ReturnStmt() : Statement(SyntaxType::RetStmt) {}

    void                    setRetExpr(Expr* rt) { _rtExpr = rt; }

    Expr*                   getRetExpr() { return _rtExpr; }

private:
    Expr*                   _rtExpr;
};

class FuncDef final: public Statement 
{
public:
    virtual void            accept(AstVisitor*);

    std::string             getClassInfo() override { return "FunDef"; }

    FuncDef();

    virtual ~FuncDef();

    void                    setId(AtomExpr* id) { _id = id; }

    void                    setType(TypeNode* type) { _rtType = type; }

    void                    addParam(VarDef*);

    void                    addStatement(Statement* stmt);

    void                    setDefined() { _defined = true; }

    bool                    isDefined() { return _defined;}

    TypeNode*               getRtType() { return _rtType; }

    AtomExpr*               getId() { return _id; }

    const VarDefVec&        getParams() { return _params; }

    const StmtVec&          getStmts() { return _stmts; }

private:
    TypeNode*               _rtType;

    AtomExpr*               _id;

    VarDefVec               _params;

    StmtVec                 _stmts;

    bool                    _defined;
};


class StructDef final: public Statement 
{
public:
    virtual void            accept(AstVisitor*);
    
    std::string             getClassInfo() override { return "StructDef"; }

    StructDef();

    virtual ~StructDef();

    void                    setId(AtomExpr* id) { _id = id; }

    void                    addMember(VarDef*);

    void                    setDefined() { _defined = true; }

    bool                    isDefined() { return _defined;}

    AtomExpr*               getId() { return _id; }

    const VarDefVec&        getmembers() { return _members;}

private:
    AtomExpr*               _id;

    VarDefVec               _members;
    
    bool                    _defined;
};


class VarDef final: public Statement 
{
public:
    virtual void            accept(AstVisitor*);

    std::string             getClassInfo() override { return "VarDef"; }

    VarDef();

    virtual ~VarDef();

    void                    setId(AtomExpr* id) { _id = id; }

    void                    setType(TypeNode* type) { _type = type; }

    void                    addInitVal(Expr* initVal);

    TypeNode*               getTypeNode() { return _type; }

    AtomExpr*               getId() { return _id; }

    Expr*                   getInit() { return _initVal; }

private:
    TypeNode*               _type;

    AtomExpr*               _id;

    Expr*                   _initVal;
};

class TypeDef final: public Statement 
{
public:
    virtual void            accept(AstVisitor*);

    std::string             getClassInfo() override { return "TypeDef"; }

    TypeDef();

    virtual ~TypeDef();

    void                    setOrigType(TypeNode*);

    void                    setdefinedType(AtomExpr* id) { _id = id;}

    TypeNode*               getOrigType() { return _type; }

    AtomExpr*               getDefinedType() { return _id; }

private:
    TypeNode*               _type;

    AtomExpr*               _id;
};

class IfStmt final: public Statement 
{
public:
    virtual void            accept(AstVisitor*);

    std::string             getClassInfo() override { return "IfStmt"; }

    IfStmt();

    virtual ~IfStmt();

    void                    setCondition(Expr*);

    void                    addThenStmt(Statement* stmt);

    void                    addElseStmt(Statement* stmt);

    Expr*                   getCondition() { return _condition; }

    StmtVec                 getStatements() { return _stmts; }

    StmtVec                 getElseStmts() { return _elseStmts; }

    void                    setIfScope(Scope* s) { _iScope = s; }

    void                    setElseScope(Scope* s) { _eScope= s; }
    
private:
    Expr*                   _condition;

    StmtVec                 _stmts;

    StmtVec                 _elseStmts;

    Scope*                  _iScope;

    Scope*                  _eScope;
};

class AssignStmt final: public Statement 
{
public:
    virtual void            accept(AstVisitor*);

    std::string             getClassInfo() override { return "AssignStmt"; }

    AssignStmt();

    virtual ~AssignStmt();

    void                    addLhs(Expr*);

    void                    addRhs(Expr*);

    Expr*                   getLhs() { return _left; }

    Expr*                   getRhs() { return _right; }

private:
    Expr*                   _left;

    Expr*                   _right;
};

class ForStmt final: public Statement 
{
public:
    virtual void            accept(AstVisitor*);

    std::string             getClassInfo() override { return "ForStmt"; }
    
    ForStmt();

    virtual ~ForStmt();

    void                    addInit(VarDef*);

    void                    addCondition(Expr*); 

    void                    addStep(Statement*);

    void                    addBody(Statement*);

    const VarDefVec&        getInit() { return _initDefs; }

    Expr*                   getCondition() { return _condition; }

    const StmtVec&          getSteps() { return _steps; }

    const StmtVec&          getBody() { return _body; }

private:
    VarDefVec               _initDefs;

    Expr*                   _condition;

    StmtVec                 _steps;

    StmtVec                 _body;
};

class FuncCall final: public Statement 
{
public:
    virtual void            accept(AstVisitor*);

    std::string             getClassInfo() override { return "FuncCall"; }

    FuncCall();

    virtual ~FuncCall();

    void                    setName(const std::string&);

    std::string             getFuncName() { return _funcName; }

    void                    addParam(Expr*);

    const ExprVec&          getParams() { return _params; }

private:
    std::string             _funcName;

    ExprVec                 _params;
};

/********************************************************************************
                        Expression Syntax Nodes
********************************************************************************/

/*
 *      ExprN -> StageN
 */
class Expr: public TreeNode
{
public:
    Expr(SyntaxType type) : TreeNode(type) {}

    virtual ~Expr() {}

    bool                    isExpr() override { return true; }
};


class AtomExpr: public Expr
{
public:
    enum class AtomType
    {
        Variable,
        FuncCall,
        StrLiteral,
        IntLiteral,
        FloatLiteral,
        Parenthesed,
        Keyword,
        Bad,
    };

public:
    virtual void            accept(AstVisitor*);

    std::string             getClassInfo() override { return "AtomExpr"; }

    AtomExpr() : Expr(SyntaxType::AtomExpr), _AtomType(AtomType::Bad), _defVar(nullptr), _defFunc(nullptr) {}

    AtomExpr(AtomType type) :Expr(SyntaxType::AtomExpr), _AtomType(type), _defVar(nullptr), _defFunc(nullptr) {}

    virtual ~AtomExpr();

    void                    setVar(const std::string& id) { _id = id; }

    void                    setKeyword(const std::string kw) { _kw = kw; }

    void                    setFuncCall(FuncCall* func) { _func = func; }

    void                    setLiteral(long long liter) { _intLiteral = liter; }

    void                    setLiteral(float liter) { _floatLiteral = liter; }

    void                    setLiteral(std::string liter) { _strLiteral = liter; }

    void                    setExpr(Expr* expr) { _parenthesedExpr = expr; }

    void                    setType(AtomType type) { _AtomType = type; }

    AtomType                getAtomType() { return _AtomType; }

    FuncCall*               getFuncCall() { return _func; }

    Expr*                   getParenthesed() { return _parenthesedExpr; }

    std::string             getIdName() { return _id; }

    std::string             getString() { return _strLiteral; }

    float                   getFloat() { return _floatLiteral; }

    long long               getInt() { return _intLiteral; }

    std::string             getKeyword() { return _kw; }

    // resolved var
    void                    setVarDef(VarDef* def) { _defVar = def; }

    VarDef*                 getVarDef() { return _defVar; }

    // resolved func
    void                    setFuncDef(FuncDef* def) { _defFunc = def; }

    FuncDef*                getFuncDef() { return _defFunc; }

private:
    AtomType                _AtomType;
    std::string             _id;
    std::string             _strLiteral;
    long long               _intLiteral;
    float                   _floatLiteral;
    FuncCall*               _func;
    Expr*                   _parenthesedExpr;
    std::string             _kw;
    VarDef*                 _defVar;
    FuncDef*                _defFunc;
};

class UniOpExpr: public Expr
{
public:
    enum UniOpType {
        PREINC,
        POSTINC,
        PREDEC,
        POSTDEC,
        NEG,
        BITNOT,         // ~
        LOGICNOT,       // !
        GETADDR,
        RESOLVEADDR,
        NONE,
        BAD
    };

    virtual void            accept(AstVisitor*);

    std::string             getClassInfo() override { return "UniOpExpr"; }

    UniOpExpr(UniOpType);

    virtual ~UniOpExpr();

    void                    setType(UniOpType type) { _type = type; }

    UniOpType               getUniType() { return _type; }

    void                    setFactor(Expr* factor) {_expr = factor;}

    Expr*                   getFactor() { return _expr; }

private:
    UniOpType               _type;

    Expr*                   _expr;
};

class BinOpExpr: public Expr
{
public:
    enum BinOpType {
        REL,
            LESS,
            LARGER,
            LESSE,
            LARGERE,
            EQUAL,
            NOTEQUAL,

        AND,
        OR,
        BITOR,
        BITAND,
        BITXOR,
        MEMBER,
            DOT,
            POINTER,
        SHIFT,
            SHR,
            SHL,
        ADD,
        MINUS,
        MULTI,
        DIV,
        MOD,
        ASSIGN,
        BAD
    };

public:
    virtual void            accept(AstVisitor*);

    std::string             getClassInfo() override { return "BinOpExpr"; }

    BinOpExpr(BinOpType type): Expr(SyntaxType::BinOpExpr), _type(type) {}

    BinOpExpr():Expr(SyntaxType::BinOpExpr), _type(BAD) {}

    virtual ~BinOpExpr();

    void                    setLhs(Expr* lhs) {_left = lhs; }

    void                    setRhs(Expr* rhs) { _right = rhs; }

    Expr*                   getLhs() { return _left; }

    Expr*                   getRhs() { return _right; }

    void                    setBinType(BinOpType type) { _type = type; } 

    BinOpType               getBinType() { return _type; }

private:
    BinOpType               _type;

    Expr*                   _left;

    Expr*                   _right;
};

/*
class SelectExpr: public Expr
{
public: 
    SelectExpr();

    void                    setCondition(Expr*);

    void                    setTrueVal(Expr*);

    void                    setFalseVal(Expr*);

    Expr*                   getCondition() { return _condition; }

    Expr*                   getTrueExpr() { return _true; }

    Expr*                   getFalseExpr() { return _false; }

private:
    Expr*                   _condition;
    
    Expr*                   _true;

    Expr*                   _false;
};
*/

/********************************************************************************
                        Type Syntax Nodes
********************************************************************************/
class TypeNode: public TreeNode
{
public:
    enum class TypeId {
        VOID,
        BOOL,
        CHAR,
        SHORT,
        INT,
        FLOAT,
        DOUBLE,
        TypeRef,

        Ref,    // Type&
        Pointer,// Type*

        EMPTY
    };

public:
    virtual void            accept(AstVisitor*);

    std::string             getClassInfo() override { return "TypeNode"; }

    virtual ~TypeNode() {if (_baseType) delete _baseType; }

    TypeNode() : TreeNode(SyntaxType::TypeNode), _typeid(TypeId::EMPTY), _baseType(nullptr) ,_topConst(false), _lowConst(false) {};

    TypeNode(TypeId id) : TreeNode(SyntaxType::TypeNode), _typeid(id), _baseType(nullptr) ,_topConst(false), _lowConst(false) {};

    TypeNode(const TypeNode& node) : TreeNode(SyntaxType::TypeNode), _typeid(node._typeid), _baseType(node._baseType) {};

    void                    setType(TypeId type) { _typeid = type; }

    void                    setBaseType(TypeNode* base) { _baseType = base; }

    void                    setTypeRef(const std::string& ref) { _typeid = TypeId::TypeRef; _refName = ref; }

    std::string             getTypeRefName() { return _refName; }

    void                    setConst(bool top = false);

    TypeId                  getTypeId();

    bool                    isBasic() { return _baseType == nullptr; } 

    bool                    isTopConst() { return _topConst; }

    bool                    isLowConst() { return _lowConst; }

    TypeNode*               getBasicType(bool resolveOnce = false);

    TypeNode*               getReferedType(bool resolveOnce = false);

    void                    resolvedType(StructDef* def) { _typeDef = def; }

    StructDef*              getTypeDef() { return _typeDef; }

protected:
    TypeNode*               _baseType;

    // before type resolving, typenode only has a _refName
    // after resolving, It has a pointer to the typeDef
    std::string             _refName;
    StructDef*              _typeDef;

    TypeId                  _typeid;

    bool                    _lowConst;

    bool                    _topConst;
};

}
