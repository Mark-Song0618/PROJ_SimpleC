#pragma once

#include "AstVisitor.hh"
#include "astPub.hh"
#include <iostream>
#include <fstream>
#include <string>

namespace SYNTAX {

class AstDumper : public AstVisitor {
public:
    AstDumper() : _tab(4), _compressExpr(false) {}

    void    dumpAst(TreeNode* root, std::string path = "");

    void    setTab(size_t n) { _tab = n;}

    void    compressExpr(bool compress = true) {_compressExpr = compress; }

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

    void    visit(FuncDef*);
    void    visit(ReturnStmt*);
    void    visit(StructDef*);
    void    visit(VarDef*);
    void    visit(TypeDef*);
    void    visit(IfStmt*);
    void    visit(ForStmt*);
    void    visit(AssignStmt*);
    void    visit(FuncCall*);
    void    visit(InclStmt*);
    void    visit(AtomExpr*);
    void    visit(UniOpExpr*);
    void    visit(BinOpExpr*);
    void    visit(TypeNode*);
    
    template <typename T>
    void    preAction(T* node)
    {
        std::string str = getDumpStr(node);
        ++_level;
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
    bool            _compressExpr;
};

}
