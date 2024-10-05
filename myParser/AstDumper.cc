#include "AstDumper.hh"
#include "astPub.hh"
#include "type.hh"
#include <string>

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
    AstVisitor::visit(rt);
    postAction(rt);
}

void
AstDumper::visit(ExprStmt* expr)
{
    preAction(expr);
    AstVisitor::visit(expr);
    postAction(expr);
}

void
AstDumper::visit(MemberExpr* expr)
{
    preAction(expr);
    dumpStr("Basic Expr:");
    AstVisitor::visit(expr);
    dumpStr("Member: ");
    dumpStr(expr->getMember());
    postAction(expr);
}

void
AstDumper::visit(FuncDef* node)
{
    preAction(node);
    if (node->getRetType()) {
        dumpStr("RetType:");
        node->getRetType()->accept(this);
    }
    dumpStr("FuncName:");
    dumpStr(node->getName());
    for (auto param : node->getParams()) {
        dumpStr("Param:");
        param->accept(this);
    }
    if (node->isVarArg()) {
        ++_level;
        dumpStr("Var Args");
        --_level;
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
    dumpStr("StructName:" + node->getName());
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

    dumpStr("VarName:" + node->getId());

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
        dumpStr("To:");
        dumpStr(node->getDefinedType());

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
AstDumper::visit(InclStmt* node)
{
    preAction(node);
    dumpStr("file path: " + node->getFilePath());
    postAction(node);
}

void
AstDumper::visit(FuncCall* node)
{
    preAction(node);
    dumpStr("FuncName: " + node->funcName());
    if (!node->getParams().empty())
    {
        dumpStr("param:");
        for (auto expr : node->getParams())
            expr->accept(this);
    }
    if (node->getFuncDef()) {
        std::string defInfo = " refered to ";
        defInfo += "Line: ";
        defInfo += std::to_string(node->getFuncDef()->getLine());
        defInfo += ", Col: ";
        defInfo += std::to_string(node->getFuncDef()->getCol());
        defInfo += ".";
        dumpStr("");
    }
    postAction(node);
}


void
AstDumper::visit(Variable* node)
{
    dumpStr("Variable:");
    dumpStr(node->getId() + "");
    if (node->getDefine()) {
        std::string defInfo = " refered to ";
        defInfo += "Line: ";
        defInfo += std::to_string(node->getDefine()->getLine());
        defInfo += ", Col: ";
        defInfo += std::to_string(node->getDefine()->getCol());
        defInfo += ".";
        dumpStr(defInfo);
    }
}


void
AstDumper::visit(StrLiteral* node)
{
        dumpStr("String Literal:");
        dumpStr(node->getLiteral());
}
void
AstDumper::visit(IntLiteral* node)
{
        dumpStr("Int Literal:");
        dumpStr(std::to_string(node->getLiteral()));
}
void
AstDumper::visit(FloatLiteral* node)
{
        dumpStr("Float Literal:");
        dumpStr(std::to_string(node->getLiteral()));
}


void
AstDumper::visit(Parenthesed* node)
{
    dumpStr("Parenthesed Expr:");
    node->getExpr()->accept(this); 
}

void
AstDumper::visit(Keyword* node)
{
    dumpStr("Keyword: " + node->getKeyword());
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
    if (expr->getExpr()) {
        expr->getExpr()->accept(this);
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

    if (node->isResolved()) {
        dumpType(node->getType());
    } else {
        dumpTypeRef(node->getTypeRef());
    }

    postAction(node);
}

void    
AstDumper::dumpType(Type* type)
{
    std::string str = "Type: ";
    if (type->isConst()) {
        str += "const ";
    }

    if (type->isBasicType()) {
        switch (dynamic_cast<BasicType*>(type)->getTypeId()) {
        case BasicType::TypeId::VOID:
            str += "VOID";
            break;
        case BasicType::TypeId::CHAR:
            str += "CHAR";
            break;
        case BasicType::TypeId::SHORT:
            str += "SHORT";
            break;
        case BasicType::TypeId::INT:
            str += "INT";
            break;
        case BasicType::TypeId::FLOAT:
            str += "FLOAT";
            break;
        case BasicType::TypeId::DOUBLE:
            str += "DOUBLE";
            break;
        default:
            // exception
            break;
        }
        dumpStr(str);
    } else if (type->isPointerType()) {
        str += "Pointer of\n";
        dumpStr(str);
        PointerType* pointer = dynamic_cast<PointerType*>(type); 
        if (pointer->getBasicType()) {
            dumpType(pointer->getBasicType());
        } else if (pointer->getBasicTypeRef()) {
            dumpTypeRef(pointer->getBasicTypeRef());
        }
    } else if (type->isRefType()) {
        str += "Reference of\n ";
        dumpStr(str);
        RefType* ref = dynamic_cast<RefType*>(type); 
        if (ref->getBasicType()) {
            dumpType(ref->getBasicType());
        } else if (ref->getBasicTypeRef()) {
            dumpTypeRef(ref->getBasicTypeRef());
        }
    } else {
        // exception
    }
}

void    
AstDumper::dumpTypeRef(TypeRef* ref)
{
    std::string str = "TypeRef: ";
    str += ref->getName();
    dumpStr(str);
}

}
