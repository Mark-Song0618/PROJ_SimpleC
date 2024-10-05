#include "../myParser/myParser.hh"
#include "../myParser/AstDumper.hh"
#include "../myLexer/lexPub.hh"
#include "../mySemantic/semanAnalyzer.hh"
#include "../myIR/IRGenerator.hh"
#include "../myIR/IRDumper.hh"
#include "../myAsm/x86/X86Generator.hh"
#include "../myAsm/AsmDumper.hh"

int main(int argc, const char** argv)
{
    LEX::MyLexer lexer;

    lexer.scan("./test.c"); 
    lexer.setOutput("./result.txt");
    LEX::Token token;
    SYNTAX::MyParser parser;
    parser.setLexer(&lexer);
    parser.parse(); 
  
    SEMANTIC::SemanticAnalyzer analyzer;
    analyzer.analyze(parser.getResult());

    SYNTAX::AstDumper dumper;
    dumper.dumpAst(parser.getResult(), "./ast.out");

    IR::IRGenerator irGen;
    IR::IR* ir = irGen.generate(parser.getResult());

    IR::IRDumper irDumper;
    irDumper.dump(ir, "./ir.out");

    ASM::X86Generator asmGen;
    auto asmCode = asmGen.generate(ir);
    ASM::AttDumper attDumper;
    attDumper.dump(asmCode, "./test.s");

   return 0;
}
