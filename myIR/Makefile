PREFIX = ..
include ../setting.mk
irTargets: evaluator.o memRef.o label.o ir.o entity.o constEntry.o irGenerator.o irVisitor.o irDumper.o literal.o irScope.o 

evaluator.o: Evaluator.cc Evaluator.hh IRVisitor.hh $(DIR_UTL)/Exception.hh
	$(compileObj)

memRef.o: memRef.cc memRef.hh Register.hh
	$(compileObj)

label.o: label.cc label.hh
	$(compileObj)

ir.o: ir.cc ir.hh Scope.hh entity.hh constEntry.hh label.hh
	$(compileObj)

entity.o: entity.cc entity.hh memRef.hh Scope.hh
	$(compileObj)

constEntry.o: constEntry.cc constEntry.hh 
	$(compileObj)

irGenerator.o: IRGenerator.cc IRGenerator.hh ir.hh $(DIR_UTL)/Exception.hh $(DIR_SYN)/astPub.hh $(DIR_SYN)/AstVisitor.hh entity.hh
	$(compileObj)

irVisitor.o : IRVisitor.cc IRVisitor.hh ir.hh
	$(compileObj)

irDumper.o : IRDumper.cc IRDumper.hh IRVisitor.hh ir.hh
	$(compileObj)

literal.o: Literal.cc Literal.hh $(DIR_SYN)/type.hh
	$(compileObj)

irScope.o: Scope.cc Scope.hh entity.hh
	$(compileObj)

.PHONY: clean
clean:
	rm *.o -rf

