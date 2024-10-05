#include "AsmDumper.hh"
namespace ASM
{


void 
AttDumper::dump(Assemble* codes, std::string path)
{

    _asmFile.open(path.c_str());
    for (auto code : codes->getAsmCodes()) {
        code->accept(this);
    }
    _asmFile.close();
}

void 
AttDumper::visit(Label* label)
{
    dumpStr(label->symbol() + ":");
}

void 
AttDumper::visit(Directive* dire)
{
    dumpStr(dire->dump());
}

void 
AttDumper::visit(Inst* inst)
{
    dumpStr(inst->dump());
}

}
