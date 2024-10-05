#pragma once

#include "../myParser/astPub.hh"

namespace SEMANTIC
{

class SemanticAnalyzer
{
public:
    void    analyze(SYNTAX::Program* root);

    void    resolveVariables(SYNTAX::Program* root);
    void    resolveTypeRefs(SYNTAX::Program* root);
    void    checkTypes(SYNTAX::Program* root);
    void    checkMisc(SYNTAX::Program* root);
};

}
