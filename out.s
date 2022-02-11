	.file	"main.c"
# GNU C17 (GCC) version 11.1.0 (x86_64-pc-linux-gnu)
#	compiled by GNU C version 11.1.0, GMP version 6.2.1, MPFR version 4.1.0-p13, MPC version 1.2.1, isl version isl-0.24-GMP

# GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
# options passed: -mtune=generic -march=x86-64
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
	subq	$32, %rsp	#,
# out/main.c:4: int main(void) {
	movq	%fs:40, %rax	# MEM[(<address-space-1> long unsigned int *)40B], tmp87
	movq	%rax, -8(%rbp)	# tmp87, D.1951
	xorl	%eax, %eax	# tmp87
# out/main.c:5: 	int a = 5;
	movl	$5, -20(%rbp)	#, a
# out/main.c:6: 	int* c = &a;
	leaq	-20(%rbp), %rax	#, tmp84
	movq	%rax, -16(%rbp)	# tmp84, c
# out/main.c:7: 	*c = 10;
	movq	-16(%rbp), %rax	# c, tmp85
	movl	$10, (%rax)	#, *c_3
	movl	$0, %eax	#, _6
# out/main.c:8: }
	movq	-8(%rbp), %rdx	# D.1951, tmp88
	subq	%fs:40, %rdx	# MEM[(<address-space-1> long unsigned int *)40B], tmp88
	je	.L3	#,
	call	__stack_chk_fail@PLT	#
.L3:
	leave	
	.cfi_def_cfa 7, 8
	ret	
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (GNU) 11.1.0"
	.section	.note.GNU-stack,"",@progbits
