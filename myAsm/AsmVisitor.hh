#pragma once

namespace ASM
{

class AsmCode;
class Label;
class Directive;
class Inst;

class AsmVisitor
{
public:
    virtual void visit(AsmCode*) {}
    virtual void visit(Label*) {}
    virtual void visit(Directive*) {}
    virtual void visit(Inst*) {} 
};

}
