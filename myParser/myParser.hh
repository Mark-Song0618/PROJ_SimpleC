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

    SyntaxType              syntaxType() const { return _node->type(); }

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

    Program*                getResult();

private:
    void                    init();

    bool                    hasMoreToken();

    bool                    rdp();

    bool                    absorb(LEX::TokenType token);

    void                    absorb();

    void                    exception(const std::string& str);

    void                    errorHandler(const std::string& = "");

    template<typename T>
    bool                    production();

    bool                    processOr(BinOpExpr*);
    bool                    processAnd(BinOpExpr*);
    bool                    processRel(BinOpExpr*);
    bool                    processBOR(BinOpExpr*);
    bool                    processBXOR(BinOpExpr*);
    bool                    processBAND(BinOpExpr*);
    bool                    processShift(BinOpExpr*);
    bool                    processADD(BinOpExpr*);
    bool                    processMul(BinOpExpr*);
    bool                    processMember(BinOpExpr*);
    bool                    processAssign(BinOpExpr*);

    Statement*              selectStmt();

    bool                    isType(LEX::TokenType token);

private:
    LEX::MyLexer*           _lexer;

    Program*                _result;

    LEX::Token              _currToken;
    
    std::stack<SyntaxNode>  _symbols;
};



}
