#include <stdio.h>
#include "../myLexer/lexPub.hh"

#define SIMPLETEST

void usage() {
    printf("Usage:\n");
    printf("\t test srcfile [output]\n");
}

int main(const int argc, const char** argv) {
    LEX::MyLexer lexer;

#ifdef SIMPLETEST
   lexer.scan("./tests/test.c"); 
   lexer.output("./result.txt");
# else
    if (argc != 3) {
        usage();
        return -1;
    }
    if (lexer.scan(argv[1])) {
        return -2;
    }

    if (lexer.output(argv[2])) {
        return -3;
    }
#endif
    LEX::Token token;
    while (true) {
        token = lexer.nextToken();
        if (token.getType() == LEX::TokenType::FEOF) {
            return 0;
        }
        if (token.getType() == LEX::TokenType::BAD) {
            // report error
            return -4;
        }
    }
    return 0;
}
