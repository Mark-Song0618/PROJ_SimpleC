PREFIX = ..
include ../setting.mk

myLexer.o: myLexer.cc lexPub.hh ../utils/Fsm.hh ../utils/Msg.hh 
	$(compileObj)

.PHONY: clean
clean:
	rm -rf *.o
