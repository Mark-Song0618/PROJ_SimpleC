#pragma once

#include "IRVisitor.hh"
#include <fstream>
namespace IR
{

class IRDumper : public IRVisitor
{
public:
    void dump(IR* prog, std::string file);
private:
    void dumpVar(Variable*);
    void dumpConst(Constant*);
    void dumpLiteral(ConstEntry*);
    void dumpFunc(Function*);
    void dumpStr(const std::string&);


    void visit(IRExpr*) {}
    void visit(IRVar*) ;
    void visit(IRInt*) ;
    void visit(IRStr*) ;
    void visit(IRAddr*) ;
    void visit(IRMem*) ;
    void visit(IRCall*) ;
    void visit(IRAssign*) ;
    void visit(IRBinExpr*) ;
    void visit(IRUniExpr*) ;
    void visit(IRExprStmt*) ;
    void visit(IRJmp*) ;
    void visit(IRCjmp*) ;
    void visit(IRRET*) ;
    void visit(IRLabel*) ;
    void visit(IR*) ;

private:
    std::ofstream   _output;
    int             _level = 0;
};

}
