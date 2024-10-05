#include "IRDumper.hh"

namespace IR
{

void
IRDumper::dump(IR* prog, std::string file)
{
    _output = std::ofstream(file, std::ios::out);
    prog->accept(this);
    _output.close();
}

void 
IRDumper::dumpStr(const std::string& str)
{
    std::string tab(4 * _level, ' ');
    _output << tab << str << std::endl;
}

void
IRDumper::dumpVar(Variable* var)
{
    ++_level;
    std::string str("Variable: ");
    str += var->name();
    str += ".";
    if (var->getInitVal()) {
        str += " Init Value: ";
        dumpStr(str);
        var->getInitVal()->accept(this);
    } else {
        dumpStr(str);
    }
    --_level;
}

void 
IRDumper::dumpConst(Constant* constVar)
{
    ++_level;
    std::string str("Constant: ");
    str += constVar->name();
    str += ".";
    if (constVar->getInitVal()) {
        str += " Init Value: ";
        dumpStr(str);
        constVar->getInitVal()->accept(this);
    } else {
        dumpStr(str);
    }
    --_level;
}

void 
IRDumper::dumpLiteral(ConstEntry* entry)
{
    ++_level;
    std::string str("String: ");
    dumpStr(str + entry->getLiteral());
    --_level;

}

void 
IRDumper::dumpFunc(Function* func)
{
    ++_level;
    dumpStr("Function: " + func->name());
    for (auto param : func->getParams()) {
        dumpStr("Param: ");
        if (param->isConst()) {
            dumpConst(dynamic_cast<Constant*>(param));
        } else {
            dumpVar(dynamic_cast<Variable*>(param));
        }
    }
    for (auto stmt : func->getStmt()) {
        dumpStr("Function Body Stmt: ");
        stmt->accept(this);
    }
    --_level;
}

void
IRDumper::visit(IRVar* var) 
{
   ++_level;
   std::string str("Referred to: ");
   str += var->getEntity()->name();
   dumpStr(str);
   --_level;
}

void
IRDumper::visit(IRInt* val) 
{
   ++_level;
   dumpStr("Int: " + std::to_string(val->getValue()));
   --_level;
}

void
IRDumper::visit(IRStr* val) 
{
   ++_level;
   std::string str("Referred to: ");
   str += val->getEntry()->getLiteral();
   dumpStr(str);
   --_level;
}

void
IRDumper::visit(IRAddr* addr) 
{
   ++_level;
   dumpStr("GetAddress of: ");
   if (addr->getEntity()) {
       dumpStr(addr->getEntity()->name());
   } else {
        addr->getValue()->accept(this);
   }
   --_level;

}

void
IRDumper::visit(IRMem* mem) 
{
    ++_level;
    dumpStr("Load Memory of: "); 
    mem->getPtr()->accept(this);
    --_level;
}

void
IRDumper::visit(IRCall* call) 
{
   ++_level;
   dumpStr("Call: " + call->getFunc()->name());
   for (auto param : call->getParams()) {
       dumpStr("Param: ");
       param->accept(this);
   }
   --_level;
}

void
IRDumper::visit(IRAssign* assign) 
{
   ++_level;
   dumpStr("Assign: ");
   assign->getRight()->accept(this);
   dumpStr("to: ");
   assign->getLeft()->accept(this);
   --_level;
}

void
IRDumper::visit(IRBinExpr* bin) 
{
   ++_level;
   dumpStr("BinOp: ");
   dumpStr("BinOpType：" + bin->getOpStr());
   dumpStr("Lhs Expr: ");
   bin->getLeft()->accept(this);
   dumpStr("Rhs Expr: ");
   bin->getRight()->accept(this);
   --_level;
}

void
IRDumper::visit(IRUniExpr* uni) 
{
   ++_level;
   dumpStr("UniOp: ");
   dumpStr("UniOp Type：" + uni->getOpStr());
   dumpStr("UniOp Expr: ");
   uni->getExpr()->accept(this);
   --_level;
}

void
IRDumper::visit(IRExprStmt* stmt) 
{
   ++_level;
   stmt->getExpr()->accept(this);
   --_level;
}

void
IRDumper::visit(IRJmp* jmp) 
{
   ++_level;
   dumpStr("Jump to: ");
   dumpStr(jmp->getDest()->symbol());
   --_level;
}

void
IRDumper::visit(IRCjmp* cjmp) 
{
   ++_level;
   dumpStr("Cjump: ");
   dumpStr("Condition: ");
   cjmp->getCond()->accept(this);
   dumpStr("Then goto: " + cjmp->getThen()->symbol());
   dumpStr("Else goto: " + cjmp->getElse()->symbol());
   --_level;
}

void
IRDumper::visit(IRRET* ret) 
{
   ++_level;
   std::string str("Return Stmt");
   dumpStr(str);
   IRVisitor::visit(ret);
   --_level;
}

void
IRDumper::visit(IRLabel* label) 
{
   ++_level;
   dumpStr(label->getLabel()->symbol());
   --_level;
}

void
IRDumper::visit(IR* ir) 
{
    dumpStr("Program:"); // todo: source file
    for (auto var : ir->getScope()->getVariables()) {
        dumpVar(var);
    } 

    for (auto constVar : ir->getScope()->getConstants()) {
        dumpConst(constVar); 
    }

    for (auto str : ir->getConstTable().getLiterals()) {
        dumpLiteral(str);
    }

    for (auto func : ir->getFunctions()) {
        dumpFunc(func);
    }
}

}
