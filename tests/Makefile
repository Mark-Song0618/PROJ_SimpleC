compile:
	gcc -S -fno-asynchronous-unwind-tables -fno-dwarf2-cfi-asm test2.c -o test2.s
build:
	gcc test2.s -o test2
.phony clean:
	rm test2 test2.s -f
