PREFIX = ..
include ../setting.mk
all: Imm.o asm.o asmGenerator.o asmDumper.o x86Reg.o x86Generator.o

Imm.o: Immediate.cc Immediate.hh $(DIR_IR)/Literal.hh
	$(compileObj)
asm.o: myAsm.cc myAsm.hh $(DIR_IR)/memRef.hh Immediate.hh AsmVisitor.hh
	$(compileObj)

asmGenerator.o: AsmGenerator.cc AsmGenerator.hh myAsm.hh $(DIR_IR)/ir.hh $(DIR_IR)/IRVisitor.hh 
	$(compileObj)

asmDumper.o: AsmDumper.cc AsmDumper.hh AsmVisitor.hh myAsm.hh 
	$(compileObj)

x86Reg.o: x86/X86Register.cc x86/X86Register.hh Register.hh $(DIR_IR)/Register.hh $(DIR_UTL)/Exception.hh $(DIR_IR)/Register.hh 
	$(compileObj)

x86Generator.o: x86/X86Generator.cc x86/X86Generator.hh x86/X86Register.hh Immediate.hh AsmGenerator.hh myAsm.hh $(DIR_IR)/Evaluator.hh $(DIR_UTL)/Exception.hh 
	$(compileObj)

.PHONY: clean
clean:
	rm -rf *.o
