#include "Evaluator.hh"
#include "../utils/Exception.hh"

namespace IR
{

int
Evaluator::evaluate(IRExpr* root)
{
    _value = 0;
    root->accept(this);
    return _value;
}


void 
Evaluator::visit(IRVar* var)
{
    if (var->getEntity()->isConst()) {
        evaluate(dynamic_cast<Constant*>(var->getEntity())->getInitVal());
    } else {
        evaluate(dynamic_cast<Variable*>(var->getEntity())->getInitVal());
    }
}

void 
Evaluator::visit(IRInt* val)
{
    _value = val->getValue();
}

void 
Evaluator::visit(IRAssign* assign)
{
    _value = evaluate(assign->getRight());
}

void 
Evaluator::visit(IRBinExpr* bin)
{
    int right = evaluate(bin->getRight());
    int left = evaluate(bin->getLeft());
    switch (bin->getOpType()) {
        case IRBinExpr::BinOpType::ADD:
            _value = left + right;
            break;
        case IRBinExpr::BinOpType::SUB:
            _value = left - right;
            break;
        case IRBinExpr::BinOpType::MUL:
            _value = left * right;
            break;
        case IRBinExpr::BinOpType::DIV:
            _value = left / right;
            break;
        case IRBinExpr::BinOpType::MOD:
            _value = left % right;
            break;
        default:
            throw UTIL::MyException("unsupported inti value.");
            break;
    }
}

void 
Evaluator::visit(IRUniExpr* expr)
{
    evaluate(expr->getExpr());
    switch(expr->getOpType()) {
        case IRUniExpr::UniOpType::NEG:
            _value = -_value;
            break;
        default:
            throw UTIL::MyException("unsupported inti value.");
            break;
    }
}

}
