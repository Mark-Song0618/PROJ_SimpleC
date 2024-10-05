.file	"./test.c"
	.data
	.section .rodata
.LC0:
	.string	"Result: %d,%d "
	.text
	.globl	myadd
	.type	myadd,@function
myadd:
pushq	%rbp
movq	%rsp,	%rbp
subq	$32,	%rsp
movl	%edi,	-4(%rbp)
movl	%esi,	-8(%rbp)
movl	%eax,	-12(%rbp)
movl	%ecx,	-16(%rbp)
movl	-4(%rbp),	%eax
movl	%eax,	-16(%rbp)
movl	-8(%rbp),	%eax
movl	-16(%rbp),	%ecx
addl	%ecx,	%eax
jmp 	.Lend_myadd
.Lend_myadd:
movq	%rbp,	%rsp
popq	%rbp
ret
	.globl	main
	.type	main,@function
main:
pushq	%rbp
movq	%rsp,	%rbp
subq	$64,	%rsp
movl	%edi,	-4(%rbp)
movq	%rsi,	-12(%rbp)
movq	%rax,	-20(%rbp)
movq	%rcx,	-28(%rbp)
leaq	-40(%rbp),	%rax
movq	%rax,	-56(%rbp)
movl	$0,	%eax
movl	%eax,	%edi
movl	$1,	%eax
movl	%eax,	%esi
call 	myadd
movq	-56(%rbp),	%rcx
movl	%eax,	0(%rcx)
leaq	-32(%rbp),	%rax
movq	%rax,	-56(%rbp)
movl	-40(%rbp),	%eax
movq	-56(%rbp),	%rcx
movl	%eax,	0(%rcx)
leaq	-48(%rbp),	%rax
movq	%rax,	-56(%rbp)
movl	$2,	%eax
movl	%eax,	%edi
movl	$3,	%eax
movl	%eax,	%esi
call 	myadd
movq	-56(%rbp),	%rcx
movl	%eax,	0(%rcx)
leaq	-36(%rbp),	%rax
movq	%rax,	-56(%rbp)
movl	-48(%rbp),	%eax
movq	-56(%rbp),	%rcx
movl	%eax,	0(%rcx)
leaq	-56(%rbp),	%rax
movq	%rax,	-56(%rbp)
leaq	.LC0(%rip),	%rax
movq	%rax,	%rdi
movl	-32(%rbp),	%eax
movl	%eax,	%esi
movl	-36(%rbp),	%eax
movl	%eax,	%edx
call 	printf
movq	-56(%rbp),	%rcx
movl	%eax,	0(%rcx)
movl	$0,	%eax
jmp 	.Lend_main
.Lend_main:
movq	%rbp,	%rsp
popq	%rbp
ret
