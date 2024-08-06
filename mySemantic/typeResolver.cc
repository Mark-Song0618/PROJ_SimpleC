#include "typeResolver.hh"
#include "../utils/Exception.hh"

namespace SEMANTIC {

void 
TypeResolver::visit(SYNTAX::StructDef* def) {
    if (def->getId() == nullptr || def->getId()->getAtomType() != SYNTAX::AtomExpr::AtomType::Variable) {
        throw UTIL::MyException("wrong struct id");
    }   
    std::string typeName = def->getId()->getIdName();
    if (def->isDefined()) {
        _definedTypes.insert({typeName, def});
    } else {
        _declaredTypes.insert({typeName, def});
    }
}

void 
TypeResolver::visit(SYNTAX::TypeDef* node) {
    _typeDefs.insert({node->getDefinedType()->getIdName(),
                      node->getOrigType()->getTypeRefName()});
}
void 
TypeResolver::visit(SYNTAX::TypeNode* node) {
    if (node->getTypeId() != SYNTAX::TypeNode::TypeId::TypeRef)
        return;

    SYNTAX::StructDef* resolvedType = resolveType(node->getTypeRefName());
    if (!resolvedType) {
        throw UTIL::MyException("Resolve Type Failed.");
    } else {
        node->resolvedType(resolvedType);
    }
}

SYNTAX::StructDef*
TypeResolver::resolveType(std::string refName)
{
    std::string resolvedName = refName;
    while (_typeDefs.find(resolvedName) != _typeDefs.end()) {
        auto orig = _typeDefs[resolvedName];
        if (orig == refName) break;
        refName = orig;
    }
    if (_definedTypes.find(resolvedName) != _definedTypes.end()) {
        return _definedTypes[resolvedName];
    } else if (_declaredTypes.find(resolvedName) != _declaredTypes.end()) {
        return _declaredTypes[resolvedName];
    } else {
        return nullptr;
    }
}

}
