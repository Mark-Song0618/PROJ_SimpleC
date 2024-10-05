#pragma once
#include <string>
#include <map>
#include <vector>

#define PTR_SIZE 8

/**
 *
 *      This file defines the type representation in compiler.
 *
 *      Type is to describe the memory structure of an entity, and has different forms in compile stage.
 *      .
 *      Firstly, type is parsed and stored in the TypeNode of an AST. 
 *      If it is basic type, like bool, char, int .etc, It is directely stored as a Type.
 *      If it is a user defiend struct name, it is stored as TypeRef. TypeRef means a 'ref' to a Type. And will be resolved later.
 *
 *      Then, semantic analyze will resolve the TypeRef to the Type by their symbol. And necessary type check will be perfomed. 
 *
 *      When it comes to IR, one special thing related with type is member operation. The member should be converted to an offset to the entity.
 *      The offset is calculated by the type of the entity. The other special thing with type is arithematic operation. The operand should be multipled by
 *      the sizeof type in some situations.
 *
 *      After IR, the type is useless. But the size of operand is used.Because the instructions need to know the size of operand.
 *
 * */

namespace SYNTAX {

class TypeRef;
class Type {
public:
    virtual bool        isBasicType() { return false; }
    virtual bool        isPointerType() { return false; }
    virtual bool        isRefType() { return false; }
    virtual bool        isArrayType() { return false; }
    virtual bool        isStructType() { return false; }
    virtual Type*       getBasicType() { return nullptr; }
    virtual TypeRef*    getBasicTypeRef() { return nullptr; }

    std::string         typeName() { return _name; } 
    void                setTypeName(std::string name) { _name = name; } 

    bool                isConst() { return _isConst;}
    void                setConst(bool isConst) { _isConst = isConst;}

    // todo: depend on platform
    virtual unsigned    size() = 0;

private:
    bool                _isConst;

    std::string         _name;
};

class BasicType : public Type 
{

public:
    enum class TypeId {
        VOID,
        CHAR,
        SHORT,
        INT,
        FLOAT,
        DOUBLE,
    };

public:
    BasicType(TypeId id); 

    bool                isBasicType() override { return true; }

    TypeId              getTypeId() { return _id; }

    unsigned            size() override;

private:
    TypeId              _id;

};

class StructType : public Type 
{
public:
    StructType(std::string name) { setTypeName(name); }

    bool                isStructType() override { return true; }

    bool                isMember(const std::string& name);

    void                addMember(const std::string& memberName, Type* memberType); 

    unsigned            getMemberOffset(std::string);

    Type*               memberType(std::string);

    unsigned            size() override;

private:
    std::vector<Type*>          _memTypes;

    std::vector<std::string>    _memNames;
};

class PointerType : public Type 
{
public:
    PointerType(Type* basicType) : _basicType(basicType) {}

    PointerType(TypeRef* basicType) : _basicRef(basicType) {}

    bool                isPointerType() override { return true; }

    Type*               getBasicType() override { return _basicType; }

    // for type resolving
    void                setBasicType(Type* basic) { _basicType = basic; }

    TypeRef*            getBasicTypeRef() override { return _basicRef; }

    unsigned            size() override { return PTR_SIZE; }

private:
    Type*               _basicType;

    TypeRef*            _basicRef;
};

class RefType : public Type 
{
public:
    RefType(Type* basicType) : _basicType(basicType) {}

    RefType(TypeRef* basicType) : _basicRef(basicType) {}

    bool                isRefType() override { return true; }

    Type*               getBasicType() override { return _basicType; }

    // for type resolving
    void                setBasicType(Type* basic) { _basicType = basic; }

    TypeRef*            getBasicTypeRef() override { return _basicRef; }

    unsigned            size() override { return _basicType->size(); }

private:
    Type*               _basicType;

    TypeRef*            _basicRef;

};

class ArrayType : public Type
{
public:
    ArrayType(Type* basic, unsigned size) : _basicType(basic), _elemCnt(size) {}

    ArrayType(TypeRef* basic, unsigned size) : _basicRef(basic), _elemCnt(size) {}

    Type*               getBasicType() override { return _basicType; }

    TypeRef*            getBasicTypeRef() override { return _basicRef; }

    bool                isArrayType() override { return true; }
    
    unsigned            size() override { return PTR_SIZE; }

private:
    Type*               _basicType;

    TypeRef*            _basicRef;

    unsigned            _elemCnt;
};

class TypeRef
{
public:
    TypeRef(std::string name) : _name(name) {}

    std::string         getName() { return _name; }

    bool                isConst() { return _isConst;}

    void                setConst(bool isConst) { _isConst = isConst;}

    unsigned            size(); 

private:
    std::string         _name;

    bool                _isConst;
};

class TypeTable {
public:
    static void                 init(); 

    static void                 addType(std::string name, Type* type);

    static Type*                resolve(const std::string& refName);

    static Type*                getType(std::string name); 

    static Type*                getPointerType(Type* basicType);

    static std::string          getPointerTypeName(Type*);

private:
    static std::map<std::string, Type*>    _definedTypes;

    inline static bool          _inited = false;
};


}
