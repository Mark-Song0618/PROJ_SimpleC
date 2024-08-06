#include "AstDumper.hh"
#include "astPub.hh"

namespace SYNTAX {

template<>
std::string
AstDumper::getDumpStr(BinOpExpr* expr)
{
    std::string rt(_level*_tab, ' ');
    rt += expr->getClassInfo();
    rt += ". Type: ";
    switch(expr->getBinType()) {
    case BinOpExpr::BinOpType::ASSIGN:
        rt += "ASSIGN.";
        break;
    case BinOpExpr::BinOpType::OR:
        rt += "LOGIC OR.";
        break;
    case BinOpExpr::BinOpType::AND:
        rt += "LOGIC AND.";
        break;
    case BinOpExpr::BinOpType::BITOR:
        rt += "BIT OR.";
        break;
    case BinOpExpr::BinOpType::BITAND:
        rt += "BIT AND.";
        break;
    case BinOpExpr::BinOpType::BITXOR:
        rt += "BIT XOR.";
        break;
    case BinOpExpr::BinOpType::DOT:
        rt += "DOT.";
        break;
    case BinOpExpr::BinOpType::POINTER:
        rt += "POINTER.";
        break;
    case BinOpExpr::BinOpType::SHR:
        rt += "SHIFT RIGHT.";
        break;
    case BinOpExpr::BinOpType::SHL:
        rt += "SHIFT LEFT.";
        break;
    case BinOpExpr::BinOpType::ADD:
        rt += "ADD.";
        break;
    case BinOpExpr::BinOpType::MINUS:
        rt += "MINUS.";
        break;
    case BinOpExpr::BinOpType::DIV:
        rt += "DIV.";
        break;
    case BinOpExpr::BinOpType::MOD:
        rt += "MOD.";
        break;
    case BinOpExpr::BinOpType::MULTI:
        rt += "MULTI.";
        break;
    case BinOpExpr::BinOpType::LESS:
        rt += "LESS.";
        break;
    case BinOpExpr::BinOpType::LESSE:
        rt += "LESS EQUAL.";
        break;
    case BinOpExpr::BinOpType::LARGER:
        rt += "LARGER.";
        break;
    case BinOpExpr::BinOpType::LARGERE:
        rt += "LARGER EQUAL.";
        break;
    case BinOpExpr::BinOpType::EQUAL:
        rt += "EQUAL.";
        break;
    case BinOpExpr::BinOpType::NOTEQUAL:
        rt += "NOTEQUAL.";
        break;
    default:
        break;
    }
    return std::move(rt);
}
void
AstDumper::dumpAst(TreeNode* root, std::string path)
{
    _level = 0;
    if (path != "") {
        _output = std::ofstream(path, std::ios::app);
    } 

    root->accept(this);

    if (path != "") {
        _output.close();
    }
}

void    
AstDumper::dumpStr(const char* str)
{
    std::string rt(_level*_tab, ' ');
    rt += str;
    _output << rt << std::endl;
}

void    
AstDumper::dumpStr(const std::string& str)
{
    std::string rt(_level*_tab, ' ');
    rt += str;
    _output << rt << std::endl;
}

void
AstDumper::visit(ReturnStmt* rt)
{
    preAction(rt);
    if (rt->getRetExpr()) {
        dumpStr("Return Expr:");
        rt->getRetExpr()->accept(this);
    }
    postAction(rt);
}

void
AstDumper::visit(FuncDef* node)
{
    preAction(node);
    if (node->getRtType()) {
        dumpStr("RetType:");
        node->getRtType()->accept(this);
    }
    if (node->getId()) {
        dumpStr("FuncName:");
        node->getId()->accept(this);
    }
    for (auto param : node->getParams()) {
        dumpStr("Param:");
        param->accept(this);
    }
    if (!node->getStmts().empty())
    {
        dumpStr("FuncBody:");
        for (auto stmt : node->getStmts()) {
            stmt->accept(this);
        }
    }
    postAction(node);
}

void
AstDumper::visit(StructDef* node)
{
    preAction(node);
    if (node->getId()) {
        dumpStr("StructName:");
        node->getId()->accept(this);
    } 
    if (!node->getmembers().empty()) {
        dumpStr("Struct Member:");
        for (auto mem: node->getmembers()) {
            mem->accept(this);
        }
    }
    postAction(node);
}

void
AstDumper::visit(VarDef* node)
{
    preAction(node);
    if (node->getTypeNode()) {
        dumpStr("VarType:");
        node->getTypeNode()->accept(this);
    }

    if (node->getId()) {
        dumpStr("VarName:");
        node->getId()->accept(this);
    }

    if (node->getInit()) {
        dumpStr("Var InitVal:");
        node->getInit()->accept(this);
    }
    postAction(node);
}

void
AstDumper::visit(TypeDef* node)
{
    preAction(node);
    if (node->getOrigType()) {
        dumpStr("Defined:");
        node->getOrigType()->accept(this);
        if (node->getDefinedType()) {
            dumpStr("To:");
            node->getDefinedType()->accept(this);
        }
    }
    postAction(node);
}

void
AstDumper::visit(IfStmt* node)
{
    preAction(node);
    if (node->getCondition()) {
        dumpStr("Condition:");
        node->getCondition()->accept(this);
    }
    if (!node->getStatements().empty()) {
        dumpStr("Then Stmt:");
        for (auto stmt : node->getStatements()) {
            stmt->accept(this);
        }
    }
    if (!node->getElseStmts().empty()) {
        dumpStr("Else Stmt:");
        for (auto stmt : node->getElseStmts()) {
            stmt->accept(this);
        }
    }
    postAction(node);
}

void
AstDumper::visit(ForStmt* node)
{
    preAction(node);
    if (!node->getInit().empty()) {
        dumpStr("Init Stmt:");
        for (auto init : node->getInit()) {
            init->accept(this);
        }
    }
    if(node->getCondition()) {
        dumpStr("Condition:");
        node->getCondition()->accept(this);
    }
    if (!node->getSteps().empty()) {
        dumpStr("Step:");
        for (auto stmt: node->getSteps()) {
            stmt->accept(this);
        }
    }
    if (!node->getBody().empty()) {
        dumpStr("Body:");
        for (auto stmt: node->getBody()) {
            stmt->accept(this);
        }
    }
    postAction(node);
}

void
AstDumper::visit(AssignStmt* node)
{
    if (!node->getLhs()) return;
    if (!node->getRhs()) {
        node->getLhs()->accept(this);
        return;
    }
    preAction(node);
    dumpStr("Assign:");
    node->getLhs()->accept(this);
    dumpStr("To:");
    node->getRhs()->accept(this);
    postAction(node);
}

void
AstDumper::visit(FuncCall* node)
{
    preAction(node);
    dumpStr("FuncName: " + node->getFuncName());
    if (!node->getParams().empty())
    {
        dumpStr("param:");
        for (auto expr : node->getParams())
            expr->accept(this);
    }
    postAction(node);
}

void
AstDumper::visit(InclStmt* node)
{
    preAction(node);
    dumpStr("file path: " + node->getFilePath());
    postAction(node);
}

void
AstDumper::visit(AtomExpr* node)
{
    switch (node->getAtomType()) {
    case AtomExpr::AtomType::Variable:
        dumpStr("Variable:");
        dumpStr(node->getIdName() + "");
        if (node->getVarDef()) {
            std::string defInfo = " refered to ";
            defInfo += "Line: ";
            defInfo += std::to_string(node->getVarDef()->getLine());
            defInfo += ", Col: ";
            defInfo += std::to_string(node->getVarDef()->getCol());
            defInfo += ".";
            dumpStr(defInfo);
        }
        break;
    case AtomExpr::AtomType::FuncCall:
        node->getFuncCall()->accept(this); 
        if (node->getFuncDef()) {
            std::string defInfo = " refered to ";
            defInfo += "Line: ";
            defInfo += std::to_string(node->getFuncDef()->getLine());
            defInfo += ", Col: ";
            defInfo += std::to_string(node->getFuncDef()->getCol());
            defInfo += ".";
            dumpStr("");
        }
        break;
    case AtomExpr::AtomType::StrLiteral:
        dumpStr("String Literal:");
        dumpStr(node->getString() + "");
        break;
    case AtomExpr::AtomType::FloatLiteral:
        dumpStr("Float Literal:");
        dumpStr(std::to_string(node->getFloat()) + "");
        break;
    case AtomExpr::AtomType::Parenthesed:
        dumpStr("Parenthesed Expr:");
        node->getParenthesed()->accept(this); 
        break;
    case AtomExpr::AtomType::IntLiteral:
        dumpStr("Int Literal:");
        dumpStr(std::to_string(node->getInt()) + "");
        break;
    default:
        break;
    }
}

void
AstDumper::visit(UniOpExpr* expr)
{
    switch(expr->getUniType()) {
    case UniOpExpr::UniOpType::PREINC:
        dumpStr("Prefix Inc:");
        break;
    case UniOpExpr::UniOpType::POSTINC:
        dumpStr("Postfix Inc:");
        break;
    case UniOpExpr::UniOpType::PREDEC:
        dumpStr("Prefix Dec:");
        break;
    case UniOpExpr::UniOpType::POSTDEC:
        dumpStr("Postfix Dec:");
        break;
    case UniOpExpr::UniOpType::NEG:
        dumpStr("Negtive:");
        break;
    case UniOpExpr::UniOpType::BITNOT:
        dumpStr("Bit Not:");
        break;
    case UniOpExpr::UniOpType::LOGICNOT:
        dumpStr("Logic Not:");
        break;
    case UniOpExpr::UniOpType::GETADDR:
        dumpStr("Get Address of:");
        break;
    case UniOpExpr::UniOpType::RESOLVEADDR:
        dumpStr("Resolve Address of:");
        break;
    default:
        break;
    }
    if (expr->getFactor()) {
        expr->getFactor()->accept(this);
    }
}

void
AstDumper::visit(BinOpExpr* expr)
{
    if (!expr->getLhs()) return;
    if (!expr->getRhs()) {
        expr->getLhs()->accept(this);
        return;
    }
    preAction(expr);
    dumpStr("Left:");
    expr->getLhs()->accept(this);
    dumpStr("Right:");
    expr->getRhs()->accept(this);
    postAction(expr);
}

void
AstDumper::visit(TypeNode* node)
{
    preAction(node);
    std::string str = "Type:";
    if (node->isLowConst()) {
        str += " Const";
    }
    switch (node->getTypeId()) {
    case TypeNode::TypeId::TypeRef:
        str += node->getTypeRefName();
        if (node->getTypeDef()) {
            str = "refered to typeDef: ";
            str += "Line: ";
            str += std::to_string(node->getTypeDef()->getLine());
            str += ", Col: ";
            str += std::to_string(node->getTypeDef()->getCol());
        }
        break;
    case TypeNode::TypeId::VOID:
        str += " void";
        break;
    case TypeNode::TypeId::BOOL:
        str += " bool";
        break;
    case TypeNode::TypeId::CHAR:
        str += " char";
        break;
    case TypeNode::TypeId::SHORT:
        str += " short";
        break;
    case TypeNode::TypeId::INT:
        str += " int";
        break;
    case TypeNode::TypeId::FLOAT:
        str += " float";
        break;
    case TypeNode::TypeId::DOUBLE:
        str += " double";
        break;
    case TypeNode::TypeId::Ref:
        if (node->isTopConst()) 
            str += "Const";
        str += " ref";
        break;
    case TypeNode::TypeId::Pointer:
        if (node->isTopConst()) 
            str += "Const";
        str += " pointer";
        break;
    default:
        break;
    }
    dumpStr(str);

    if (node->getTypeId() == TypeNode::TypeId::Ref) {
        if (node->getBasicType())
            node->getBasicType(true)->accept(this);
    }
    if (node->getTypeId() == TypeNode::TypeId::Pointer) {
        if (node->getBasicType())
            node->getBasicType(true)->accept(this);
    }

    postAction(node);
}
}
