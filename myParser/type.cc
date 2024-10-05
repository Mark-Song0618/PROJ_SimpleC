#include "type.hh"
#include "../utils/Exception.hh"
#include <algorithm>

namespace SYNTAX {

std::map<std::string, Type*>
TypeTable::_definedTypes;

BasicType::BasicType(TypeId id) : _id(id)
{
    switch (id) {
    case TypeId::VOID:
        setTypeName("void");
        break;
    case TypeId::CHAR:
        setTypeName("char");
        break;
    case TypeId::SHORT:
        setTypeName("short");
        break;
    case TypeId::INT:
        setTypeName("int");
        break;
    case TypeId::FLOAT:
        setTypeName("float");
        break;
    case TypeId::DOUBLE:
        setTypeName("double");
        break;
    }
}

unsigned 
BasicType::size()
{
    switch (_id) {
    case TypeId::VOID:
        return 0;
    case TypeId::CHAR:
        return 1;
    case TypeId::SHORT:
        return 2;
    case TypeId::INT:
        return 4;
    case TypeId::FLOAT:
        return 4;
    case TypeId::DOUBLE:
        return 8;
    }
    return 0;
}

bool
StructType::isMember(const std::string& name)
{
    if (std::find(_memNames.begin(), _memNames.end(), name) != _memNames.end()) {
        return true;
    }
    return false;
}

void            
StructType::addMember(const std::string& memberName, Type* memberType)
{
    if (isMember(memberName)) {
        std::string error = "Duplicated Member: " + memberName;
        throw UTIL::MyException(error.c_str());
    }
    _memNames.push_back(memberName);
    _memTypes.push_back(memberType);
}

unsigned 
StructType::getMemberOffset(std::string member)
{
   if (!isMember(member)) {
        std::string error = member + "is Not a Member.";
        throw UTIL::MyException(error.c_str());
   }
   unsigned totalOffset = 0;
   for (int idx = 0; idx != _memTypes.size(); ++ idx) {
        if (_memNames[idx] == member) {
            return totalOffset;
        } else {
            unsigned typeSize = _memTypes[idx]->size();
            if (totalOffset % typeSize != 0) {
                totalOffset = totalOffset / typeSize * typeSize + typeSize;
            } 
            totalOffset += typeSize;
        }
   }
   return 0; // won't reach here
}

Type*           
StructType::memberType(std::string member)
{
    if (!isMember(member)) {
        std::string error = member + "is Not a Member.";
        throw UTIL::MyException(error.c_str());
    }
    for (int idx = 0; idx != _memTypes.size(); ++ idx) {
        if (_memNames[idx] == member) {
            return _memTypes[idx];
        }        
    } 

    return nullptr; // won't reach here
}

unsigned
StructType::size()
{
    unsigned totalSize = 0;
    for (auto type : _memTypes) {
        unsigned typeSize = type->size();
        if (totalSize % typeSize != 0) {
            totalSize = totalSize / typeSize * typeSize + typeSize;
        } 
        totalSize += typeSize;
    }
    return totalSize;
}

unsigned
TypeRef::size()
{
    Type* type = TypeTable::resolve(_name);
    if (!type) {
        throw UTIL::MyException("Not defined");
    }
    return type->size();
}

void    
TypeTable::init() 
{ 
    if (_inited) return;
    _inited = true;
    _definedTypes.clear(); 
    addType("void", new BasicType(BasicType::TypeId::VOID));
    addType("char", new BasicType(BasicType::TypeId::CHAR));
    addType("short", new BasicType(BasicType::TypeId::SHORT));
    addType("int", new BasicType(BasicType::TypeId::INT));
    addType("float", new BasicType(BasicType::TypeId::FLOAT));
    addType("double", new BasicType(BasicType::TypeId::DOUBLE));

    // todo: config basic type size
}

void
TypeTable::addType(std::string typeName, Type* type) {
    if (_definedTypes.find(typeName) != _definedTypes.end()) {
        std::string error = "redefined type: " + typeName; 
        throw UTIL::MyException(error.c_str());
    } else {
        _definedTypes.insert({typeName, type});
    }
}

Type*   
TypeTable::resolve(const std::string& refName)
{
    if (_definedTypes.find(refName) != _definedTypes.end()) {
        return _definedTypes[refName];
    } else {
        return nullptr;
    }
}

Type*
TypeTable::getType(std::string name)
{
    init();
    return _definedTypes[name];
}

Type*
TypeTable::getPointerType(Type* basicType)
{
    init();
    std::string typeName = getPointerTypeName(basicType);
    if (_definedTypes.find(typeName) != _definedTypes.end()) {
        return _definedTypes[typeName];
    }
    PointerType* type = new PointerType(basicType);
    addType(getPointerTypeName(basicType), type);
    return type;
}

std::string     
TypeTable::getPointerTypeName(Type* basicType)
{
    return "p_" + basicType->typeName(); 
}

}
