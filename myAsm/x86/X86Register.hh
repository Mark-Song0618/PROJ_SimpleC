#include "../myAsm.hh"
#include "../Register.hh"

namespace ASM
{

class X86Register : public Register 
{
public:
    enum RegisterType {
        AX,
        BX,
        CX,
        DX,
        BP,
        SP,
        SI,
        DI,
        IP,
        R8,
        R9,
    };

public:
    X86Register(RegisterType type, unsigned size = 4) : _type(type) { _size = Oper::getSize(size); }

    std::string     dump() override;

    std::string     basicStr();

    RegisterType    type() { return _type; }

    Oper::OperSize  size() override { return _size; }

    unsigned        byte(); 

    void            setSize(Oper::OperSize size) override { _size = size; }

private:
    RegisterType    _type;

    Oper::OperSize  _size;
};

}
