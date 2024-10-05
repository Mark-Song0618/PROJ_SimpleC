#pragma once

#include "../myIR/ir.hh"
#include "../myIR/IRVisitor.hh"
#include "myAsm.hh"
namespace ASM
{
class AsmGenerator : public IR::IRVisitor
{
public:
    Assemble*   generate(IR::IR* program);

private:
    virtual void visit(IR::IRExpr*) {}
};

}
