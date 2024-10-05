#pragma once
#include <cwctype>
#include <stack>
#include "../myLexer/lexPub.hh"
#include "astPub.hh"

namespace SYNTAX
{
class SyntaxNode
{
public:
    SyntaxNode(TreeNode* node);

    SyntaxNode(LEX::TokenType kw);

    bool                    isKeyword() const { return _isKw; } 

    LEX::TokenType          keywordType() const { return _kw; } 

    SyntaxType              syntaxType() const { return _node->syntaxType(); }

    TreeNode*               getNode() {return _node;}

private:
    bool                    _isKw;

    TreeNode*               _node;

    LEX::TokenType          _kw;
};

class MyParser {
public:
    MyParser() = default;

    MyParser(LEX::MyLexer* lexer) { _lexer = lexer; }

    void                    setLexer(LEX::MyLexer* lexer);

    bool                    hasLexer();

    bool                    parse();

    std::string             srcFile() { return _lexer->srcFile(); }

    Program*                getResult();

private:
    void                    init();

    bool                    hasMoreToken();

    bool                    absorb(LEX::TokenType token);

    void                    absorb();

    void                    exception(const std::string& str);

    void                    errorHandler(const std::string& = "");

    template<typename T>
    T*                      production();

    Expr*                   productUniOp();

    Expr*                   productOr();
    Expr*                   productAnd();
    Expr*                   productRel();
    Expr*                   productBOR();
    Expr*                   productBXOR();
    Expr*                   productBAND();
    Expr*                   productShift();
    Expr*                   productADD();
    Expr*                   productMul();
    Expr*                   productMember();
    Expr*                   productAssign();
    Expr*                   productFactor();

    Statement*              selectStmt();

    bool                    isType(LEX::TokenType token);
    bool                    isPostfix();

private:
    LEX::MyLexer*           _lexer;

    Program*                _result;

    LEX::Token              _currToken;
    
    std::stack<SyntaxNode>  _symbols;
};

template<>
VarDef*
MyParser::production<VarDef>();

template<>
TypeNode*
MyParser::production<TypeNode>();

}
