compile = g++ -g
compileObj = g++ -g -c -o $@ $<

testLexer: ./tests/testLexer.cc myLexer.o utilMsg.o
	$(compile) -o $@ $^

testParser: ./tests/testParser.cc ./myParser/myParser.hh ./myParser/AstDumper.hh ./myLexer/lexPub.hh ./utils/Exception.hh \
			myLexer.o myParser.o myPProc.o astDumper.o astVisitor.o utilMsg.o ast.o IdResolver.o TypeResolver.o scope.o
	$(compile) -o $@ ./tests/testParser.cc myLexer.o myParser.o astDumper.o astVisitor.o utilMsg.o ast.o myPProc.o IdResolver.o TypeResolver.o scope.o 

utilMsg.o: ./utils/Msg.cc ./utils/Msg.hh
	$(compileObj)

myLexer.o: ./myLexer/myLexer.cc ./myLexer/lexPub.hh ./utils/Fsm.hh ./utils/Msg.hh 
	$(compileObj)

myParser.o: ./myParser/myParser.cc ./myParser/astPub.hh ./myParser/myParser.hh
	$(compileObj)

myPProc.o: ./myPreProc/myPreProc.cc ./myPreProc/myPreProc.hh ./myLexer/lexPub.hh ./myParser/astPub.hh ./myParser/myParser.hh
	$(compileObj)

IdResolver.o : ./mySemantic/IdResolver.cc ./mySemantic/IdResolver.hh ./myParser/astPub.hh ./myParser/AstVisitor.hh
	$(compileObj)

TypeResolver.o : ./mySemantic/typeResolver.cc ./mySemantic/typeResolver.hh ./myParser/AstVisitor.hh ./utils/Exception.hh
	$(compileObj)

ast.o: ./myParser/ast.cc ./myParser/astPub.hh 
	$(compileObj)

scope.o : ./myParser/Scope.cc ./myParser/Scope.hh
	$(compileObj)

astDumper.o: ./myParser/AstDumper.cc ./myParser/AstDumper.hh ./myParser/AstVisitor.hh 
	$(compileObj)

astVisitor.o: ./myParser/AstVisitor.cc ./myParser/AstVisitor.hh ./myParser/astPub.hh
	$(compileObj)

.PHONY:clean
clean:
	rm *.o
