compile = g++ -g
compileObj = g++ -g -c -o $@ $<

# directories
DIR_LEX  = $(PREFIX)/myLexer
DIR_SYN  = $(PREFIX)/myParser
DIR_SEM  = $(PREFIX)/mySemantic
DIR_PRE  = $(PREFIX)/myPreProc
DIR_IR   = $(PREFIX)/myIR
DIR_ASM  = $(PREFIX)/myAsm
DIR_TEST = $(PREFIX)/tests
DIR_UTL  = $(PREFIX)/utils

