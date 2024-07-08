compile = g++ -g

myLexer.o: ./myLexer/myLexer.cc ./myLexer/lexPub.hh ./utils/Fsm.hh ./utils/Msg.hh 
	$(compile) -c -o $@ $<	

testLexer: ./tests/testLexer.cc ./myLexer/lexPub.hh myLexer.o utilMsg.o
	$(compile) -o $@ ./tests/testLexer.cc myLexer.o utilMsg.o

utilMsg.o: ./utils/Msg.cc ./utils/Msg.hh
	$(compile) -c -o $@ $<


.PHONY:clean
clean:
	rm *.o
