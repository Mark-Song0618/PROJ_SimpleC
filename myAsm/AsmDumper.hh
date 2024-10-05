#pragma once
#include "AsmVisitor.hh"
#include "myAsm.hh"
#include <string>
#include <fstream>

namespace ASM
{

class AttDumper : public AsmVisitor 
{
public:
    void dump(Assemble* code, std::string path);

private:
    virtual void visit(AsmCode*) {}
    virtual void visit(Label*);
    virtual void visit(Directive*);
    virtual void visit(Inst*);

    void dumpStr(std::string str) { _asmFile << str << std::endl; }
private:
    std::ofstream _asmFile;
};

}
