#pragma once

#include <string>

namespace IR
{

class Label {
public:
    Label(std::string label) : _symbol(label) {}
    Label();
    std::string             symbol() { return _symbol; }

private:
    std::string             _symbol;
    inline static int       _tmpCnt = 0;
};

}
