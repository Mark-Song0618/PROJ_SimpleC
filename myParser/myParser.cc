#include "myParser.hh"
#include "astPub.hh"
#include "../utils/Msg.hh"
#include "../utils/Exception.hh"
#include <string>

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
    _symbols.push(_result);
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
bool 
MyParser::production<Program>()
{
    Program* prog = dynamic_cast<Program*>(_symbols.top().getNode());
    if (!hasMoreToken()) {
        _symbols.pop();
        return true;
    } else {
        if (!absorb(LEX::TokenType::SEMICOLON) && !prog->getStatements().empty()) {
            auto stmt = prog->getStatements().back();
            if (!dynamic_cast<FuncDef*>(stmt) &&
                !dynamic_cast<IfStmt*>(stmt) &&
                !dynamic_cast<ForStmt*>(stmt) &&
                !dynamic_cast<InclStmt*>(stmt)) {
                exception("Expected ';' after statement.");
            }
        }
        Statement* stmt = selectStmt();
        prog->addStatement(stmt);
        _symbols.push(stmt);
    }
    return true;
}

template<>
bool
MyParser::production<ReturnStmt>()
{
    ReturnStmt* rt = dynamic_cast<ReturnStmt*>(_symbols.top().getNode());
    if (absorb(LEX::TokenType::RETURN)) {
        _symbols.pop();
        if (!absorb(LEX::TokenType::SEMICOLON)) {
            Expr* assign = new BinOpExpr(BinOpExpr::BinOpType::ASSIGN);
            rt->setRetExpr(assign);
            _symbols.push(assign);
        }
        return true;
    }
    return false;
}

/*
 * to create a funcDef of funcdecl from current token serial 
 */
template <> 
bool 
MyParser::production<FuncDef>()
{
    FuncDef* func = dynamic_cast<FuncDef*>(_symbols.top().getNode());
    LEX::TokenType tktype = _currToken.getType();
    if (func->currStage() == 0) {
        // absorb "rettype funcName ("
        TypeNode* type = new TypeNode(TypeNode::TypeId::EMPTY);
        AtomExpr* id = new AtomExpr(AtomExpr::AtomType::Variable);
        _symbols.push(id);
        _symbols.push(type);
        func->setId(id);
        func->setType(type);
        func->setStage(1);
        return true;
    } else if (func->currStage() == 1) {
        // absorb "param defs )"
        if (!absorb(LEX::TokenType::PARENTHESESL)) {
            exception("parse fail");
        }
        if (absorb(LEX::TokenType::PARENTHESESR)) {
            if (_currToken.getType() == LEX::TokenType::SEMICOLON) {
                _symbols.pop();
            } else if (absorb(LEX::TokenType::PARENTHESESL)) {
                func->setDefined();
                func->setStage(3); 
            }
        } else {
            VarDef* param = new VarDef();
            _symbols.push(param);
            func->addParam(param);
            func->setStage(2);
        } 
        return true;
    } else if (func->currStage() == 2) {
        if (absorb(LEX::TokenType::COMMA)) {
            VarDef* param = new VarDef();
            _symbols.push(param);
            func->addParam(param);
        } else if (absorb(LEX::TokenType::PARENTHESESR)) {
            if (_currToken.getType() == LEX::TokenType::SEMICOLON) {
                _symbols.pop();
            } else if (absorb(LEX::TokenType::BRACEL)) {
                func->setDefined();
                func->setStage(3); 
            }
        } else {
            exception("parse fail");
        }
    } else if (func->currStage() == 3) {
        // absorb "stmts}"
        if (!absorb(LEX::TokenType::SEMICOLON)) {
            if (!func->getStmts().empty()) {
                auto lastStmt = func->getStmts().back();
                if (!dynamic_cast<FuncDef*>(lastStmt) &&
                    !dynamic_cast<IfStmt*>(lastStmt) &&
                    !dynamic_cast<ForStmt*>(lastStmt)) {
                    exception("parse fail");
                }
            }
        }
        if (absorb(LEX::TokenType::BRACER)) {
            _symbols.pop();
        } else {
            Statement* stmt = selectStmt();
            func->addStatement(stmt);
            _symbols.push(stmt);
        }
    } else {
        exception("parse fail");
    }

    return true;
}

template <>
bool MyParser::production<StructDef>()
{
    StructDef* node = dynamic_cast<StructDef*>(_symbols.top().getNode());
    if (node->currStage() == 1) {
        if (!absorb(LEX::TokenType::SEMICOLON)) {
            if (!node->getmembers().empty())
                exception("parse fail");
        }
        if (absorb(LEX::TokenType::BRACER)) {
            _symbols.pop();
        } else {
            // one more member
            VarDef* def = new VarDef();
            node->addMember(def);
            _symbols.push(def);
        }
        return true;
    } else if (absorb(LEX::TokenType::STRUCT)) {
        AtomExpr* id = new AtomExpr(AtomExpr::AtomType::Variable);
        if (_currToken.getType() == LEX::TokenType::ID) {
            id->setVar(_currToken.getValue<std::string>());
            absorb(LEX::TokenType::ID);
            node->setId(id);
            if (_currToken.getType() == LEX::TokenType::SEMICOLON) {
                _symbols.pop();    
                return true;
            } else if (absorb(LEX::TokenType::BRACEL)) {
                node->setStage(1);
                node->setDefined();
                return true;
            }
        }
    }

    exception("parse fail");
    return false;
}

template <>
bool MyParser::production<VarDef>()
{
    VarDef* node = dynamic_cast<VarDef*>(_symbols.top().getNode());
    if (node->currStage() == 1) {
        _symbols.pop();
        if (absorb(LEX::TokenType::EQUAL)) {
            Expr* init = new BinOpExpr(BinOpExpr::ASSIGN); // the least priority expr : assign expr
            node->addInitVal(init);
            _symbols.push(init);
        }
    } else {
        TypeNode* type = new TypeNode();
        AtomExpr* id = new AtomExpr(AtomExpr::AtomType::Variable);
        node->setType(type);
        node->setId(id);
        node->setStage(1);
        _symbols.push(id);
        _symbols.push(type);
    }
    return true;

}

template <>
bool MyParser::production<TypeDef>()
{
    TypeDef* node = dynamic_cast<TypeDef*>(_symbols.top().getNode());
    if (absorb(LEX::TokenType::TYPEDEF)) {
        TypeNode* type = new TypeNode();
        AtomExpr* id = new AtomExpr(AtomExpr::AtomType::Variable);
        node->setOrigType(type);
        node->setdefinedType(id);
        _symbols.pop();
        _symbols.push(id);
        _symbols.push(type);
        return true;
    } else {
        exception("parse fail");
    }
    return false;
}

template <>
bool MyParser::production<IfStmt>()
{
    IfStmt* node = dynamic_cast<IfStmt*>(_symbols.top().getNode());
    if (node->currStage() == 0) {
        if (!absorb(LEX::TokenType::IF) || !absorb(LEX::TokenType::PARENTHESESL)) {
            exception("parse fail");
        }
        Expr* cond = new BinOpExpr(BinOpExpr::ASSIGN);
        node->setCondition(cond);
        _symbols.push(cond);
        node->setStage(1);
    } else if (node->currStage() == 1) {
        if (!absorb(LEX::TokenType::PARENTHESESR) || !absorb(LEX::TokenType::BRACEL)) 
            exception("parse fail");
        if (absorb(LEX::TokenType::BRACER)) {
            if (_currToken.getType() == LEX::TokenType::ELSE && _lexer->peekToken(true).getType() == LEX::TokenType::BRACEL) {
                absorb(LEX::TokenType::ELSE);
                absorb(LEX::TokenType::BRACEL);
                node->setStage(2);
            } else if (_currToken.getType() == LEX::TokenType::SEMICOLON) {
                _symbols.pop();
            } else {
                exception("parse fail");
            }
        } else {
            if (!absorb(LEX::TokenType::SEMICOLON) && !node->getStatements().empty()) 
                exception("parse fail");
            Statement* stmt = selectStmt();
            node->addThenStmt(stmt);
            _symbols.push(stmt);
        }
    } else if (node->currStage() == 2) {
        if (!absorb(LEX::TokenType::SEMICOLON) && !node->getElseStmts().empty()) 
            exception("parse fail");
        if (absorb(LEX::TokenType::BRACER)) {
            _symbols.pop();
        } else {
            Statement* stmt = selectStmt();
            node->addElseStmt(stmt);
            _symbols.push(stmt);
        }
    } else {
        exception("parse fail");
    } 
    return true;
}

template <>
bool MyParser::production<ForStmt>()
{
    ForStmt* node = dynamic_cast<ForStmt*>(_symbols.top().getNode());
    LEX::TokenType tktype = _currToken.getType();
    if (node->currStage() == 0) {
        // begining
        if (!absorb(LEX::TokenType::FOR) || !absorb(LEX::TokenType::PARENTHESESL)) 
            exception("parse fail");
        if (absorb(LEX::TokenType::SEMICOLON)) {
            node->setStage(1);
        } else {
            if(!absorb(LEX::TokenType::COMMA) && !node->getInit().empty()) 
                exception("parse fail");
            VarDef* vardef = new VarDef();
            node->addInit(vardef);
            _symbols.push(vardef);
        }
    } else if (node->currStage() == 1) {
        // parse condition
        if (!absorb(LEX::TokenType::SEMICOLON)) {
            if (node->getCondition() != nullptr) 
                exception("parse fail");
            Expr* cond = new BinOpExpr(BinOpExpr::ASSIGN);
            node->addCondition(cond);
            _symbols.push(cond);
        }
        node->setStage(2);
    } else if (node->currStage() == 2) {
        // parse steps
        if (absorb(LEX::TokenType::PARENTHESESR) && absorb(LEX::TokenType::BRACEL)) {
            node->setStage(3);
        } else {
            if (!absorb(LEX::TokenType::COMMA) && !node->getSteps().empty()) 
                exception("parse fail");
            Statement* stmt = selectStmt();
            node->addStep(stmt);
            _symbols.push(stmt);
        }
    } else if (node->currStage() == 3) {
        // parse body
        if (absorb(LEX::TokenType::BRACER)) {
            _symbols.pop();
        } else {
            if (!absorb(LEX::TokenType::SEMICOLON) && !node->getBody().empty()) 
                exception("parse fail");
            Statement* stmt = selectStmt();
            node->addBody(stmt);
            _symbols.push(stmt);
        }
    }
    return true;
}

template <>
bool MyParser::production<AssignStmt>()
{
    AssignStmt* node = dynamic_cast<AssignStmt*>(_symbols.top().getNode());
    if (node->getLhs() ) {
        _symbols.pop();
        if (absorb(LEX::TokenType::EQUAL)) {
            BinOpExpr* assign = new BinOpExpr(BinOpExpr::BinOpType::ASSIGN);
            node->addRhs(assign);
            _symbols.push(assign);
        }
        return true;
    } else if (node->getLhs() == nullptr) {
        BinOpExpr* lhs = new BinOpExpr(BinOpExpr::BinOpType::OR);
        node->addLhs(lhs);
        _symbols.push(lhs);
        return true;
    } 
    exception("parse fail");
    return false;
}

template <>
bool MyParser::production<FuncCall>()
{
    FuncCall* node = dynamic_cast<FuncCall*>(_symbols.top().getNode());
    if (node->currStage() == 0) {
        node->setName(_currToken.getValue<std::string>());
        absorb(LEX::TokenType::ID);
        if (absorb(LEX::TokenType::PARENTHESESL)) {
            node->setStage(1);
        } else {
            exception("parse fail");
        }
    } else if (node->currStage() == 1) {
        if (absorb(LEX::TokenType::PARENTHESESR)) {
            _symbols.pop();
        } else {
            if(!absorb(LEX::TokenType::COMMA) && !node->getParams().empty()) 
                exception("parse fail");
            Expr* param = new BinOpExpr(BinOpExpr::BinOpType::ASSIGN);
            node->addParam(param);
            _symbols.push(param);
        }
    }
    return true;
}

template<>
bool MyParser::production<InclStmt>()
{
    InclStmt* incl = dynamic_cast<InclStmt*>(_symbols.top().getNode());
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
        _symbols.pop();
    } else {
        throw UTIL::MyException("invalid included file name");
    }
    return true;
}

template<>
bool MyParser::production<TypeNode>()
{
    /*
     * 0： basic Type: bool, char, [struct] id, void ...
     * 1： ref
     * 2： pointer
     * 3:  top-level const
     * 4:  low-level const
     * */
    std::stack<int> _typeInfo;
    TypeNode::TypeId basicType = TypeNode::TypeId::EMPTY;
    std::string type = "";
    while(true) {
        if (absorb(LEX::TokenType::CONST)) {
            if (_typeInfo.empty()) {
                _typeInfo.push(4); 
            } else {
                _typeInfo.push(3);
                break;
            }
        } else if (absorb(LEX::TokenType::VOID)) {
            if (basicType != TypeNode::TypeId::EMPTY) {
                exception("parse fail");
            }
            basicType = TypeNode::TypeId::VOID;
            _typeInfo.push(0);
        } else if (absorb(LEX::TokenType::CHAR)) {
            if (basicType != TypeNode::TypeId::EMPTY) {
                exception("parse fail");
            }
            basicType = TypeNode::TypeId::CHAR;
            _typeInfo.push(0);
        } else if (absorb(LEX::TokenType::BOOL)) {
            if (basicType != TypeNode::TypeId::EMPTY) {
                exception("parse fail");
            }
            basicType = TypeNode::TypeId::BOOL;
        } else if (absorb(LEX::TokenType::SHORT)) {
            if (basicType != TypeNode::TypeId::EMPTY) {
                exception("parse fail");
            }
            basicType = TypeNode::TypeId::SHORT;
            _typeInfo.push(0);
        } else if (absorb(LEX::TokenType::INT)) {
            if (basicType != TypeNode::TypeId::EMPTY) {
                exception("parse fail");
            }
            basicType = TypeNode::TypeId::INT;
            _typeInfo.push(0);
        } else if (absorb(LEX::TokenType::FLOAT)) {
            if (basicType != TypeNode::TypeId::EMPTY) {
                exception("parse fail");
            }
            basicType = TypeNode::TypeId::FLOAT;
            _typeInfo.push(0);
        } else if (absorb(LEX::TokenType::DOUBLE)) {
            if (basicType != TypeNode::TypeId::EMPTY) {
                exception("parse fail");
            }
            basicType = TypeNode::TypeId::DOUBLE;
            _typeInfo.push(0);
        } else if (absorb(LEX::TokenType::STRUCT)) {
            if (basicType != TypeNode::TypeId::EMPTY) {
                exception("parse fail");
            }
            if (_currToken.getType() != LEX::TokenType::ID) {
                exception("parse fail");
            } else {
                type = _currToken.getValue<std::string>();
                absorb(LEX::TokenType::ID);
                basicType = TypeNode::TypeId::Ref;
                _typeInfo.push(0);
            }
        } else if (_currToken.getType() == LEX::TokenType::ID) {
            if (basicType == TypeNode::TypeId::EMPTY) {
                type = _currToken.getValue<std::string>();
                absorb(LEX::TokenType::ID);
                basicType = TypeNode::TypeId::Ref;
                _typeInfo.push(0);
            } else {
                break;
            }
        } else if (absorb(LEX::TokenType::MULTI)) {
            _typeInfo.push(2);
        } else if (absorb(LEX::TokenType::REF)) {
            _typeInfo.push(1);
        }
    }

    TypeNode* node = dynamic_cast<TypeNode*>(_symbols.top().getNode());
    TypeNode* curr = node; 
    while (!_typeInfo.empty()) {
        int sym = _typeInfo.top();
        _typeInfo.pop();
        if (sym == 0) {
            curr->setType(basicType);
            if (type != "") {
                curr->setTypeRef(type);
            }
        } else if (sym == 1) {
            TypeNode* child = new TypeNode();
            curr->setBaseType(child);
            curr->setType(TypeNode::TypeId::Ref);
            curr = child;
        } else if (sym == 2) {
            TypeNode* child = new TypeNode();
            curr->setBaseType(child);
            curr->setType(TypeNode::TypeId::Pointer);
            curr = child;
        } else if (sym == 3) {
            curr->setConst(true);
        } else if (sym == 4) {
            curr->setConst();
        } else {
            exception("parse fail");
        }
    }
    _symbols.pop();
    return true;
}

template <>
bool MyParser::production<AtomExpr>()
{
    AtomExpr* node = dynamic_cast<AtomExpr*>(_symbols.top().getNode());
    auto processVariable = [&](){
                node->setType(AtomExpr::AtomType::Variable);
                node->setVar(_currToken.getValue<std::string>());
                absorb(LEX::TokenType::ID);
                _symbols.pop();
    };
    auto processKeyword= [&](){
                node->setType(AtomExpr::AtomType::Keyword);
                node->setKeyword(_lexer->keywordStr(_currToken.getType()));
                absorb();
                _symbols.pop();
    };
    auto processFuncCall = [&](){
        FuncCall* func = new FuncCall();
        node->setType(AtomExpr::AtomType::FuncCall);
        node->setFuncCall(func);
        _symbols.pop();
        _symbols.push(func);
    };
    auto processStrLiteral = [&](){
        node->setType(AtomExpr::AtomType::StrLiteral);
        node->setLiteral(_currToken.getValue<std::string>());
        if(!absorb(LEX::TokenType::STRLITERAL)) {
            exception("parse fail");
        }
        _symbols.pop();
    };
    auto processIntLiteral = [&](){
        node->setType(AtomExpr::AtomType::IntLiteral);
        node->setLiteral(_currToken.getValue<long long>());
        if(!absorb(LEX::TokenType::INTEGERLITERAL)) {
            exception("parse fail");
        }
        _symbols.pop();
    };
    auto processFloatLiteral = [&](){
        node->setType(AtomExpr::AtomType::FloatLiteral);
        node->setLiteral(_currToken.getValue<float>());
        if(!absorb(LEX::TokenType::FLOATLITERAL)) {
            exception("parse fail");
        }
        _symbols.pop();
    };
    auto processParenthesed = [&](){
        if (node->getAtomType() == AtomExpr::AtomType::Parenthesed && node->getParenthesed()) {
            if (!absorb(LEX::TokenType::PARENTHESESR)) 
                exception("parse fail");
            _symbols.pop();
        } else {
            absorb(LEX::TokenType::PARENTHESESL);
            node->setType(AtomExpr::AtomType::Parenthesed);
            BinOpExpr* assign = new BinOpExpr(BinOpExpr::ASSIGN); 
            node->setExpr(assign);
            _symbols.push(assign);
        }
        return true;
    };

    if (node->getAtomType() == AtomExpr::AtomType::Bad) {
        LEX::TokenType tktype = _currToken.getType();
        if (tktype == LEX::TokenType::ID) {
            if (_lexer->peekToken(true).getType() == LEX::TokenType::PARENTHESESL) {
                // funcCall
                processFuncCall();
            } else {
                processVariable();
            }
        } else if (_currToken.getType() == LEX::TokenType::STRLITERAL) {
            processStrLiteral();
        } else if (_currToken.getType() == LEX::TokenType::INTEGERLITERAL) {
            processIntLiteral();
        } else if (_currToken.getType() == LEX::TokenType::FLOATLITERAL) {
            processFloatLiteral();
        } else if (_currToken.getType() == LEX::TokenType::PARENTHESESL) {
            return processParenthesed();
        } else if (_lexer->keywordStr(tktype) != "") {
            processKeyword();
        } else {
            exception("parse fail");
        }
    } else {
        switch (node->getAtomType()) {
        case AtomExpr::AtomType::Variable:
            processVariable();
            break;
        case AtomExpr::AtomType::FuncCall:
            processFuncCall();
            break;
        case AtomExpr::AtomType::StrLiteral:
            processStrLiteral();
            break;
        case AtomExpr::AtomType::IntLiteral:
            processIntLiteral();
            break;
        case AtomExpr::AtomType::FloatLiteral:
            processFloatLiteral();
            break;
        case AtomExpr::AtomType::Parenthesed:
            return processParenthesed();
            break;
        case SYNTAX::AtomExpr::AtomType::Keyword:
            processKeyword();
            break;
        default:
            break;
        }
    }
    return true;
}

template <>
bool MyParser::production<UniOpExpr>()
{
    UniOpExpr* node = dynamic_cast<UniOpExpr*>(_symbols.top().getNode());
    LEX::TokenType tktype = _currToken.getType();
    // preOp
    if (node->currStage() == 0) {
        BinOpExpr* factor = new BinOpExpr(BinOpExpr::BinOpType::MEMBER);
        if (absorb(LEX::TokenType::ADD)) {
            if (absorb(LEX::TokenType::ADD)) {
                _symbols.pop();
                node->setType(UniOpExpr::UniOpType::PREINC);
            } else {
                exception("parse fail");
            }
        } else if (absorb(LEX::TokenType::MINUS)) {
            if (absorb(LEX::TokenType::MINUS)) {
                _symbols.pop();
                node->setType(UniOpExpr::UniOpType::PREDEC);
            } else {
                _symbols.pop();
                node->setType(UniOpExpr::UniOpType::NEG);
            }
        } else if (absorb(LEX::TokenType::BITNOT)) {
            _symbols.pop();
            node->setType(UniOpExpr::UniOpType::BITNOT);
        } else if (absorb(LEX::TokenType::LOGNOT)) {
            _symbols.pop();
            node->setType(UniOpExpr::UniOpType::LOGICNOT);
        } else if (absorb(LEX::TokenType::REF)) {
            _symbols.pop();
            node->setType(UniOpExpr::UniOpType::GETADDR);
        } else if (absorb(LEX::TokenType::MULTI)) {
            _symbols.pop();
            node->setType(UniOpExpr::UniOpType::RESOLVEADDR);
        } else {
            // try PostOp
            node->setStage(1);
        }
        node->setFactor(factor); 
        _symbols.push(factor);
    } else if (node->currStage() == 1) {
        _symbols.pop(); 
        if (_currToken.getType() == LEX::TokenType::ADD && _lexer->peekToken(true).getType() == LEX::TokenType::ADD) {
            absorb(LEX::TokenType::ADD);
            absorb(LEX::TokenType::ADD);
           node->setType(UniOpExpr::UniOpType::POSTINC);
        } else if (_currToken.getType() == LEX::TokenType::MINUS && _lexer->peekToken(true).getType() == LEX::TokenType::MINUS) {
            absorb(LEX::TokenType::MINUS);
            absorb(LEX::TokenType::MINUS);
            node->setType(UniOpExpr::UniOpType::POSTDEC);
        } else {
            node->setType(UniOpExpr::UniOpType::NONE);
        } 
    }
    return true;
}

bool
MyParser::processOr(BinOpExpr* expr)
{
    if (expr->getLhs() == nullptr) {
        BinOpExpr* lhs = new BinOpExpr(BinOpExpr::BinOpType::AND);
        expr->setLhs(lhs);
        _symbols.push(lhs);
    } else {
        _symbols.pop();
        if (absorb(LEX::TokenType::LOGOR)) {
            BinOpExpr* rhs = new BinOpExpr(BinOpExpr::BinOpType::OR);
            expr->setRhs(rhs);
            _symbols.push(rhs);
        }
    }
    return true;
}

bool                    
MyParser::processAnd(BinOpExpr* expr)
{
    if (expr->getLhs() == nullptr) {
        BinOpExpr* lhs = new BinOpExpr(BinOpExpr::BinOpType::REL);
        expr->setLhs(lhs);
        _symbols.push(lhs);
    } else {
        _symbols.pop();
        if (absorb(LEX::TokenType::LOGAND)) {
            BinOpExpr* rhs = new BinOpExpr(BinOpExpr::BinOpType::AND);
            expr->setRhs(rhs);
            _symbols.push(rhs);
        }
    }
    return true;
}


bool
MyParser::processRel(BinOpExpr* expr)
{
    if (expr->getLhs() == nullptr) {
        BinOpExpr* lhs = new BinOpExpr(BinOpExpr::BinOpType::BITOR);
        expr->setLhs(lhs);
        _symbols.push(lhs);
    } else {
        _symbols.pop();
        Expr* rhs = nullptr;
        if (absorb(LEX::TokenType::LESS)) {
            if (absorb(LEX::TokenType::EQUAL)) {
                expr->setBinType(BinOpExpr::BinOpType::LESSE);
            } else {
                expr->setBinType(BinOpExpr::BinOpType::LESS);
            }
            rhs = new BinOpExpr(BinOpExpr::BinOpType::REL);
        } else if (absorb(LEX::TokenType::GREATER)) {
            if (absorb(LEX::TokenType::EQUAL)) {
                expr->setBinType(BinOpExpr::BinOpType::LARGERE);
            } else {
                expr->setBinType(BinOpExpr::BinOpType::LARGER);
            }
            rhs = new BinOpExpr(BinOpExpr::BinOpType::REL);
        } else if (_currToken.getType() == LEX::TokenType::EQUAL &&
                   _currToken.getType() == LEX::TokenType::EQUAL) {
            absorb(LEX::TokenType::EQUAL);
            absorb(LEX::TokenType::EQUAL);
            expr->setBinType(BinOpExpr::BinOpType::EQUAL);
            rhs = new BinOpExpr(BinOpExpr::BinOpType::REL);
        } else if (absorb(LEX::TokenType::LOGNOT)) {
            if (absorb(LEX::TokenType::EQUAL)) {
                expr->setBinType(BinOpExpr::BinOpType::EQUAL);
                rhs = new BinOpExpr(BinOpExpr::BinOpType::REL);
            }
        }

        if (rhs) {
            expr->setRhs(rhs);
            _symbols.push(rhs);
        } 
    }
    return true;
}

bool
MyParser::processBOR(BinOpExpr* expr)
{
    if (expr->getLhs() == nullptr) {
        BinOpExpr* lhs = new BinOpExpr(BinOpExpr::BinOpType::BITXOR); 
        expr->setLhs(lhs);
        _symbols.push(lhs);
    } else {
        _symbols.pop();
        if (absorb(LEX::TokenType::BITOR)) {
            BinOpExpr* rhs = new BinOpExpr(BinOpExpr::BinOpType::BITOR);
            expr->setRhs(rhs);
            _symbols.push(rhs);
        }
    }
    return true;
}

bool     
MyParser::processBXOR(BinOpExpr* expr)
{
    if (expr->getLhs() == nullptr) {
        BinOpExpr* lhs = new BinOpExpr(BinOpExpr::BinOpType::BITAND); 
        expr->setLhs(lhs);
        _symbols.push(lhs);
    } else {
        _symbols.pop();
        if (absorb(LEX::TokenType::XOR)) {
            BinOpExpr* rhs = new BinOpExpr(BinOpExpr::BinOpType::BITXOR);
            expr->setRhs(rhs);
            _symbols.push(rhs);
        }
    }
    return true;
}

bool     
MyParser::processBAND(BinOpExpr* expr)
{
    if (expr->getLhs() == nullptr) {
        BinOpExpr* lhs = new BinOpExpr(BinOpExpr::BinOpType::SHIFT); 
        expr->setLhs(lhs);
        _symbols.push(lhs);
    } else {
        _symbols.pop();
        if (absorb(LEX::TokenType::REF)) {
            BinOpExpr* rhs = new BinOpExpr(BinOpExpr::BinOpType::BITAND);
            expr->setRhs(rhs);
            _symbols.push(rhs);
        }
    }
    return true;
}

bool     
MyParser::processShift(BinOpExpr* expr)
{
    if (expr->getBinType() == BinOpExpr::BinOpType::SHIFT) {
        BinOpExpr* lhs = new BinOpExpr(BinOpExpr::BinOpType::ADD);
        expr->setLhs(lhs);
        _symbols.push(lhs);
        expr->setBinType(BinOpExpr::BinOpType::SHL/* temporary*/);
    } else {
         _symbols.pop();
         if (absorb(LEX::TokenType::LESS)) {
             if (absorb(LEX::TokenType::LESS))
                expr->setBinType(BinOpExpr::BinOpType::SHL); 
             else 
                exception("parse fail");
         } else if (absorb(LEX::TokenType::GREATER)) {
             if (absorb(LEX::TokenType::GREATER))
                expr->setBinType(BinOpExpr::BinOpType::SHR);
             else 
                exception("parse fail");
         } else {
            return true;
         }
         BinOpExpr* rhs = new BinOpExpr(BinOpExpr::BinOpType::SHIFT);
         expr->setRhs(rhs);
         _symbols.push(rhs);
    }
    return true;
}

bool     
MyParser::processADD(BinOpExpr* expr)
{
    if (expr->getLhs() == nullptr) {
        BinOpExpr* lhs = new BinOpExpr(BinOpExpr::BinOpType::MULTI); 
        expr->setLhs(lhs);
        _symbols.push(lhs);
    } else {
        _symbols.pop();
        if (absorb(LEX::TokenType::ADD)) {
            BinOpExpr* rhs = new BinOpExpr(BinOpExpr::BinOpType::ADD);
            expr->setRhs(rhs);
            _symbols.push(rhs);
        } else if (absorb(LEX::TokenType::MINUS)) {
            expr->setBinType(BinOpExpr::BinOpType::MINUS);
            BinOpExpr* rhs = new BinOpExpr(BinOpExpr::BinOpType::ADD);
            expr->setRhs(rhs);
            _symbols.push(rhs);
        }
    }
    return true;
}

bool     
MyParser::processMul(BinOpExpr* expr)
{
    if (expr->getLhs() == nullptr) {
        UniOpExpr* lhs = new UniOpExpr(UniOpExpr::UniOpType::BAD); 
        expr->setLhs(lhs);
        _symbols.push(lhs);
    } else {
        _symbols.pop();
        if (absorb(LEX::TokenType::MULTI)) {
            BinOpExpr* rhs = new BinOpExpr(BinOpExpr::BinOpType::MULTI);
            expr->setRhs(rhs);
            _symbols.push(rhs);
        } else if (absorb(LEX::TokenType::DIV)) {
            expr->setBinType(BinOpExpr::BinOpType::DIV);
            BinOpExpr* rhs = new BinOpExpr(BinOpExpr::BinOpType::MULTI);
            expr->setRhs(rhs);
            _symbols.push(rhs);
        } else if (absorb(LEX::TokenType::MOD)) {
            expr->setBinType(BinOpExpr::BinOpType::MOD);
            BinOpExpr* rhs = new BinOpExpr(BinOpExpr::BinOpType::MULTI);
            expr->setRhs(rhs);
            _symbols.push(rhs);
        }
    }
    return true;
}

bool
MyParser::processMember(BinOpExpr* expr)
{
    if (expr->getLhs() == nullptr) {
        AtomExpr* lhs = new AtomExpr(AtomExpr::AtomType::Bad);
        expr->setLhs(lhs);
        _symbols.push(lhs);
    } else {
        _symbols.pop();
        if (absorb(LEX::TokenType::DOT)) {
            BinOpExpr* rhs = new BinOpExpr(BinOpExpr::BinOpType::MEMBER);
            expr->setRhs(rhs);
            _symbols.push(rhs);
            expr->setBinType(BinOpExpr::BinOpType::DOT);
        } else if (absorb(LEX::TokenType::POINTER)) {
            BinOpExpr* rhs = new BinOpExpr(BinOpExpr::BinOpType::MEMBER);
            expr->setRhs(rhs);
            _symbols.push(rhs);
            expr->setBinType(BinOpExpr::BinOpExpr::POINTER);
        }
    }
    return true;
}

bool
MyParser::processAssign(BinOpExpr* expr)
{
    if (expr->getLhs() == nullptr) {
        BinOpExpr* lhs = new BinOpExpr(BinOpExpr::BinOpType::OR);
        expr->setLhs(lhs);
        _symbols.push(lhs);
    } else {
        _symbols.pop();
        if (absorb(LEX::TokenType::EQUAL)) {
            BinOpExpr* rhs = new BinOpExpr(BinOpExpr::BinOpType::ASSIGN);
            expr->setRhs(rhs);
            _symbols.push(rhs);
        }
    }
    return true;
}

template <> 
bool MyParser::production<BinOpExpr>()
{
    BinOpExpr* node = dynamic_cast<BinOpExpr*>(_symbols.top().getNode());
    switch(node->getBinType()) {
    case BinOpExpr::BinOpType::OR:
         return processOr(node);
    case BinOpExpr::BinOpType::AND:
         return processAnd(node);
    case BinOpExpr::BinOpType::REL:
         return processRel(node);
    case BinOpExpr::BinOpType::BITOR:
         return processBOR(node);
    case BinOpExpr::BinOpType::BITXOR:
         return processBXOR(node);
    case BinOpExpr::BinOpType::BITAND:
         return processBAND(node);
    case BinOpExpr::BinOpType::SHIFT:
    case BinOpExpr::BinOpType::SHL:
    case BinOpExpr::BinOpType::SHR:
         return processShift(node);
    case BinOpExpr::BinOpType::ADD:
    case BinOpExpr::BinOpType::MINUS:
         return processADD(node);
    case BinOpExpr::BinOpType::MULTI:
    case BinOpExpr::BinOpType::DIV:
    case BinOpExpr::BinOpType::MOD:
         return processMul(node);
    case BinOpExpr::BinOpType::ASSIGN:
         return processAssign(node);
    case BinOpExpr::BinOpType::MEMBER:
    case BinOpExpr::BinOpType::POINTER:
    case BinOpExpr::BinOpType::DOT:
         return processMember(node);
    default:
        exception("parse fail");
        return false;
    }
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
    auto func_var_def = [&]() {
        while(true) {
            LEX::TokenType tmp = _lexer->peekToken().getType();
            if (tmp == LEX::TokenType::SEMICOLON) break;
            if (tmp == LEX::TokenType::PARENTHESESL) {
                return dynamic_cast<Statement*>(new FuncDef());
            }
        }
        return dynamic_cast<Statement*>(new VarDef());
    };

    if (type == LEX::TokenType::STRUCT && _lexer->peekToken(true).getType() == LEX::TokenType::ID) {
        // struct Def
        if (_lexer->peekToken().getType() == LEX::TokenType::ID) {
            stmt = func_var_def();
        } else {
            stmt = new StructDef();
        }
    } else if (isType(type)) {
        // check if is function
        stmt = func_var_def();
    } else if (type == LEX::TokenType::RETURN) {
        stmt = new ReturnStmt();
    } else if (type == LEX::TokenType::TYPEDEF) {
        stmt = new TypeDef();
    } else if (type == LEX::TokenType::IF) {
        stmt = new IfStmt();
    } else if (type == LEX::TokenType::FOR) {
        stmt = new ForStmt();
    } else if (type == LEX::TokenType::ID) {
        // check vardef/funcdef start with typeref
        LEX::TokenType ttype = _lexer->peekToken(true).getType();
        if (ttype == LEX::TokenType::ID || ttype == LEX::TokenType::REF || ttype == LEX::TokenType::MULTI) {
            // curr id is a typeRef
            stmt = func_var_def();
        } else if (_lexer->peekToken(true).getType() == LEX::TokenType::PARENTHESESL) {
            stmt = new FuncCall();
        } else {
            stmt = new AssignStmt(); 
        }
    } else if (type ==LEX::TokenType::SHARP) {
        stmt = new InclStmt();
    } else {
        stmt = new AssignStmt(); 
    }
    stmt->setLine(_currToken.currLine(), _currToken.currColumn());
    return stmt;
}

bool                    
MyParser::isType(LEX::TokenType token)
{
    if (token == LEX::TokenType::VOID ||
        token == LEX::TokenType::BOOL ||
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
MyParser::rdp()
{
    SyntaxNode currNode = _symbols.top();
    if (currNode.isKeyword()) {
        if (absorb(currNode.keywordType())) {
            _symbols.pop();
            return true;
        } else {
            exception("parse fail");
        }
    } else if (!currNode.isKeyword()) {
        switch(currNode.syntaxType()) {
        case SyntaxType::Program:
            return production<Program>();
        case SyntaxType::RetStmt:
            return production<ReturnStmt>();
        case SyntaxType::FuncDef:
            return production<FuncDef>();
        case SyntaxType::StructDef:
            return production<StructDef>();
        case SyntaxType::VarDef:
            return production<VarDef>();
        case SyntaxType::TypeDef:
            return production<TypeDef>();
        case SyntaxType::IfStmt:
            return production<IfStmt>();
        case SyntaxType::ForStmt:
            return production<ForStmt>();
        case SyntaxType::Assignment:
            return production<AssignStmt>();
        case SyntaxType::FuncCall:
            return production<FuncCall>();
        case SyntaxType::InclStmt:
            return production<InclStmt>();
        case SyntaxType::TypeNode:
            return production<TypeNode>();
        case SyntaxType::AtomExpr:
            return production<AtomExpr>();
        case SyntaxType::UniOpExpr:
            return production<UniOpExpr>();
        case SyntaxType::BinOpExpr:
            return production<BinOpExpr>();
        //case SyntaxType::SelectExpr:
        //    return production<SelectExpr>();
        default:
            exception("parse fail");
        }
    }
    exception("parse fail");
    return false;
}

bool        
MyParser::parse()
{
    //init
    init();

    try{
        while (!_symbols.empty()) {
            if (hasMoreToken()) {
                if(rdp()) {
                    continue;
                } else {
                    errorHandler();
                    return false;
                }
            } else {
                return production<Program>();
            } 
        }
    } catch (UTIL::MyException err) {
        errorHandler(err.what());
        exit(-1);
    }
    return !hasMoreToken();
}

}
