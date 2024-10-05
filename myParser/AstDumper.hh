#pragma once

#include "AstVisitor.hh"
#include "astPub.hh"
#include <iostream>
#include <fstream>
#include <string>

namespace SYNTAX {

class AstDumper : public AstVisitor {
public:
    AstDumper() : _tab(4) {}

    void    dumpAst(TreeNode* root, std::string path = "");

    void    setTab(size_t n) { _tab = n;}

private:
    template<typename NodeType>
    std::string
    getDumpStr(NodeType* node)
    {
        std::string rt(_level*_tab, ' ');
        rt += node->getClassInfo();
        return std::move(rt);
    }

    void    dumpStr(const char*);
    void    dumpStr(const std::string&);
    void    dumpType(Type*);
    void    dumpTypeRef(TypeRef*);

    void    visit(FuncDef*);
    void    visit(MemberExpr*);
    void    visit(ExprStmt*);
    void    visit(ReturnStmt*);
    void    visit(StructDef*);
    void    visit(VarDef*);
    void    visit(TypeDef*);
    void    visit(IfStmt*);
    void    visit(ForStmt*);
    void    visit(Variable*);
    void    visit(FuncCall*);
    void    visit(StrLiteral*);
    void    visit(IntLiteral*);
    void    visit(FloatLiteral*);
    void    visit(Parenthesed*);
    void    visit(Keyword*);
    void    visit(InclStmt*);
    void    visit(UniOpExpr*);
    void    visit(BinOpExpr*);
    void    visit(TypeNode*);
    
    template <typename T>
    void    preAction(T* node)
    {
        ++_level;
        std::string str = getDumpStr(node);
        _output << str << std::endl;
    }

    template <typename T>
    void    postAction(T*)
    {
        --_level;
    }

private:
    int             _level;
    std::ofstream   _output;
    size_t          _tab;
};

}
