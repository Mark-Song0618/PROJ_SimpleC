PREFIX = .
include setting.mk

# targets
synObjs = $(DIR_SYN)/astDumper.o $(DIR_SYN)/astVisitor.o  $(DIR_SYN)/ast.o  $(DIR_SYN)/astScope.o $(DIR_SYN)/type.o $(DIR_SYN)/myParser.o
irObjs = $(DIR_IR)/evaluator.o $(DIR_IR)/memRef.o $(DIR_IR)/label.o $(DIR_IR)/ir.o $(DIR_IR)/entity.o $(DIR_IR)/constEntry.o $(DIR_IR)/irScope.o $(DIR_IR)/irGenerator.o $(DIR_IR)/irVisitor.o $(DIR_IR)/irDumper.o $(DIR_IR)/literal.o
asmObjs = $(DIR_ASM)/asm.o $(DIR_ASM)/asmGenerator.o $(DIR_ASM)/asmDumper.o $(DIR_ASM)/x86Reg.o $(DIR_ASM)/x86Generator.o $(DIR_ASM)/Imm.o
semanObjs = $(DIR_SEM)/idResolver.o $(DIR_SEM)/typeResolver.o $(DIR_SEM)/typeChecker.o $(DIR_SEM)/miscSemantic.o $(DIR_SEM)/semanAnalyzer.o
lexObjs = $(DIR_LEX)/myLexer.o
utilObjs = $(DIR_UTL)/utilMsg.o
preObjs = $(DIR_PRE)/myPProc.o

testAll: $(DIR_TEST)/testAll.cc $(DIR_SYN)/myParser.hh $(DIR_SYN)/AstDumper.hh $(DIR_LEX)/lexPub.hh $(DIR_UTL)/Exception.hh $(DIR_IR)/IRDumper.hh \
			lexer parser preProcess semantic ir Asm utils 
	$(compile) -o $@ $(DIR_TEST)/testAll.cc $(preObjs) $(irObjs) $(asmObjs) $(utilObjs) $(lexObjs) $(synObjs) $(semanObjs)

testLexer: $(DIR_TEST)/testLexer.cc lexer utils 
	$(compile) -o $@ $(DIR_TEST)/testLexer.cc $(DIR_LEX)/myLexer.o $(DIR_UTL)/utilMsg.o


.PHONY: lexer
lexer:
	make -C $(DIR_LEX)

.PHONY: parser 
parser:
	make -C $(DIR_SYN)

.PHONY: semantic 
semantic:
	make -C $(DIR_SEM)

.PHONY: preProcess
preProcess:
	make -C $(DIR_PRE)

.PHONY: ir
ir:
	make -C $(DIR_IR)

.PHONY:Asm
Asm:
	make -C $(DIR_ASM)

.PHONY: utils
utils:
	make -C $(DIR_UTL)

.PHONY:clean
clean:
	for dir in $(DIR_LEX) $(DIR_SYN) $(DIR_SEM) $(DIR_PRE) $(DIR_IR) $(DIR_ASM) $(DIR_UTL); do \
		make -C $$dir clean; \
	done
	rm -rf testAll testLexer
