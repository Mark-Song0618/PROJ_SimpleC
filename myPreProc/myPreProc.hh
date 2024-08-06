#pragma once
#include "../myParser/astPub.hh"
namespace PPROC
{

// InclStmt     :=  <#> <include> <<> PATH <>> |

class MyPreProc
{
public:
    SYNTAX::StmtVec     includeFile(const std::string& path);
};

}
