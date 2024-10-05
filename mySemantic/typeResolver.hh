#pragma once

#include "../myParser/AstVisitor.hh"

namespace SEMANTIC {

class TypeResolver : public SYNTAX::AstVisitor {
public:
void                resolve(SYNTAX::Program* root) { root->accept(this); }
virtual void        visit(SYNTAX::TypeNode*);
virtual void        visit(SYNTAX::TypeDef*);
virtual void        visit(SYNTAX::StructDef*);

private:
// resolve defined or declared type
SYNTAX::Type*       resolveType(std::string name);

private:
SYNTAX::TypeTable   _typeTable;
};


}
