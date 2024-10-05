PREFIX = ..
include ../setting.mk

all: astDumper.o astVisitor.o ast.o astScope.o type.o myParser.o

astDumper.o: AstDumper.cc AstDumper.hh AstVisitor.hh 
	$(compileObj)

astVisitor.o: AstVisitor.cc AstVisitor.hh astPub.hh
	$(compileObj)

ast.o: ast.cc astPub.hh 
	$(compileObj)

astScope.o : Scope.cc Scope.hh
	$(compileObj)

type.o: type.cc type.hh $(DIR_UTL)/Exception.hh
	$(compileObj)

myParser.o: myParser.cc astPub.hh myParser.hh $(DIR_PRE)/myPreProc.hh
	$(compileObj)

.PHONY: clean
clean:
	rm -rf *.o
