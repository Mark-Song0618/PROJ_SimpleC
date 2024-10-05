#include "entity.hh"


namespace IR
{

unsigned 
Function::getParamsSize()
{
    unsigned totalParamSize = 0;
    for (auto param : getParams()) {
        totalParamSize += param->size();
    }
    return totalParamSize;
}

}
