#include "ir.hh"
#include "Scope.hh"

namespace IR
{

IR::IR() { _topLevelScope = new Scope(); }

std::string     
IRBinExpr::getOpStr()
{
    switch (_opType) {
    case BinOpType::ADD:
        return "ADD";
    case BinOpType::SUB:
        return "SUB";
    case BinOpType::MUL:
        return "MUL";
    case BinOpType::DIV:
        return "DIV";
    case BinOpType::MOD:
        return "MOD";
    case BinOpType::SHL:
        return "SHL";
    case BinOpType::SHR:
        return "SHR";
    case BinOpType::BITAND:
        return "BIT AND";
    case BinOpType::BITOR:
        return "BIT OR";
    case BinOpType::BITXOR:
        return "BIT XOR";
    case BinOpType::EQ:
        return "EQ";
    case BinOpType::NE:
        return "NE";
    case BinOpType::LT:
        return "LT";
    case BinOpType::LE:
        return "LE";
    case BinOpType::GT:
        return "GT";
    case BinOpType::GE:
        return "GE";
    case BinOpType::UNSUPPORTED:
        return "";
    }
    return "";
}

std::string     
IRUniExpr::getOpStr()
{
    switch (_opType) {
    case UniOpType::NEG:
        return "NEG";
    case UniOpType::BITNOT:
        return "BIT NOT";
    case UniOpType::LOGICNOT:
        return "LOGIC NOT";
    case UniOpType::UNSUPPORTED:
        return "";
    }
    return "";
}

std::vector<Variable*>  
IR::getGlobVariable()
{
    return _topLevelScope->getVariables();
}

std::vector<Constant*>
IR::getGlobConst()
{
    return _topLevelScope->getConstants();
}

}
