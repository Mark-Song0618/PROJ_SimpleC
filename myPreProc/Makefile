PREFIX = ..
include ../setting.mk

myPProc.o: myPreProc.cc myPreProc.hh $(DIR_LEX)/lexPub.hh $(DIR_SYN)/astPub.hh $(DIR_SYN)/myParser.hh
	$(compileObj)

.PHONY: clean
clean:
	rm -rf *.o
