#include "myParser.hh"
#include "astPub.hh"
#include "../utils/Msg.hh"
#include "../utils/Exception.hh"
#include "../myPreProc/myPreProc.hh"
#include <string>
#include <queue>

namespace SYNTAX
{

SyntaxNode::SyntaxNode(TreeNode* node)
    :_isKw(false), _node(node) {}

SyntaxNode::SyntaxNode(LEX::TokenType kw)
    :_isKw(true), _kw(kw) {}

void
MyParser::setLexer(LEX::MyLexer* lexer)
{
    _lexer = lexer;
}
bool        
MyParser::hasLexer()
{
    return _lexer != nullptr;
}

bool 
MyParser::hasMoreToken()
{
    return (_currToken.getType() != LEX::TokenType::BAD) && 
           (_currToken.getType() != LEX::TokenType::FEND);
}

Program*
MyParser::getResult()
{
    return _result;
}

void
MyParser::init()
{
    _result = new Program();
    _currToken = _lexer->nextToken();
}

bool
MyParser::absorb(LEX::TokenType type)
{
    if (_currToken.getType() == type) {
        _currToken = _lexer->nextToken();
        return true;
    }
    return false;
}

void
MyParser::absorb()
{
    _currToken = _lexer->nextToken();
}

void
MyParser::errorHandler(const std::string& msg)
{
    if (msg.empty()) {
        std::string errMsg =  "Cannot absorb token(";
                    errMsg += _currToken.dump();
                    errMsg += ") when parsing AST node: ";
                    if (_symbols.top().isKeyword()) {
                        LEX::TokenType kw = _symbols.top().keywordType();
                        errMsg += _lexer->keywordStr(kw);
                    } else {
                        auto currNode = _symbols.top().getNode();
                        errMsg += _symbols.top().getNode()->getClassInfo();
                        if (dynamic_cast<BinOpExpr*>(currNode)) {
                            errMsg += " .BinOpType：" + std::to_string(dynamic_cast<BinOpExpr*>(currNode)->getBinType());
                        } else {
                           // ... 
                        }
                    }
                    errMsg += ".";
        UTIL::Msg::instance().message(UTIL::MSGTYPE::FILE_EXIST, errMsg);
    } else {
        UTIL::Msg::instance().message(UTIL::MSGTYPE::PARSE_FAIL, msg); 
    }
}

void
MyParser::exception(const std::string& str)
{
    throw UTIL::MyException(str.c_str());
}




/*
 * to extract a Statement from tokens;
 */
template <> 
Program*
MyParser::production<Program>()
{
    Program* prog = new Program();
    prog->setSrcFile(_lexer->srcFile());
    while (hasMoreToken()) {
        try {
            Statement* stmt = selectStmt();
            if (stmt->syntaxType() == SyntaxType::InclStmt) {
                InclStmt* incl = dynamic_cast<InclStmt*>(stmt);
                PPROC::MyPreProc pproc;
                auto stmts = pproc.includeFile(incl->getFilePath());
                prog->addStatements(stmts);
            } else {
                prog->addStatement(stmt);
            }
            if (!absorb(LEX::TokenType::SEMICOLON) && !stmt->isBlock() && !dynamic_cast<InclStmt*>(stmt)) {
                exception("Expected ';' after statement.");
            }
        } catch (UTIL::MyException& e) {
            exception(e.what());
        }
    }
    return prog;
}

template<>
ReturnStmt*
MyParser::production<ReturnStmt>()
{
    ReturnStmt* stmt = new ReturnStmt();
    if (absorb(LEX::TokenType::RETURN)) {
        if (!absorb(LEX::TokenType::SEMICOLON)) {
            stmt->setRetExpr(productAssign());
        }
    }
    return stmt;
}

/*
 * to create a funcDef of funcdecl from current token serial 
 */
template <> 
FuncDef*
MyParser::production<FuncDef>()
{
    FuncDef* func = new FuncDef();

    // absorb "rettype funcName ("
    func->setRetType(production<TypeNode>());

    if (_currToken.getType() != LEX::TokenType::ID) {
        throw UTIL::MyException("Expect function name");
    }
    func->setFuncName(_currToken.getValue<std::string>());
    absorb(LEX::TokenType::ID);

    // parse parameters
    if (!absorb(LEX::TokenType::PARENTHESESL)) {
        exception("parse fail");
    }
    
    bool hasParam = false;
    while (!absorb(LEX::TokenType::PARENTHESESR)) {
        if (!hasParam) {
            hasParam = true;
        } else {
            absorb(LEX::TokenType::COMMA);
        }
        if (absorb(LEX::TokenType::VARPARAMS)) {
            if (absorb(LEX::TokenType::PARENTHESESR)) {
                func->setVarArg();
                break;
            } else {
                throw UTIL::MyException("Expect ')' after var arg");
            }
        } else {
            func->addParam(production<VarDef>());
        }
    }

    
    if (_currToken.getType() == LEX::TokenType::SEMICOLON) {
        return func;

    } else if (absorb(LEX::TokenType::BRACEL)) {
        func->setDefined();
    } else {
        exception("parse fail");
    }
    
    // parse func body
    while (!absorb(LEX::TokenType::BRACER)) {
        Statement* stmt = selectStmt();
        func->addStatement(stmt);
        if (!absorb(LEX::TokenType::SEMICOLON) && !func->getStmts().back()->isBlock()) {
            exception("expect ';' after statement");
        }
    }

    return func;
}

template <>
StructDef*
MyParser::production<StructDef>()
{
    StructDef* stmt = new StructDef();
    if (!absorb(LEX::TokenType::STRUCT) || _currToken.getType() != LEX::TokenType::ID) {
        exception("parse fail");
    }
    stmt->setName(_currToken.getValue<std::string>());
    absorb(LEX::TokenType::ID);
    if (absorb(LEX::TokenType::SEMICOLON)) {
        return stmt;
    } else if (absorb(LEX::TokenType::BRACEL)) {
        stmt->setDefined();
    } else {
        exception("parse fail");
    }

    while (!absorb(LEX::TokenType::BRACER)) {
        stmt->addMember(production<VarDef>());
        if (!absorb(LEX::TokenType::SEMICOLON)) {
            exception("parse fail");
        }
    } 
    return stmt;
}

template <>
VarDef*
MyParser::production<VarDef>()
{
    VarDef* stmt = new VarDef();
    stmt->setType(production<TypeNode>());

    if (_currToken.getType() != LEX::TokenType::ID) {
        throw UTIL::MyException("Expect variable name");
    } else {
        stmt->setId(_currToken.getValue<std::string>());
        absorb(LEX::TokenType::ID);
    }
    if (absorb(LEX::TokenType::EQUAL)) {
        stmt->addInitVal(productAssign());
    }
    return stmt;

}

template <>
TypeDef*
MyParser::production<TypeDef>()
{
    TypeDef* stmt = new TypeDef();
    if (!absorb(LEX::TokenType::TYPEDEF)) {
        exception("parse fail");
    }
    stmt->setOrigType(production<TypeNode>());
    if (_currToken.getType() != LEX::TokenType::ID) {
        exception("Expect Type Name");
    } else {
        stmt->setDefinedType(_currToken.getValue<std::string>());
        absorb(LEX::TokenType::ID);
    }
    return stmt;
}

template <>
IfStmt*
MyParser::production<IfStmt>()
{
    IfStmt* ifStmt = new IfStmt();
        if (!absorb(LEX::TokenType::IF) || !absorb(LEX::TokenType::PARENTHESESL)) {
            exception("parse fail");
        }
        ifStmt->setCondition(productAssign());

        if (!absorb(LEX::TokenType::PARENTHESESR) || !absorb(LEX::TokenType::BRACEL)) {
            exception("parse fail");
        }
        while (!absorb(LEX::TokenType::BRACER)) {
            // parse then stmts
            Statement* stmt = selectStmt();
            ifStmt->addThenStmt(stmt);
            if (!absorb(LEX::TokenType::SEMICOLON) && !stmt->isBlock()) 
                exception("parse fail");
        }

        if (absorb(LEX::TokenType::ELSE) && absorb(LEX::TokenType::BRACEL)) {
            while (!absorb(LEX::TokenType::BRACER)) {
                Statement* stmt = selectStmt();
                ifStmt->addElseStmt(stmt);
                if (!absorb(LEX::TokenType::SEMICOLON) && !stmt->isBlock()) {
                    exception("parse fail");
                }
            }
        } 

    return ifStmt;
}

template <>
ForStmt*
MyParser::production<ForStmt>()
{
    ForStmt* fStmt = new ForStmt();
    // begining
    if (!absorb(LEX::TokenType::FOR) || !absorb(LEX::TokenType::PARENTHESESL)) {
        exception("parse fail");
    }

    // parse Init Expr
    bool hasInitExpr = false;
    while (!absorb(LEX::TokenType::SEMICOLON)) {
        if (hasInitExpr) {
            if(!absorb(LEX::TokenType::COMMA)) {
                exception("parse fail");
            } 
        } else {
            hasInitExpr = true;
        }
        VarDef* init = production<VarDef>();
        // todo: support vardef and assignment in init
        //Statement* init = selectStmt();
        //if (init->getType() != SyntaxType::VarDef && init->getType() != SyntaxType::BinOpExpr::ExprStmt) {
        //    exception("parse fail");
        //}
        fStmt->addInit(init);
    }

    // parse condition
    if (!absorb(LEX::TokenType::SEMICOLON)) {
        fStmt->addCondition(productAssign());
        absorb(LEX::TokenType::SEMICOLON);
    }

    // parse steps
    bool hasStepExpr = false;
    while (!absorb(LEX::TokenType::PARENTHESESR)) {
        if (hasStepExpr) {
            if(!absorb(LEX::TokenType::COMMA)) {
                exception("parse fail");
            } 
        } else {
            hasStepExpr = true;
        }
        Statement* step = selectStmt();
        fStmt->addStep(step);
    }

    if (!absorb(LEX::TokenType::BRACEL)) {
        exception("parse fail");
    }

    // parse body
    while (!absorb(LEX::TokenType::BRACER)) {
        Statement* stmt = selectStmt();
        fStmt->addBody(stmt);
        if (!absorb(LEX::TokenType::SEMICOLON) && !stmt->isBlock()) 
            exception("parse fail");
    }

    return fStmt;
}

template<>
BreakStmt*
MyParser::production<BreakStmt>()
{
    if (absorb(LEX::TokenType::BREAK)) {
        return new BreakStmt;
    } else {
        exception("parse fail");
    }
    return nullptr;
}

template<>
ContinueStmt*
MyParser::production<ContinueStmt>()
{
    if (absorb(LEX::TokenType::CONTINUE)) {
        return new ContinueStmt;
    } else {
        exception("parse fail");
    }
    return nullptr;
}

template <>
ExprStmt*
MyParser::production<ExprStmt>()
{
    ExprStmt* stmt = new ExprStmt();
    stmt->setExpr(productAssign()); 
    return stmt;
}

template<>
InclStmt*
MyParser::production<InclStmt>()
{
    InclStmt* incl = new InclStmt();
    if (!absorb(LEX::TokenType::SHARP) || !absorb(LEX::TokenType::INCLUDE) || !absorb(LEX::TokenType::LESS)) {
        throw UTIL::MyException("Parse Include Statement Error. Expected: '#include <path>'");
    }

    if (_currToken.getType() == LEX::TokenType::ID &&
        _lexer->peekToken(true).getType() == LEX::TokenType::DOT &&
        _lexer->peekToken().getType() == LEX::TokenType::ID &&
        _lexer->peekToken().getType() == LEX::TokenType::GREATER) {
        std::string filePath = _currToken.getValue<std::string>();
        absorb(LEX::TokenType::ID);
        absorb(LEX::TokenType::DOT);
        filePath = filePath + '.' + _currToken.getValue<std::string>();
        absorb(LEX::TokenType::ID);
        absorb(LEX::TokenType::GREATER);
        incl->setFilePath(filePath);
    } else {
        throw UTIL::MyException("invalid included file name");
    }
    return incl;
}

template<>
TypeNode*
MyParser::production<TypeNode>()
{
    /**
     *
     * typenode := [const] typeName (*|&)*
     *
     * typeName := void | char | bool | short | int | long | float | double | user defined type name
     *
     * */
    TypeNode* node = new TypeNode();
    bool isConst = false;
    bool useType = true;
    if (absorb(LEX::TokenType::CONST)) {
        isConst = true;
    }
    std::string type = "";
    Type* basicType = nullptr;
    TypeRef* basicRef = nullptr;
    while(true) {
        if (absorb(LEX::TokenType::VOID)) {
            if (basicType || basicRef) {
                exception("parse fail");
            }
            basicType = new BasicType(BasicType::TypeId::VOID);
        } else if (absorb(LEX::TokenType::CHAR)) {
            if (basicType || basicRef) {
                exception("parse fail");
            }
            basicType = new BasicType(BasicType::TypeId::CHAR);
        } else if (absorb(LEX::TokenType::SHORT)) {
            if (basicType || basicRef) {
                exception("parse fail");
            }
            basicType = new BasicType(BasicType::TypeId::SHORT);
        } else if (absorb(LEX::TokenType::INT)) {
            if (basicType || basicRef) {
                exception("parse fail");
            }
            basicType = new BasicType(BasicType::TypeId::INT);
        } else if (absorb(LEX::TokenType::FLOAT)) {
            if (basicType || basicRef) {
                exception("parse fail");
            }
            basicType = new BasicType(BasicType::TypeId::FLOAT);
        } else if (absorb(LEX::TokenType::DOUBLE)) {
            if (basicType || basicRef) {
                exception("parse fail");
            }
            basicType = new BasicType(BasicType::TypeId::DOUBLE);
        } else if (absorb(LEX::TokenType::STRUCT)) {
            if (basicType || basicRef) {
                exception("parse fail");
            }
            if (_currToken.getType() != LEX::TokenType::ID) {
                exception("parse fail");
            } else {
                type = _currToken.getValue<std::string>();
                absorb(LEX::TokenType::ID);
                basicRef = new TypeRef("struct " + type);
                useType = false;
            }
        } else if (_currToken.getType() == LEX::TokenType::ID) {
            if (basicType || basicRef) {
                break;
            }
            type = _currToken.getValue<std::string>();
            absorb(LEX::TokenType::ID);
            basicRef = new TypeRef(type);
            useType = false;
        } else if (absorb(LEX::TokenType::MULTI)) {
            if (basicType) {
                basicType = new PointerType(basicType);
            } else if (basicRef) {
                basicType = new PointerType(basicRef);
            } else {
                exception("parse fail");
            }
            useType = true;
        } else if (absorb(LEX::TokenType::REF)) {
            if (basicType) {
                basicType = new RefType(basicType);
            } else if (basicRef) {
                basicType = new RefType(basicRef);
            } else {
                exception("parse fail");
            }
            useType = true;
        } else {
            break;
        }
    }

    if (useType && basicType) {
        basicType->setConst(isConst);
        node->setType(basicType);
    } else if (!useType && basicRef) {
        basicRef->setConst(isConst);
        node->setTypeRef(basicRef);
    } else {
        exception("parse fail");
    }

    return node;
}

template <>
Variable*
MyParser::production<Variable>()
{
    Variable* var = new Variable();
    if (_currToken.getType() != LEX::TokenType::ID) {
        exception("parse fail");
    }
    var->setId(_currToken.getValue<std::string>());
    absorb(LEX::TokenType::ID);
    return var;
}

template <>
FuncCall*
MyParser::production<FuncCall>()
{
    FuncCall* func = new FuncCall();
    func->setFuncName(_currToken.getValue<std::string>());
    absorb(LEX::TokenType::ID);
    if (!absorb(LEX::TokenType::PARENTHESESL)) {
        exception("parse fail");
    }

    bool hasParam = false;
    while (!absorb(LEX::TokenType::PARENTHESESR)) {
        if (hasParam) {
            if (!absorb(LEX::TokenType::COMMA)) {
                exception("parse fail");
            }
        } else {
            hasParam = true;
        }

        func->addParam(productAssign());
    }
    return func;
}

template <>
StrLiteral*
MyParser::production<StrLiteral>()
{
    StrLiteral* str = new StrLiteral();
    str->setLiteral(_currToken.getValue<std::string>());
    if(!absorb(LEX::TokenType::STRLITERAL)) {
        exception("parse fail");
    }
    return str;
}

template <>
IntLiteral*
MyParser::production<IntLiteral>()
{
    IntLiteral* literal = new IntLiteral();
    literal->setLiteral(_currToken.getValue<long long>());
    if(!absorb(LEX::TokenType::INTEGERLITERAL)) {
        exception("parse fail");
    }
    return literal;
}

template <>
FloatLiteral*
MyParser::production<FloatLiteral>()
{
    FloatLiteral* literal = new FloatLiteral();
    literal->setLiteral(_currToken.getValue<float>());
    if(!absorb(LEX::TokenType::FLOATLITERAL)) {
        exception("parse fail");
    }
    return literal;
}

template <>
Parenthesed*
MyParser::production<Parenthesed>()
{
    Parenthesed* expr = new Parenthesed();
    absorb(LEX::TokenType::PARENTHESESL);
    expr->setExpr(productAssign());
    if (!absorb(LEX::TokenType::PARENTHESESR)) 
        exception("parse fail");
    return expr;
}

template <>
Keyword*
MyParser::production<Keyword>()
{
    Keyword* kw = new Keyword();
    kw->setKeyword(_lexer->keywordStr(_currToken.getType()));
    absorb();
    return kw;
}

Expr*
MyParser::productUniOp()
{
    // preOp
    if (absorb(LEX::TokenType::ADD)) {
        if (absorb(LEX::TokenType::ADD)) {
            UniOpExpr* expr = new UniOpExpr(UniOpExpr::UniOpType::PREINC);
            expr->setExpr(productUniOp()); 
            return expr;
        } 
    } else if (absorb(LEX::TokenType::MINUS)) {
        if (absorb(LEX::TokenType::MINUS)) {
            UniOpExpr* expr = new UniOpExpr(UniOpExpr::UniOpType::PREDEC);
            expr->setExpr(productUniOp()); 
            return expr;
        } else {
            UniOpExpr* expr = new UniOpExpr(UniOpExpr::UniOpType::NEG);
            expr->setExpr(productUniOp()); 
            return expr;
        }
    } else if (absorb(LEX::TokenType::BITNOT)) {
        UniOpExpr* expr = new UniOpExpr(UniOpExpr::UniOpType::BITNOT);
        expr->setExpr(productUniOp()); 
        return expr;
    } else if (absorb(LEX::TokenType::LOGNOT)) {
        UniOpExpr* expr = new UniOpExpr(UniOpExpr::UniOpType::LOGICNOT);
        expr->setExpr(productUniOp()); 
        return expr;
    } else if (absorb(LEX::TokenType::REF)) {
        UniOpExpr* expr = new UniOpExpr(UniOpExpr::UniOpType::GETADDR);
        expr->setExpr(productUniOp()); 
        return expr;
    } else if (absorb(LEX::TokenType::MULTI)) {
        UniOpExpr* expr = new UniOpExpr(UniOpExpr::UniOpType::RESOLVEADDR);
        expr->setExpr(productUniOp()); 
        return expr;
    } 

    Expr* expr = productFactor(); 

    while(isPostfix()) {
        if (absorb(LEX::TokenType::ADD) && absorb(LEX::TokenType::ADD)) {
            auto postExpr = new UniOpExpr(UniOpExpr::UniOpType::POSTINC);
            postExpr->setExpr(expr);
            expr = postExpr;
        } else if (absorb(LEX::TokenType::MINUS) && absorb(LEX::TokenType::MINUS)) {
            auto postExpr = new UniOpExpr(UniOpExpr::UniOpType::POSTDEC);
            postExpr->setExpr(expr);
            expr = postExpr;
        }
    }

    return expr;
}

Expr*
MyParser::productOr()
{
    Expr* lhs = productAnd();
    if (absorb(LEX::TokenType::LOGOR)) {
        BinOpExpr* expr = new BinOpExpr(BinOpExpr::BinOpType::OR);
        expr->setLhs(lhs);
        expr->setRhs(productOr());
        return expr;
    } else {
        return lhs;
    }
}

Expr*
MyParser::productAnd()
{
    Expr* lhs = productRel();
    if (absorb(LEX::TokenType::LOGAND)) {
        BinOpExpr* expr = new BinOpExpr(BinOpExpr::BinOpType::AND);
        expr->setLhs(lhs);
        expr->setRhs(productAnd());
        return expr;
    } else {
        return lhs;
    }
}


Expr*
MyParser::productRel()
{
    Expr* lhs = productBOR();
    if (absorb(LEX::TokenType::LESS)) {
        BinOpExpr* expr = new BinOpExpr(BinOpExpr::BinOpType::REL);
        if (absorb(LEX::TokenType::EQUAL)) {
            expr->setBinType(BinOpExpr::BinOpType::LESSE);
        } else {
            expr->setBinType(BinOpExpr::BinOpType::LESS);
        }
        expr->setLhs(lhs);
        expr->setRhs(productRel());
        return expr;
    } else if (absorb(LEX::TokenType::GREATER)) {
        BinOpExpr* expr = new BinOpExpr(BinOpExpr::BinOpType::REL);
        if (absorb(LEX::TokenType::EQUAL)) {
            expr->setBinType(BinOpExpr::BinOpType::LARGERE);
        } else {
            expr->setBinType(BinOpExpr::BinOpType::LARGER);
        }
        expr->setLhs(lhs);
        expr->setRhs(productRel());
        return expr;
    } else if (_currToken.getType() == LEX::TokenType::EQUAL &&
               _lexer->peekToken(true).getType() == LEX::TokenType::EQUAL) {
        absorb(LEX::TokenType::EQUAL);
        absorb(LEX::TokenType::EQUAL);
        BinOpExpr* expr = new BinOpExpr(BinOpExpr::BinOpType::EQUAL);
        expr->setLhs(lhs);
        expr->setRhs(productRel());
        return expr;
    } else if (absorb(LEX::TokenType::LOGNOT) && absorb(LEX::TokenType::EQUAL)) {
        BinOpExpr* expr = new BinOpExpr(BinOpExpr::BinOpType::NOTEQUAL);
        expr->setLhs(lhs);
        expr->setRhs(productRel());
        return expr;
    } else {
        return lhs;
    }
}

Expr*
MyParser::productBOR()
{
    Expr* lhs = productBXOR();
    if (absorb(LEX::TokenType::BITOR)) {
        BinOpExpr* expr = new BinOpExpr(BinOpExpr::BinOpType::BITOR);
        expr->setLhs(lhs);
        expr->setRhs(productBOR());
        return expr;
    }
    return lhs;
}

Expr*
MyParser::productBXOR()
{
    Expr* lhs = productBAND();
    if (absorb(LEX::TokenType::XOR)) {
        BinOpExpr* expr = new BinOpExpr(BinOpExpr::BinOpType::BITXOR);
        expr->setLhs(lhs);
        expr->setRhs(productBXOR());
        return expr;
    }
    return lhs;
}

Expr*
MyParser::productBAND()
{
    Expr* lhs = productShift();
    if (absorb(LEX::TokenType::REF)) {
        BinOpExpr* expr = new BinOpExpr(BinOpExpr::BinOpType::BITAND);
        expr->setLhs(lhs);
        expr->setRhs(productBAND());
        return expr;
    }
    return lhs;
}

Expr*
MyParser::productShift()
{
    Expr* lhs = productADD();
    if (_currToken.getType() == LEX::TokenType::LESS &&
        _lexer->peekToken(true).getType() == LEX::TokenType::LESS) {
        absorb(LEX::TokenType::LESS); 
        absorb(LEX::TokenType::LESS); 
        BinOpExpr* expr = new BinOpExpr(BinOpExpr::BinOpType::SHL);
        expr->setLhs(lhs);
        expr->setRhs(productShift());
        return expr;
    } else if (absorb(LEX::TokenType::GREATER)) {
        if (absorb(LEX::TokenType::GREATER)) {
            BinOpExpr* expr = new BinOpExpr(BinOpExpr::BinOpType::SHR);
            expr->setLhs(lhs);
            expr->setRhs(productShift());
            return expr;
        }
        else 
            exception("parse fail");
    } else {
        return lhs;
    }
    return nullptr; // wont get here
}

Expr*
MyParser::productADD()
{
    Expr* lhs = productMul();
    if (absorb(LEX::TokenType::ADD)) {
        BinOpExpr* expr = new BinOpExpr(BinOpExpr::BinOpType::ADD);
        expr->setLhs(lhs);
        expr->setRhs(productADD());
        return expr;
    } else if (absorb(LEX::TokenType::MINUS)) {
        BinOpExpr* expr = new BinOpExpr(BinOpExpr::BinOpType::MINUS);
        expr->setLhs(lhs);
        expr->setRhs(productADD());
        return expr;
    }
    return lhs;
}

Expr*
MyParser::productMul()
{
    Expr* lhs = productUniOp(); 
    if (absorb(LEX::TokenType::MULTI)) {
        BinOpExpr* expr = new BinOpExpr(BinOpExpr::BinOpType::MULTI);
        expr->setLhs(lhs);
        expr->setRhs(productMul());
        return expr;
    } else if (absorb(LEX::TokenType::DIV)) {
        BinOpExpr* expr = new BinOpExpr(BinOpExpr::BinOpType::DIV);
        expr->setBinType(BinOpExpr::BinOpType::DIV);
        expr->setLhs(lhs);
        expr->setRhs(productMul());
        return expr;
    } else if (absorb(LEX::TokenType::MOD)) {
        BinOpExpr* expr = new BinOpExpr(BinOpExpr::BinOpType::MOD);
        expr->setLhs(lhs);
        expr->setRhs(productMul());
        return expr;
    }
    return lhs;
}

Expr*
MyParser::productAssign()
{
    Expr* lhs = productOr();
    if (absorb(LEX::TokenType::EQUAL)) {
        BinOpExpr* expr = new BinOpExpr(BinOpExpr::BinOpType::ASSIGN);
        expr->setLhs(lhs);
        expr->setRhs(productAssign());
        return expr;
    }
    return lhs;
}

template<>
MemberExpr*
MyParser::production<MemberExpr>()
{
    MemberExpr* expr = new MemberExpr();
    if (_currToken.getType() != LEX::TokenType::ID) {
        exception("parse fail"); 
    }
    Variable* var = new Variable();
    var->setId(_currToken.getValue<std::string>()); 
    absorb(LEX::TokenType::ID);
    std::queue<std::pair<std::string, bool>> _queue;
    while(true) {
        if (absorb(LEX::TokenType::DOT)) {
            if (_currToken.getType() == LEX::TokenType::ID) {
                _queue.push({_currToken.getValue<std::string>(), true});
                absorb(LEX::TokenType::ID);
            } else {
                exception("Expect identifier after . in member expression");
            }
        } else if (absorb(LEX::TokenType::POINTER)) {
            if (_currToken.getType() == LEX::TokenType::ID) {
                _queue.push({_currToken.getValue<std::string>(), false});
                absorb(LEX::TokenType::ID);
            } else {
                exception("Expect identifier after . in member expression");
            }
        } else {
            break;
        }
    }

    // only lhs
    if (_queue.empty()) {
        expr->setExpr(var);
        return expr;
    }

    MemberExpr* curr = nullptr;
    while (!_queue.empty()) {
        MemberExpr* tmp = new MemberExpr(); 
        if (curr) {
            tmp->setExpr(curr);
        } else {
            tmp->setExpr(var);
        }
        tmp->setMember(_queue.front().first);
        tmp->setIsPointer(_queue.front().second);
        curr = tmp;
        _queue.pop();
    }
    return curr;
}

Expr*
MyParser::productFactor()
{
    LEX::TokenType tktype = _currToken.getType();
    if (tktype == LEX::TokenType::ID) {
        if (_lexer->peekToken(true).getType() == LEX::TokenType::PARENTHESESL) {
            // funcCall
            return production<FuncCall>(); 
        } else if (_lexer->peekToken(true).getType() == LEX::TokenType::DOT ||
                   _lexer->peekToken(true).getType() == LEX::TokenType::POINTER) {
            return production<MemberExpr>();
        } else {
            return production<Variable>();
        }
    } else if (_currToken.getType() == LEX::TokenType::STRLITERAL) {
        return production<StrLiteral>();
    } else if (_currToken.getType() == LEX::TokenType::INTEGERLITERAL) {
        return production<IntLiteral>();
    } else if (_currToken.getType() == LEX::TokenType::FLOATLITERAL) {
        return production<FloatLiteral>();
    } else if (_currToken.getType() == LEX::TokenType::PARENTHESESL) {
        return production<Parenthesed>();
    } else if (_lexer->keywordStr(tktype) != "") {
        return production<Keyword>();
    } else {
        exception("parse fail");
    }
    return nullptr;
}

/*
template <> 
bool MyParser::production<SelectExpr>()
{
    SelectExpr* node = dynamic_cast<SelectExpr*>(_symbols.top().getNode());
    if (node->getCondition() == nullptr) {
        BinOpExpr* cond = new BinOpExpr(BinOpExpr::BinOpType::BAD);
        node->setCondition(cond);
        cond->setStage(9);
        _symbols.push({cond, false});
    } else if (node->getCondition() && node->getTrueExpr() == nullptr) {
        if (_currToken.getType() == LEX::TokenType::QUEST) {
            absorb();
            BinOpExpr* tExpr = new BinOpExpr(BinOpExpr::BinOpType::BAD);
            tExpr->setStage(9);
            node->setTrueVal(tExpr);
            _symbols.push({tExpr, false});
            return true;
        } else {
            _symbols.pop();
            return true;
        }
    } else if (node->getCondition() && node->getTrueExpr() && node->getFalseExpr() == nullptr) {
        if (_currToken.getType() == LEX::TokenType::COLOMN) {
            absorb();
            BinOpExpr* fExpr = new BinOpExpr(BinOpExpr::BinOpType::BAD);
            fExpr->setStage(9);
            node->setFalseVal(fExpr);
            _symbols.push({fExpr, false});
            return true;
        } else {
            return false;
        } 
    } else {
        return false;
    }
}
*/

    
Statement*
MyParser::selectStmt()
{
    Statement* stmt = nullptr;
    LEX::TokenType type = _currToken.getType();
    auto func_var_def = [&]()->Statement* {
        bool firstTime = true;
        while(true) {
            LEX::TokenType tmp = _lexer->peekToken(firstTime).getType();
            firstTime = false;
            if (tmp == LEX::TokenType::SEMICOLON) break;
            if (tmp == LEX::TokenType::PARENTHESESL) {
                return production<FuncDef>();
            }
        }
        return production<VarDef>();
    };

    auto is_func_var_def = [&]()->bool {
        LEX::TokenType ttype = _lexer->peekToken(true).getType();
        if (ttype == LEX::TokenType::ID || ttype == LEX::TokenType::REF || ttype == LEX::TokenType::MULTI) {
            return true;
        }
        return false;
    }; 

    if (type == LEX::TokenType::STRUCT && _lexer->peekToken(true).getType() == LEX::TokenType::ID) {
        // struct Def
        if (_lexer->peekToken().getType() == LEX::TokenType::ID) {
            stmt = func_var_def();
        } else {
            stmt = production<StructDef>();
        }
    } else if (isType(type)) {
        // check if is function
        stmt = func_var_def();
    } else if (type == LEX::TokenType::RETURN) {
        stmt = production<ReturnStmt>();
    } else if (type == LEX::TokenType::TYPEDEF) {
        stmt = production<TypeDef>();
    } else if (type == LEX::TokenType::IF) {
        stmt = production<IfStmt>();
    } else if (type == LEX::TokenType::FOR) {
        stmt = production<ForStmt>();
    } else if (type == LEX::TokenType::BREAK) {
        stmt = production<BreakStmt>();
    } else if (type == LEX::TokenType::CONTINUE) {
        stmt = production<ContinueStmt>();
    } else if (type == LEX::TokenType::ID && is_func_var_def()) {
            stmt = func_var_def();
    } else if (type ==LEX::TokenType::SHARP) {
        stmt = production<InclStmt>();
    } else {
        stmt = production<ExprStmt>(); 
    }
    stmt->setLine(_currToken.currLine(), _currToken.currColumn());
    return stmt;
}

bool                    
MyParser::isType(LEX::TokenType token)
{
    if (token == LEX::TokenType::VOID ||
        token == LEX::TokenType::CHAR ||
        token == LEX::TokenType::SHORT ||
        token == LEX::TokenType::INT  ||
        token == LEX::TokenType::LONG ||
        token == LEX::TokenType::FLOAT ||
        token == LEX::TokenType::DOUBLE) 
    {
        return true;
    }
    return false;
}

bool                    
MyParser::isPostfix()
{
    if (_currToken.getType() == LEX::TokenType::ADD) {
        return _lexer->peekToken(true).getType() == LEX::TokenType::ADD;
    } else if (_currToken.getType() == LEX::TokenType::MINUS) {
        return _lexer->peekToken(true).getType() == LEX::TokenType::MINUS;
    }
    return false;
}

bool        
MyParser::parse()
{
    init();

    try{
       _result = production<Program>();
    } catch (UTIL::MyException err) {
        errorHandler(err.what());
        exit(-1);
    }
    return true;
}

}
