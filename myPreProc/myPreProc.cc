#include "myPreProc.hh"
#include "../myLexer/lexPub.hh"
#include "../myParser/myParser.hh"

namespace PPROC
{

SYNTAX::StmtVec
MyPreProc::includeFile(const std::string& path)
{
    LEX::MyLexer lexer;
    lexer.scan(path);
    SYNTAX::MyParser parser(&lexer);
    parser.parse();
    SYNTAX::StmtVec ret = parser.getResult()->fetchStatements();
    delete parser.getResult();
    return std::move(ret);
}

}
