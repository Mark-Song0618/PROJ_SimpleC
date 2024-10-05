#include "label.hh"


namespace IR
{


Label::Label()
{
    _symbol = "L" + std::to_string(_tmpCnt++);
}

}
