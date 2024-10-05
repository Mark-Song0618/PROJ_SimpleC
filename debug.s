.file	"./test.c"
	.data
	.section .rodata
.LC0:
	.string	"Result: %d,%d "
	.text
	.globl	main
	.type	main,@function
main:
pushq	%rbp
movq	%rsp,	%rbp
subq	$16,	%rsp
movq	%rdi,	-4(%rbp)
movq	%rsi,	-12(%rbp)
movl	%eax,	-12(%rbp)
movl	%ecx,	-14(%rbp)
leal	-20(%rbp),	%eax
movl	%eax,	20(%rbp)
leaq	.LC0(%rip),	%rax
movq	%rax,	%rdi
movl	$0,	%eax
movq	%rax,	%rsi
movl	$0,	%eax
movq	%rax,	%rdx
callq	printf
movl	2(%rbp),	%ecx
movq	%rax,	0(%ecx)
movl	$0,	%eax
jmp 	.Lend_main
.Lend_main:
movl	-16(%rbp),	%ecx
movl	-14(%rbp),	%eax
movq	%rbp,	%rsp
popq	%rbp
ret
