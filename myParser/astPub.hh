#pragma once

#include <vector>
#include <map>
#include <string>
#include "type.hh"
#include "Scope.hh"

/* supported grammers :
 *	 
 * Program		:=	(Statement)*
 * 
 * Statement	:=	VarDef <;>		|
 *					FuncDef <;>		|
 *					StructDef <;>   |
 *                  TypeDef         |
 *					ExprStmt<;>	    |
 *					IfStmt <;>      |
 *					ForStmt <;>		|
 *					BreakStmt <;>	|
 *					ContinueStmt <;>|
 *					InclStmt
 *
 * ExprStmt     :=  Expr;
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
 * InclStmt     :=  INCLUDE "<" PATH ">" 
 * 
 * Expr          := Assignment
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
 * Atom         :=	<id> | <Literal> | FuncCall | <(> Expr <)> | <kwyword>  
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
 * FuncCall		:= <id> <(> [Expr [,Expr]*]<)>
 *
 * donot support top-level const for now
 * TypeNode     := [const] (BaseType | TypeRef | TypePointer | TypeReference) 
 *
 * BaseType     := <void> | <char> | <short> | <int> | <float> | <double> | StructDef 
 *
 * TypeRef      := [struct] <id>
 *
 * TypePointer  := TypeNode <*> 
 *
 * TypeReference:= TypeNode <&> 
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
    BreakStmt,
    ContinueStmt,
    ExprStmt,
    InclStmt,

    // type 
    TypeNode,

    // Expr 
    AtomExpr,
    Variable,
    FuncCall,
    StrLiteral,
    IntLiteral,
    FloatLiteral,
    ParenExpr,
    Keyword,

    UniOpExpr,

    BinOpExpr,
    MemberExpr,
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
class BreakStmt;
class ContinueStmt;
class InclStmt;
class Expr;
class AtomExpr;
class FuncCall;
class Variable;
class IntLiteral;
class StrLiteral;
class UniOpExpr;
class BinOpExpr;
class MemberExpr;
//class SelectExpr;
class TypeNode;     // the node contains type info
class AstVisitor;

using StmtVec   = std::vector<Statement*>;
using ExprVec   = std::vector<Expr*>;
using VarDefVec = std::vector<VarDef*>;
using FuncVec = std::vector<FuncDef*>;
using InclStmts = std::map<std::string, StmtVec>;

class TreeNode {
public:
    virtual void            accept(AstVisitor*) = 0;

    virtual std::string     getClassInfo() = 0;

    TreeNode(SyntaxType type) : _syntaxType(type), _parent(nullptr), _scope(nullptr) {}

    virtual ~TreeNode() { if (_scope) delete _scope; } 

    SyntaxType              syntaxType() { return _syntaxType; } 

    virtual bool            isStatement() { return false; }

    virtual bool            isExpr() { return false; }

    void                    setScope(Scope* s) { _scope = s; }

    Scope*                  getScope() { return _scope; }

protected:
    SyntaxType              _syntaxType;

    TreeNode*               _parent;

    Scope*                  _scope;
};

class Program final: public TreeNode
{
public:
    void                    accept(AstVisitor*) override;

    std::string             getClassInfo() override { return "Program"; }

    Program();

    virtual ~Program();

    void                    setSrcFile(const std::string& file) { _srcFile = file; }

    std::string             srcFile() { return _srcFile; }

    void                    addStatement(Statement* stmt);

    void                    addStatements(std::vector<Statement*> stmts);

    StmtVec                 getStatements() { return _stmts; }

    StmtVec                 fetchStatements(); 

    FuncVec                 getFuncs();

    VarDefVec               getGlobalVars();

private:
    std::string             _srcFile;

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

    virtual bool            isBlock() { return false; }

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
    virtual void            accept(AstVisitor*) override;

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
    virtual void            accept(AstVisitor*) override;

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
    virtual void            accept(AstVisitor*) override;

    std::string             getClassInfo() override { return "FunDef"; }

    FuncDef();

    virtual ~FuncDef();

    bool                    isBlock() override { return true; }

    void                    setFuncName(const std::string& name) { _name = name; }

    std::string             getName() { return _name; }

    void                    setRetType(TypeNode* type) { _rtType = type; }

    TypeNode*               getRetType() { return _rtType; }

    void                    addParam(VarDef*);

    void                    addStatement(Statement* stmt);

    void                    setDefined() { _defined = true; }

    bool                    isDefined() { return _defined;}

    const VarDefVec&        getParams() { return _params; }

    const StmtVec&          getStmts() { return _stmts; }

    void                    setVarArg() { _varArg = true; }

    bool                    isVarArg() { return _varArg; }

private:
    TypeNode*               _rtType;

    std::string             _name;

    VarDefVec               _params;

    StmtVec                 _stmts;

    bool                    _defined;

    bool                    _varArg;
};


class StructDef final: public Statement 
{
public:
    virtual void            accept(AstVisitor*) override;
    
    std::string             getClassInfo() override { return "StructDef"; }

    StructDef();

    virtual ~StructDef();

    void                    setName(std::string name) { _name = name; }

    void                    addMember(VarDef*);

    void                    setDefined() { _defined = true; }

    bool                    isDefined() { return _defined;}

    std::string             getName() { return _name; }

    const VarDefVec&        getmembers() { return _members;}

    StructType*             extractType();

private:
    std::string             _name;

    VarDefVec               _members;
    
    bool                    _defined;
};


class VarDef final: public Statement 
{
public:
    virtual void            accept(AstVisitor*) override;

    std::string             getClassInfo() override { return "VarDef"; }

    VarDef();

    virtual ~VarDef();

    void                    setId(const std::string& id) { _id = id; }

    void                    setType(TypeNode* type) { _type = type; }

    void                    addInitVal(Expr* initVal);

    TypeNode*               getTypeNode() { return _type; }

    std::string             getId() { return _id; }

    Expr*                   getInit() { return _initVal; }

    unsigned                size(); 

private:
    TypeNode*               _type;

    std::string             _id;

    Expr*                   _initVal;
};

class TypeDef final: public Statement 
{
public:
    virtual void            accept(AstVisitor*) override;

    std::string             getClassInfo() override { return "TypeDef"; }

    TypeDef();

    virtual ~TypeDef();

    void                    setOrigType(TypeNode*);

    void                    setDefinedType(const std::string& id) { _id = id;}

    TypeNode*               getOrigType() { return _type; }

    std::string             getDefinedType() { return _id; }

private:
    TypeNode*               _type;

    std::string             _id;
};

class IfStmt final: public Statement 
{
public:
    virtual void            accept(AstVisitor*) override;

    std::string             getClassInfo() override { return "IfStmt"; }

    IfStmt();

    virtual ~IfStmt();

    bool                    isBlock() override { return true; }

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

class ForStmt final: public Statement 
{
public:
    virtual void            accept(AstVisitor*) override;

    std::string             getClassInfo() override { return "ForStmt"; }
    
    ForStmt();

    virtual ~ForStmt();

    bool                    isBlock() override { return true; }

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

class BreakStmt final : public Statement
{
public:
    virtual void            accept(AstVisitor* visitor) override; 

    std::string             getClassInfo() override { return "Break Statement"; }
        
    BreakStmt() : Statement(SyntaxType::BreakStmt) {}
};

class ContinueStmt final : public Statement
{
public:
    virtual void            accept(AstVisitor* visitor) override; 

    std::string             getClassInfo() override { return "Continue Statement"; }

    ContinueStmt() : Statement(SyntaxType::ContinueStmt) {}
};

class ExprStmt : public Statement
{
public:
    ExprStmt() : Statement(SyntaxType::ExprStmt), _expr(nullptr) {}

    virtual void            accept(AstVisitor* visitor) override; 

    std::string             getClassInfo() override { return "Expr Statement"; }

    void                    setExpr(Expr* expr) { _expr = expr; }

    Expr*                   getExpr() { return _expr; }

private:
    Expr*                   _expr;
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

    virtual Type*           getType() = 0; 

    virtual unsigned        size() = 0;

    virtual void            setType(Type* type) {} 

    virtual bool            isLhs() { return false; }

    // an expr is loadable when:
    //  1. it is a lhs, and 
    //  2. it is not an array
    virtual bool            isLoadable() { return false; }
    //
    // an expr is assignable when:
    //  1. it is a lhs, and 
    //  2. it is not const 
    virtual bool            isAssignale() { return false; }

};


class AtomExpr : public Expr
{
public:
    AtomExpr(SyntaxType type) :Expr(type) {}

};

class Variable : public AtomExpr 
{
public:
    virtual void            accept(AstVisitor*) override;

    std::string             getClassInfo() override { return "Variable"; }

    Variable() : AtomExpr(SyntaxType::Variable) {}

    void                    setId (const std::string& id) { _id = id; }

    std::string             getId() { return _id; }

    void                    setVarDef(VarDef* defVar) { _defVar = defVar; }

    VarDef*                 getDefine() { return _defVar; }

    bool                    isLhs() override { return true; }

    bool                    isAssignale() override; // depend on type  

    bool                    isLoadable() override; 

    Type*                   getType() override;

    unsigned                size() override { return _defVar->size(); } 

public:
    std::string             _id;

    VarDef*                 _defVar;
};

class FuncCall : public AtomExpr 
{
public:
    virtual void            accept(AstVisitor*) override;

    std::string             getClassInfo() override { return "FuncCall"; }

    FuncCall() : AtomExpr(SyntaxType::FuncCall) {}

    ~FuncCall();

    void                    addParam(Expr*);

    void                    addParams(ExprVec*);

    const ExprVec&          getParams() { return _params; }

    void                    setFuncName(const std::string& name) { _funcName = name; }

    std::string             funcName() { return _funcName; }

    void                    setFuncDef(FuncDef* def) { _defFunc = def; }

    FuncDef*                getFuncDef() { return _defFunc; }

    Type*                   getType() override;

    unsigned                size() override;

private:
    std::string             _funcName;

    ExprVec                 _params;

    FuncDef*                _defFunc;
};

class StrLiteral : public AtomExpr 
{
public:
    virtual void            accept(AstVisitor*) override;

    std::string             getClassInfo() override { return "StrLiteral"; }

    StrLiteral() : AtomExpr(SyntaxType::StrLiteral) {}

    void                    setLiteral(std::string liter) { _strLiteral = liter; }

    std::string             getLiteral() { return _strLiteral; }

    Type*                   getType() override; 

    unsigned                size() override { return getType()->size(); }

private:
    std::string             _strLiteral;
};

class IntLiteral : public AtomExpr 
{
public:
    virtual void            accept(AstVisitor*) override;

    std::string             getClassInfo() override { return "IntLiteral"; }

    IntLiteral() : AtomExpr(SyntaxType::IntLiteral) {}

    void                    setLiteral(int liter) { _intLiteral = liter; }

    int                     getLiteral() { return _intLiteral; }

    Type*                   getType() override { return TypeTable::getType("int"); } 

    unsigned                size() override { return getType()->size(); }

private:
    int                     _intLiteral;

};

class FloatLiteral : public AtomExpr 
{
public:
    virtual void            accept(AstVisitor*) override;

    std::string             getClassInfo() override { return "FloatLiteral"; }

    FloatLiteral() : AtomExpr(SyntaxType::FloatLiteral) {}

    void                    setLiteral(float liter) { _literal = liter; }

    Type*                   getType() override { return TypeTable::getType("float"); } 

    float                   getLiteral() { return _literal; }

    unsigned                size() override { return getType()->size(); } 

private:
    float                   _literal;

};

class Parenthesed : public AtomExpr 
{
public:
    virtual void            accept(AstVisitor*) override;

    std::string             getClassInfo() override { return "Parenthesed Expr"; }

    Parenthesed() : AtomExpr(SyntaxType::ParenExpr) {}

    void                    setExpr(Expr* expr) { _parenthesedExpr = expr; }

    Expr*                   getExpr() { return _parenthesedExpr; }

    Type*                   getType() override { return _parenthesedExpr->getType(); }

    unsigned                size() override { return _parenthesedExpr->size(); }

private:
    Expr*                   _parenthesedExpr;
};

class Keyword: public AtomExpr
{
public:
    virtual void            accept(AstVisitor*) override;

    std::string             getClassInfo() override { return "Keyword"; }

    Keyword() : AtomExpr(SyntaxType::Keyword) {}

    void                    setKeyword(const std::string kw) { _kw = kw; }

    std::string             getKeyword() { return _kw; }

    Type*                   getType() override { return nullptr; }

    unsigned                size() override { return 0; }

private:
    std::string             _kw;
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

    virtual void            accept(AstVisitor*) override;

    std::string             getClassInfo() override { return "UniOpExpr"; }

    UniOpExpr(UniOpType);

    virtual ~UniOpExpr();

    void                    setType(UniOpType type) { _opType = type; }

    UniOpType               getUniType() { return _opType; }

    void                    setExpr(Expr* expr) {_expr = expr;}

    Expr*                   getExpr() { return _expr; }

    bool                    isLhs() override; 

    bool                    isAssignale() override; 

    bool                    isLoadable() override; 

    Type*                   getType() override;

    unsigned                size() override { return getType()->size(); }

private:
    UniOpType               _opType;

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
    virtual void            accept(AstVisitor*) override;

    std::string             getClassInfo() override { return "BinOpExpr"; }

    BinOpExpr(BinOpType type): Expr(SyntaxType::BinOpExpr), _opType(type) {}

    BinOpExpr():Expr(SyntaxType::BinOpExpr), _opType(BAD) {}

    virtual ~BinOpExpr();

    void                    setLhs(Expr* lhs) {_left = lhs; }

    void                    setRhs(Expr* rhs) { _right = rhs; }

    Expr*                   getLhs() { return _left; }

    Expr*                   getRhs() { return _right; }

    void                    setBinType(BinOpType type) { _opType = type; } 

    BinOpType               getBinType() { return _opType; }

    Type*                   getType() override;

    unsigned                size() override { return getType()->size(); }

private:
    BinOpType               _opType;

    Expr*                   _left;

    Expr*                   _right;
};

class MemberExpr : public Expr
{
public:
    MemberExpr() : Expr(SyntaxType::MemberExpr) {}

    virtual void            accept(AstVisitor*) override;

    std::string             getClassInfo() override { return "Member Expr"; }

    virtual ~MemberExpr();

    void                    setExpr(Expr* expr) { _expr = expr;}

    Expr*                   getExpr() { return _expr; }

    void                    setMember(const std::string& member) { _member = member; }

    std::string             getMember() { return _member; }

    void                    setIsPointer(bool b) { _isPointer = b; }

    bool                    isPointer() { return _isPointer; }

    Type*                   getBasicType();

    Type*                   getMemberType();

    bool                    isLhs() override { return true; } 

    Type*                   getType() override;

    unsigned                size() override { return getType()->size(); }

private:
    Expr*                   _expr;

    std::string             _member;

    bool                    _isPointer;
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
    virtual void            accept(AstVisitor*) override;

    TypeNode() :TreeNode(SyntaxType::TypeNode) {}

    TypeNode(Type* type) :TreeNode(SyntaxType::TypeNode), _type(type) {}

    TypeNode(TypeRef* ref) : TreeNode(SyntaxType::TypeNode), _typeRef(ref) {}

    std::string             getClassInfo() override { return "TypeNode"; }

    bool                    isResolved(); 

    void                    setType(Type* type) { _type = type; }

    Type*                   getType() { return _type; }

    void                    setTypeRef(TypeRef* ref) { _typeRef = ref; }

    TypeRef*                getTypeRef() { return _typeRef; }

protected:
    Type*                   _type;

    TypeRef*                _typeRef;
};

}
