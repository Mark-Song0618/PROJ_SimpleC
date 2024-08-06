#include "../myParser/myParser.hh"
#include "../myParser/AstDumper.hh"
#include "../myLexer/lexPub.hh"
#include "../mySemantic/IdResolver.hh"
#include "../mySemantic/typeResolver.hh"

int main(int argc, const char** argv)
{
    LEX::MyLexer lexer;

    lexer.scan("./tests/test.c"); 
    lexer.setOutput("./result.txt");
    LEX::Token token;
    SYNTAX::MyParser parser;
    parser.setLexer(&lexer);
    parser.parse(); 
  
    SEMANTIC::IdResolver idResolver;
    idResolver.resolve(parser.getResult());
    SEMANTIC::TypeResolver typeResolver;
    typeResolver.resolve(parser.getResult());

    SYNTAX::AstDumper dumper;
    dumper.compressExpr();
    dumper.dumpAst(parser.getResult(), "./ast.out");

   return 0;
}
