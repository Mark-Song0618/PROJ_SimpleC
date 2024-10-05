#include "typeResolver.hh"
#include "../utils/Exception.hh"

namespace SEMANTIC {

void 
TypeResolver::visit(SYNTAX::TypeNode* node) {
    if (node->isResolved())
        return;

    if (!node->getType()) {
        SYNTAX::Type* resolvedType = resolveType(node->getTypeRef()->getName());
        if (!resolvedType) {
            throw UTIL::MyException("Resolve Type Failed.");
        } else {
            node->setType(resolvedType);
        }
    } else {
        // for type* or type&, we should resolve type for basicType
        SYNTAX::Type* type = node->getType();
        while (type->getBasicType()) {
            type = type->getBasicType();
        }
        SYNTAX::Type* resolvedType = resolveType(type->getBasicTypeRef()->getName());
        if (!resolvedType) {
            throw UTIL::MyException("Resolve Type Failed.");
        } else {
            if (type->isPointerType()) {
                dynamic_cast<SYNTAX::PointerType*>(type)->setBasicType(resolvedType);
            } else if (type->isRefType()) {
                dynamic_cast<SYNTAX::PointerType*>(type)->setBasicType(resolvedType);
            }
        }
    }
}

void    
TypeResolver::visit(SYNTAX::StructDef* def)
{
    std::string typeName = "struct " + def->getName();
    _typeTable.addType(typeName, def->extractType());
}

void    
TypeResolver::visit(SYNTAX::TypeDef* def) 
{
    std::string definedTypeName = def->getDefinedType();
    SYNTAX::TypeNode* origType = def->getOrigType();
    if (origType->isResolved()) {
        _typeTable.addType(definedTypeName, origType->getType());
    } else {
        std::string basicRefName = origType->getTypeRef()->getName();
        SYNTAX::Type* resolvedType= _typeTable.resolve(basicRefName);
        if (resolvedType == nullptr) {
            std::string err = "Use Undeclared Type: " + basicRefName;
            throw UTIL::MyException(err.c_str());
        } else {
            _typeTable.addType(definedTypeName, resolvedType);
        }
    }
}

SYNTAX::Type*
TypeResolver::resolveType(std::string refName)
{
    return _typeTable.resolve(refName);
}

}
