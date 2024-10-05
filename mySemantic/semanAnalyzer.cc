#include "semanAnalyzer.hh"
#include "IdResolver.hh"
#include "typeResolver.hh"
#include "TypeChecker.hh"
#include "MiscSemantic.hh"

namespace SEMANTIC
{

void 
SemanticAnalyzer::analyze(SYNTAX::Program* root)
{
    resolveVariables(root);
    resolveTypeRefs(root);
    checkTypes(root);
    checkMisc(root);
}

void
SemanticAnalyzer::resolveVariables(SYNTAX::Program* root)
{
    IdResolver resolver;
    resolver.resolve(root);
}

void    
SemanticAnalyzer::resolveTypeRefs(SYNTAX::Program* root)
{
    TypeResolver resolver;
    resolver.resolve(root);
}

void    
SemanticAnalyzer::checkTypes(SYNTAX::Program* root)
{
    TypeChecker checker;
    checker.check(root);
}

void    
SemanticAnalyzer::checkMisc(SYNTAX::Program* root)
{
    MiscSemanChecker checker;
    checker.check(root);
}

}
