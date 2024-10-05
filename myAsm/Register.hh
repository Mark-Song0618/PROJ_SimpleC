#include "../myIR/Register.hh"
#include "myAsm.hh"

namespace ASM
{

class Register : public IR::Register 
{
public:
    virtual Oper::OperSize  size() = 0; 

    virtual void            setSize(Oper::OperSize) = 0;

};
}
