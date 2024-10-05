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
subq	$48,	%rsp
movq	%rdi,	-8(%rbp)
movq	%rsi,	-16(%rbp)
movq    %rax,   -24(%rbp)
movq    %rcx,   -32(%rbp)
leaq	.LC0(%rip),	%rax
movq	%rax,	%rdi
movl	$0,	%eax
movq	%rax,	%rsi
movl	$0,	%eax
movq	%rax,	%rdx
call	printf@PLT
movq    %rax,   -40(%rbp)
movq    -32(%rbp), %rcx
movq    -24(%rbp), %rax
movq    $0,     %rax
jmp     .end
.end:
movq	%rbp,	%rsp
popq	%rbp
ret
