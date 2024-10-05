PREFIX = ..
include ../setting.mk

all: idResolver.o typeResolver.o typeChecker.o miscSemantic.o semanAnalyzer.o

idResolver.o: IdResolver.cc IdResolver.hh $(DIR_SYN)/astPub.hh $(DIR_SYN)/AstVisitor.hh $(DIR_UTL)/Exception.hh
	$(compileObj) 

typeResolver.o: typeResolver.cc typeResolver.hh $(DIR_SYN)/AstVisitor.hh $(DIR_UTL)/Exception.hh
	$(compileObj) 

typeChecker.o: TypeChecker.cc TypeChecker.hh $(DIR_SYN)/AstVisitor.hh $(DIR_UTL)/Exception.hh
	$(compileObj) 

miscSemantic.o: miscSemantic.cc miscSemantic.hh $(DIR_SYN)/astPub.hh $(DIR_SYN)/AstVisitor.hh $(DIR_UTL)/Exception.hh
	$(compileObj) 

semanAnalyzer.o: semanAnalyzer.cc semanAnalyzer.hh $(DIR_SYN)/astPub.hh IdResolver.hh MiscSemantic.hh TypeChecker.hh typeResolver.hh 
	$(compileObj) 

.PHONY: clean
clean:
	rm -rf *.o
