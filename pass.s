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
subq	$32,	%rsp
movq	%rdi,	-8(%rbp)
movq	%rsi,	-16(%rbp)
leaq	.LC0(%rip),	%rax
movq	%rax,	%rdi
movl	$0,	%eax
movq	%rax,	%rsi
movl	$0,	%eax
movq	%rax,	%rdx
call	printf@PLT
movl	$0,	%eax
movq	%rbp,	%rsp
popq	%rbp
ret
