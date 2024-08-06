#pragma once

#include "../myParser/AstVisitor.hh"
#include <map>

namespace SEMANTIC {

class TypeResolver : public SYNTAX::AstVisitor {
public:
void            resolve(SYNTAX::TreeNode* root) { root->accept(this); }
virtual void    visit(SYNTAX::StructDef*);
virtual void    visit(SYNTAX::TypeDef*);
virtual void    visit(SYNTAX::TypeNode*);

private:
// resolve defined or declared type
SYNTAX::StructDef*  resolveType(std::string name);

private:
std::map<std::string, SYNTAX::StructDef*> _declaredTypes;
std::map<std::string, SYNTAX::StructDef*> _definedTypes;
std::map<std::string, std::string>        _typeDefs; // refName: defName
};


}
