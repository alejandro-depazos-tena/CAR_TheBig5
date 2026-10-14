	.file	"p3_ej3.c"
	.text
	.p2align 4
	.type	l_reduction._omp_fn.0, @function
l_reduction._omp_fn.0:
.LFB51:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	movq	%rdi, %rbx
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
	call	omp_get_num_threads@PLT
	movl	%eax, %ebp
	call	omp_get_thread_num@PLT
	xorl	%edx, %edx
	movl	%eax, %ecx
	movl	$20000000, %eax
	idivl	%ebp
	cmpl	%edx, %ecx
	jl	.L2
.L5:
	imull	%eax, %ecx
	addl	%ecx, %edx
	xorl	%ecx, %ecx
	leal	(%rax,%rdx), %esi
	cmpl	%esi, %edx
	jge	.L3
	movslq	%edx, %rdx
	movq	v(%rip), %rsi
	movl	%eax, %eax
	addq	%rdx, %rax
	leaq	(%rsi,%rdx,8), %rcx
	leaq	(%rsi,%rax,8), %rdx
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L4:
	addq	(%rcx), %rax
	addq	$8, %rcx
	cmpq	%rdx, %rcx
	jne	.L4
	movq	%rax, %rcx
.L3:
	lock addq	%rcx, (%rbx)
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L2:
	.cfi_restore_state
	addl	$1, %eax
	xorl	%edx, %edx
	jmp	.L5
	.cfi_endproc
.LFE51:
	.size	l_reduction._omp_fn.0, .-l_reduction._omp_fn.0
	.p2align 4
	.type	l_atomic._omp_fn.0, @function
l_atomic._omp_fn.0:
.LFB52:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	movq	%rdi, %rbx
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
	call	omp_get_num_threads@PLT
	movl	%eax, %ebp
	call	omp_get_thread_num@PLT
	xorl	%edx, %edx
	movl	%eax, %ecx
	movl	$20000000, %eax
	idivl	%ebp
	cmpl	%edx, %ecx
	jl	.L12
.L15:
	imull	%eax, %ecx
	addl	%ecx, %edx
	leal	(%rax,%rdx), %ecx
	cmpl	%ecx, %edx
	jge	.L11
	movslq	%edx, %rdx
	movl	%eax, %eax
	leaq	0(,%rdx,8), %rcx
	addq	%rax, %rdx
	salq	$3, %rdx
	.p2align 4,,10
	.p2align 3
.L14:
	movq	v(%rip), %rsi
	movq	(%rbx), %rax
	movq	(%rsi,%rcx), %rsi
	lock addq	%rsi, (%rax)
	addq	$8, %rcx
	cmpq	%rcx, %rdx
	jne	.L14
.L11:
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L12:
	.cfi_restore_state
	addl	$1, %eax
	xorl	%edx, %edx
	jmp	.L15
	.cfi_endproc
.LFE52:
	.size	l_atomic._omp_fn.0, .-l_atomic._omp_fn.0
	.p2align 4
	.type	l_critical._omp_fn.0, @function
l_critical._omp_fn.0:
.LFB53:
	.cfi_startproc
	endbr64
	pushq	%r12
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
	pushq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	movq	%rdi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	call	omp_get_num_threads@PLT
	movl	%eax, %ebx
	call	omp_get_thread_num@PLT
	xorl	%edx, %edx
	movl	%eax, %ecx
	movl	$20000000, %eax
	idivl	%ebx
	cmpl	%edx, %ecx
	jl	.L19
.L22:
	imull	%eax, %ecx
	addl	%ecx, %edx
	leal	(%rax,%rdx), %ecx
	cmpl	%ecx, %edx
	jge	.L18
	movslq	%edx, %rdx
	movl	%eax, %r12d
	addq	%rdx, %r12
	leaq	0(,%rdx,8), %rbx
	salq	$3, %r12
	.p2align 4,,10
	.p2align 3
.L21:
	call	GOMP_critical_start@PLT
	movq	v(%rip), %rax
	movq	(%rax,%rbx), %rax
	addq	%rax, 0(%rbp)
	addq	$8, %rbx
	call	GOMP_critical_end@PLT
	cmpq	%r12, %rbx
	jne	.L21
.L18:
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r12
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L19:
	.cfi_restore_state
	addl	$1, %eax
	xorl	%edx, %edx
	jmp	.L22
	.cfi_endproc
.LFE53:
	.size	l_critical._omp_fn.0, .-l_critical._omp_fn.0
	.p2align 4
	.type	d_reduction._omp_fn.0, @function
d_reduction._omp_fn.0:
.LFB54:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	movq	%rdi, %rbx
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
	call	omp_get_num_threads@PLT
	movl	%eax, %ebp
	call	omp_get_thread_num@PLT
	xorl	%edx, %edx
	movl	%eax, %ecx
	movl	$20000000, %eax
	idivl	%ebp
	cmpl	%edx, %ecx
	jl	.L26
.L30:
	imull	%eax, %ecx
	pxor	%xmm0, %xmm0
	addl	%ecx, %edx
	leal	(%rax,%rdx), %ecx
	cmpl	%ecx, %edx
	jge	.L27
	movslq	%edx, %rdx
	movq	x(%rip), %rsi
	movl	%eax, %eax
	addq	%rdx, %rax
	leaq	(%rsi,%rax,8), %rax
	leaq	(%rsi,%rdx,8), %rcx
	movq	%rax, %rdx
	subq	%rcx, %rdx
	andl	$8, %edx
	je	.L28
	addsd	(%rcx), %xmm0
	addq	$8, %rcx
	cmpq	%rax, %rcx
	je	.L27
	.p2align 4,,10
	.p2align 3
.L28:
	addsd	(%rcx), %xmm0
	addq	$16, %rcx
	addsd	-8(%rcx), %xmm0
	cmpq	%rax, %rcx
	jne	.L28
.L27:
	movq	(%rbx), %rdx
.L29:
	movq	%rdx, %xmm1
	movq	%rdx, %rax
	addsd	%xmm0, %xmm1
	movq	%xmm1, %rcx
	lock cmpxchgq	%rcx, (%rbx)
	jne	.L41
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L26:
	.cfi_restore_state
	addl	$1, %eax
	xorl	%edx, %edx
	jmp	.L30
.L41:
	movq	%rax, %rdx
	jmp	.L29
	.cfi_endproc
.LFE54:
	.size	d_reduction._omp_fn.0, .-d_reduction._omp_fn.0
	.p2align 4
	.type	d_atomic._omp_fn.0, @function
d_atomic._omp_fn.0:
.LFB55:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	movq	%rdi, %rbx
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
	call	omp_get_num_threads@PLT
	movl	%eax, %ebp
	call	omp_get_thread_num@PLT
	xorl	%edx, %edx
	movl	%eax, %ecx
	movl	$20000000, %eax
	idivl	%ebp
	cmpl	%edx, %ecx
	jl	.L43
.L47:
	imull	%eax, %ecx
	addl	%ecx, %edx
	leal	(%rax,%rdx), %ecx
	cmpl	%ecx, %edx
	jge	.L42
	movslq	%edx, %rdx
	movl	%eax, %edi
	addq	%rdx, %rdi
	leaq	0(,%rdx,8), %rcx
	salq	$3, %rdi
	.p2align 4,,10
	.p2align 3
.L46:
	movq	x(%rip), %rax
	movq	(%rbx), %rsi
	movsd	(%rax,%rcx), %xmm0
	movq	(%rsi), %rdx
.L45:
	movq	%rdx, %xmm1
	movq	%rdx, %rax
	addsd	%xmm0, %xmm1
	movq	%xmm1, %r8
	lock cmpxchgq	%r8, (%rsi)
	jne	.L48
	addq	$8, %rcx
	cmpq	%rcx, %rdi
	jne	.L46
.L42:
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L43:
	.cfi_restore_state
	addl	$1, %eax
	xorl	%edx, %edx
	jmp	.L47
.L48:
	movq	%rax, %rdx
	jmp	.L45
	.cfi_endproc
.LFE55:
	.size	d_atomic._omp_fn.0, .-d_atomic._omp_fn.0
	.p2align 4
	.type	d_critical._omp_fn.0, @function
d_critical._omp_fn.0:
.LFB56:
	.cfi_startproc
	endbr64
	pushq	%r12
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
	pushq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	movq	%rdi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	call	omp_get_num_threads@PLT
	movl	%eax, %ebx
	call	omp_get_thread_num@PLT
	xorl	%edx, %edx
	movl	%eax, %ecx
	movl	$20000000, %eax
	idivl	%ebx
	cmpl	%edx, %ecx
	jl	.L52
.L55:
	imull	%eax, %ecx
	addl	%ecx, %edx
	leal	(%rax,%rdx), %ecx
	cmpl	%ecx, %edx
	jge	.L51
	movslq	%edx, %rdx
	movl	%eax, %r12d
	addq	%rdx, %r12
	leaq	0(,%rdx,8), %rbx
	salq	$3, %r12
	.p2align 4,,10
	.p2align 3
.L54:
	call	GOMP_critical_start@PLT
	movq	x(%rip), %rax
	movsd	0(%rbp), %xmm0
	addsd	(%rax,%rbx), %xmm0
	addq	$8, %rbx
	movsd	%xmm0, 0(%rbp)
	call	GOMP_critical_end@PLT
	cmpq	%r12, %rbx
	jne	.L54
.L51:
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r12
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L52:
	.cfi_restore_state
	addl	$1, %eax
	xorl	%edx, %edx
	jmp	.L55
	.cfi_endproc
.LFE56:
	.size	d_critical._omp_fn.0, .-d_critical._omp_fn.0
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC2:
	.string	"acumulador de tipo %s\n"
.LC3:
	.string	" secuencial %8.4f s\n"
.LC4:
	.string	" critical   %8.4f s\n"
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC5:
	.string	" atomic     %8.4f s %6.2f veces mas rapido que critical\n"
	.align 8
.LC6:
	.string	" reduction  %8.4f s %6.0f veces mas rapido que critical\n"
	.text
	.p2align 4
	.type	tanda, @function
tanda:
.LFB49:
	.cfi_startproc
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	movl	$3, %r14d
	pushq	%r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	movq	%rsi, %r13
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	movq	%rdx, %r12
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	movq	%rcx, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	movq	%r8, %rbx
	subq	$104, %rsp
	.cfi_def_cfa_offset 160
	movq	%rdi, 40(%rsp)
	leaq	56(%rsp), %r15
	movq	%fs:40, %rax
	movq	%rax, 88(%rsp)
	xorl	%eax, %eax
	movq	.LC1(%rip), %rax
	movq	%rax, 16(%rsp)
.L60:
	movq	%r15, %rdi
	call	*%r13
	minsd	16(%rsp), %xmm0
	movsd	%xmm0, 16(%rsp)
	subl	$1, %r14d
	jne	.L60
	movq	.LC1(%rip), %rax
	movl	$3, %r13d
	leaq	64(%rsp), %r15
	movq	%rax, 8(%rsp)
.L62:
	movq	%r15, %rdi
	call	*%r12
	minsd	8(%rsp), %xmm0
	movsd	%xmm0, 8(%rsp)
	subl	$1, %r13d
	jne	.L62
	movq	.LC1(%rip), %rax
	movl	$3, %r12d
	leaq	72(%rsp), %r13
	movq	%rax, 32(%rsp)
.L64:
	movq	%r13, %rdi
	call	*%rbp
	minsd	32(%rsp), %xmm0
	movsd	%xmm0, 32(%rsp)
	subl	$1, %r12d
	jne	.L64
	movq	.LC1(%rip), %rax
	movl	$3, %ebp
	leaq	80(%rsp), %r12
	movq	%rax, 24(%rsp)
.L66:
	movq	%r12, %rdi
	call	*%rbx
	minsd	24(%rsp), %xmm0
	movsd	%xmm0, 24(%rsp)
	subl	$1, %ebp
	jne	.L66
	movq	40(%rsp), %rdx
	leaq	.LC2(%rip), %rsi
	movl	$2, %edi
	xorl	%eax, %eax
	call	__printf_chk@PLT
	movsd	16(%rsp), %xmm0
	movl	$2, %edi
	leaq	.LC3(%rip), %rsi
	movl	$1, %eax
	call	__printf_chk@PLT
	movsd	8(%rsp), %xmm0
	movl	$2, %edi
	leaq	.LC4(%rip), %rsi
	movl	$1, %eax
	call	__printf_chk@PLT
	movsd	32(%rsp), %xmm6
	movsd	8(%rsp), %xmm1
	leaq	.LC5(%rip), %rsi
	movl	$2, %edi
	movl	$2, %eax
	divsd	%xmm6, %xmm1
	movapd	%xmm6, %xmm0
	call	__printf_chk@PLT
	movsd	24(%rsp), %xmm7
	movsd	8(%rsp), %xmm1
	leaq	.LC6(%rip), %rsi
	movl	$2, %edi
	movl	$2, %eax
	divsd	%xmm7, %xmm1
	movapd	%xmm7, %xmm0
	call	__printf_chk@PLT
	movsd	56(%rsp), %xmm0
	ucomisd	64(%rsp), %xmm0
	jp	.L67
	jne	.L67
	ucomisd	72(%rsp), %xmm0
	jp	.L67
	jne	.L67
	ucomisd	80(%rsp), %xmm0
	jp	.L67
	je	.L58
	.p2align 4,,10
	.p2align 3
.L67:
	movl	$0, bien(%rip)
.L58:
	movq	88(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L84
	addq	$104, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	ret
.L84:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE49:
	.size	tanda, .-tanda
	.p2align 4
	.type	l_atomic, @function
l_atomic:
.LFB46:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rdi, %rbp
	movl	$1, %edi
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	subq	$72, %rsp
	.cfi_def_cfa_offset 96
	movq	%fs:40, %rax
	movq	%rax, 56(%rsp)
	xorl	%eax, %eax
	leaq	32(%rsp), %rbx
	movq	$0, 24(%rsp)
	movq	%rbx, %rsi
	call	clock_gettime@PLT
	pxor	%xmm1, %xmm1
	pxor	%xmm0, %xmm0
	xorl	%ecx, %ecx
	cvtsi2sdq	32(%rsp), %xmm1
	leaq	24(%rsp), %rax
	movq	%rbx, %rsi
	movl	$8, %edx
	cvtsi2sdq	40(%rsp), %xmm0
	divsd	.LC7(%rip), %xmm0
	movq	%rax, 32(%rsp)
	leaq	l_atomic._omp_fn.0(%rip), %rdi
	addsd	%xmm1, %xmm0
	movsd	%xmm0, 8(%rsp)
	call	GOMP_parallel@PLT
	pxor	%xmm0, %xmm0
	movq	%rbx, %rsi
	movl	$1, %edi
	cvtsi2sdq	24(%rsp), %xmm0
	movsd	%xmm0, 0(%rbp)
	call	clock_gettime@PLT
	pxor	%xmm1, %xmm1
	pxor	%xmm0, %xmm0
	cvtsi2sdq	32(%rsp), %xmm1
	cvtsi2sdq	40(%rsp), %xmm0
	divsd	.LC7(%rip), %xmm0
	addsd	%xmm1, %xmm0
	subsd	8(%rsp), %xmm0
	movq	56(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L88
	addq	$72, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
.L88:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE46:
	.size	l_atomic, .-l_atomic
	.p2align 4
	.type	d_reduction, @function
d_reduction:
.LFB43:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rdi, %rbp
	movl	$1, %edi
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	subq	$56, %rsp
	.cfi_def_cfa_offset 80
	movq	%fs:40, %rax
	movq	%rax, 40(%rsp)
	xorl	%eax, %eax
	leaq	16(%rsp), %rbx
	movq	%rbx, %rsi
	call	clock_gettime@PLT
	pxor	%xmm1, %xmm1
	pxor	%xmm0, %xmm0
	xorl	%ecx, %ecx
	cvtsi2sdq	16(%rsp), %xmm1
	movl	$8, %edx
	movq	%rbx, %rsi
	movq	$0x000000000, 16(%rsp)
	cvtsi2sdq	24(%rsp), %xmm0
	divsd	.LC7(%rip), %xmm0
	leaq	d_reduction._omp_fn.0(%rip), %rdi
	addsd	%xmm1, %xmm0
	movsd	%xmm0, 8(%rsp)
	call	GOMP_parallel@PLT
	movsd	16(%rsp), %xmm0
	movq	%rbx, %rsi
	movl	$1, %edi
	movsd	%xmm0, 0(%rbp)
	call	clock_gettime@PLT
	pxor	%xmm1, %xmm1
	pxor	%xmm0, %xmm0
	cvtsi2sdq	16(%rsp), %xmm1
	cvtsi2sdq	24(%rsp), %xmm0
	divsd	.LC7(%rip), %xmm0
	addsd	%xmm1, %xmm0
	subsd	8(%rsp), %xmm0
	movq	40(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L92
	addq	$56, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
.L92:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE43:
	.size	d_reduction, .-d_reduction
	.p2align 4
	.type	d_critical, @function
d_critical:
.LFB41:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rdi, %rbp
	movl	$1, %edi
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	subq	$56, %rsp
	.cfi_def_cfa_offset 80
	movq	%fs:40, %rax
	movq	%rax, 40(%rsp)
	xorl	%eax, %eax
	leaq	16(%rsp), %rbx
	movq	%rbx, %rsi
	call	clock_gettime@PLT
	pxor	%xmm1, %xmm1
	pxor	%xmm0, %xmm0
	xorl	%ecx, %ecx
	cvtsi2sdq	16(%rsp), %xmm1
	movl	$8, %edx
	movq	%rbx, %rsi
	movq	$0x000000000, 16(%rsp)
	cvtsi2sdq	24(%rsp), %xmm0
	divsd	.LC7(%rip), %xmm0
	leaq	d_critical._omp_fn.0(%rip), %rdi
	addsd	%xmm1, %xmm0
	movsd	%xmm0, 8(%rsp)
	call	GOMP_parallel@PLT
	movsd	16(%rsp), %xmm0
	movq	%rbx, %rsi
	movl	$1, %edi
	movsd	%xmm0, 0(%rbp)
	call	clock_gettime@PLT
	pxor	%xmm1, %xmm1
	pxor	%xmm0, %xmm0
	cvtsi2sdq	16(%rsp), %xmm1
	cvtsi2sdq	24(%rsp), %xmm0
	divsd	.LC7(%rip), %xmm0
	addsd	%xmm1, %xmm0
	subsd	8(%rsp), %xmm0
	movq	40(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L96
	addq	$56, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
.L96:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE41:
	.size	d_critical, .-d_critical
	.p2align 4
	.type	d_atomic, @function
d_atomic:
.LFB42:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rdi, %rbp
	movl	$1, %edi
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	subq	$72, %rsp
	.cfi_def_cfa_offset 96
	movq	%fs:40, %rax
	movq	%rax, 56(%rsp)
	xorl	%eax, %eax
	leaq	32(%rsp), %rbx
	movq	$0x000000000, 24(%rsp)
	movq	%rbx, %rsi
	call	clock_gettime@PLT
	pxor	%xmm1, %xmm1
	pxor	%xmm0, %xmm0
	xorl	%ecx, %ecx
	cvtsi2sdq	32(%rsp), %xmm1
	leaq	24(%rsp), %rax
	movq	%rbx, %rsi
	movl	$8, %edx
	cvtsi2sdq	40(%rsp), %xmm0
	divsd	.LC7(%rip), %xmm0
	movq	%rax, 32(%rsp)
	leaq	d_atomic._omp_fn.0(%rip), %rdi
	addsd	%xmm1, %xmm0
	movsd	%xmm0, 8(%rsp)
	call	GOMP_parallel@PLT
	movsd	24(%rsp), %xmm0
	movq	%rbx, %rsi
	movl	$1, %edi
	movsd	%xmm0, 0(%rbp)
	call	clock_gettime@PLT
	pxor	%xmm1, %xmm1
	pxor	%xmm0, %xmm0
	cvtsi2sdq	32(%rsp), %xmm1
	cvtsi2sdq	40(%rsp), %xmm0
	divsd	.LC7(%rip), %xmm0
	addsd	%xmm1, %xmm0
	subsd	8(%rsp), %xmm0
	movq	56(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L100
	addq	$72, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
.L100:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE42:
	.size	d_atomic, .-d_atomic
	.p2align 4
	.type	l_critical, @function
l_critical:
.LFB45:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rdi, %rbp
	movl	$1, %edi
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	subq	$56, %rsp
	.cfi_def_cfa_offset 80
	movq	%fs:40, %rax
	movq	%rax, 40(%rsp)
	xorl	%eax, %eax
	leaq	16(%rsp), %rbx
	movq	%rbx, %rsi
	call	clock_gettime@PLT
	pxor	%xmm1, %xmm1
	pxor	%xmm0, %xmm0
	xorl	%ecx, %ecx
	cvtsi2sdq	16(%rsp), %xmm1
	movl	$8, %edx
	movq	%rbx, %rsi
	movq	$0, 16(%rsp)
	cvtsi2sdq	24(%rsp), %xmm0
	divsd	.LC7(%rip), %xmm0
	leaq	l_critical._omp_fn.0(%rip), %rdi
	addsd	%xmm1, %xmm0
	movsd	%xmm0, 8(%rsp)
	call	GOMP_parallel@PLT
	pxor	%xmm0, %xmm0
	movq	%rbx, %rsi
	movl	$1, %edi
	cvtsi2sdq	16(%rsp), %xmm0
	movsd	%xmm0, 0(%rbp)
	call	clock_gettime@PLT
	pxor	%xmm1, %xmm1
	pxor	%xmm0, %xmm0
	cvtsi2sdq	16(%rsp), %xmm1
	cvtsi2sdq	24(%rsp), %xmm0
	divsd	.LC7(%rip), %xmm0
	addsd	%xmm1, %xmm0
	subsd	8(%rsp), %xmm0
	movq	40(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L104
	addq	$56, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
.L104:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE45:
	.size	l_critical, .-l_critical
	.p2align 4
	.type	l_reduction, @function
l_reduction:
.LFB47:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rdi, %rbp
	movl	$1, %edi
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	subq	$56, %rsp
	.cfi_def_cfa_offset 80
	movq	%fs:40, %rax
	movq	%rax, 40(%rsp)
	xorl	%eax, %eax
	leaq	16(%rsp), %rbx
	movq	%rbx, %rsi
	call	clock_gettime@PLT
	pxor	%xmm1, %xmm1
	pxor	%xmm0, %xmm0
	xorl	%ecx, %ecx
	cvtsi2sdq	16(%rsp), %xmm1
	movl	$8, %edx
	movq	%rbx, %rsi
	movq	$0, 16(%rsp)
	cvtsi2sdq	24(%rsp), %xmm0
	divsd	.LC7(%rip), %xmm0
	leaq	l_reduction._omp_fn.0(%rip), %rdi
	addsd	%xmm1, %xmm0
	movsd	%xmm0, 8(%rsp)
	call	GOMP_parallel@PLT
	pxor	%xmm0, %xmm0
	movq	%rbx, %rsi
	movl	$1, %edi
	cvtsi2sdq	16(%rsp), %xmm0
	movsd	%xmm0, 0(%rbp)
	call	clock_gettime@PLT
	pxor	%xmm1, %xmm1
	pxor	%xmm0, %xmm0
	cvtsi2sdq	16(%rsp), %xmm1
	cvtsi2sdq	24(%rsp), %xmm0
	divsd	.LC7(%rip), %xmm0
	addsd	%xmm1, %xmm0
	subsd	8(%rsp), %xmm0
	movq	40(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L108
	addq	$56, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
.L108:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE47:
	.size	l_reduction, .-l_reduction
	.p2align 4
	.type	d_secuencial, @function
d_secuencial:
.LFB40:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	movq	%rdi, %rbx
	movl	$1, %edi
	subq	$56, %rsp
	.cfi_def_cfa_offset 80
	movq	%fs:40, %rax
	movq	%rax, 40(%rsp)
	xorl	%eax, %eax
	leaq	16(%rsp), %rbp
	movq	%rbp, %rsi
	call	clock_gettime@PLT
	pxor	%xmm1, %xmm1
	pxor	%xmm0, %xmm0
	movq	x(%rip), %rax
	cvtsi2sdq	16(%rsp), %xmm1
	cvtsi2sdq	24(%rsp), %xmm0
	divsd	.LC7(%rip), %xmm0
	leaq	160000000(%rax), %rdx
	addsd	%xmm1, %xmm0
	movsd	%xmm0, 8(%rsp)
	pxor	%xmm0, %xmm0
	.p2align 4,,10
	.p2align 3
.L110:
	addsd	(%rax), %xmm0
	addq	$16, %rax
	addsd	-8(%rax), %xmm0
	cmpq	%rax, %rdx
	jne	.L110
	movsd	%xmm0, (%rbx)
	movq	%rbp, %rsi
	movl	$1, %edi
	call	clock_gettime@PLT
	pxor	%xmm1, %xmm1
	pxor	%xmm0, %xmm0
	cvtsi2sdq	16(%rsp), %xmm1
	cvtsi2sdq	24(%rsp), %xmm0
	divsd	.LC7(%rip), %xmm0
	addsd	%xmm1, %xmm0
	subsd	8(%rsp), %xmm0
	movq	40(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L114
	addq	$56, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
.L114:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE40:
	.size	d_secuencial, .-d_secuencial
	.p2align 4
	.type	l_secuencial, @function
l_secuencial:
.LFB44:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	movq	%rdi, %rbx
	movl	$1, %edi
	subq	$56, %rsp
	.cfi_def_cfa_offset 80
	movq	%fs:40, %rax
	movq	%rax, 40(%rsp)
	xorl	%eax, %eax
	leaq	16(%rsp), %rbp
	movq	%rbp, %rsi
	call	clock_gettime@PLT
	pxor	%xmm1, %xmm1
	pxor	%xmm0, %xmm0
	xorl	%edx, %edx
	cvtsi2sdq	16(%rsp), %xmm1
	movq	v(%rip), %rax
	cvtsi2sdq	24(%rsp), %xmm0
	divsd	.LC7(%rip), %xmm0
	leaq	160000000(%rax), %rcx
	addsd	%xmm1, %xmm0
	movsd	%xmm0, 8(%rsp)
	.p2align 4,,10
	.p2align 3
.L116:
	addq	(%rax), %rdx
	addq	$8, %rax
	cmpq	%rcx, %rax
	jne	.L116
	pxor	%xmm0, %xmm0
	movq	%rbp, %rsi
	movl	$1, %edi
	cvtsi2sdq	%rdx, %xmm0
	movsd	%xmm0, (%rbx)
	call	clock_gettime@PLT
	pxor	%xmm1, %xmm1
	pxor	%xmm0, %xmm0
	cvtsi2sdq	16(%rsp), %xmm1
	cvtsi2sdq	24(%rsp), %xmm0
	divsd	.LC7(%rip), %xmm0
	addsd	%xmm1, %xmm0
	subsd	8(%rsp), %xmm0
	movq	40(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L120
	addq	$56, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
.L120:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE44:
	.size	l_secuencial, .-l_secuencial
	.section	.rodata.str1.1
.LC9:
	.string	"lo mismo"
.LC10:
	.string	"DISTINTO"
.LC11:
	.string	"sin memoria\n"
	.section	.rodata.str1.8
	.align 8
.LC15:
	.string	"%d hilos, %d sumas, mejor de %d\n"
	.section	.rodata.str1.1
.LC16:
	.string	"double"
.LC17:
	.string	"long long"
.LC18:
	.string	"los ocho acumulados dan %s\n"
	.section	.text.startup,"ax",@progbits
	.p2align 4
	.globl	main
	.type	main, @function
main:
.LFB50:
	.cfi_startproc
	endbr64
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movl	$160000000, %edi
	call	malloc@PLT
	movl	$160000000, %edi
	movq	%rax, x(%rip)
	movq	%rax, %rbx
	call	malloc@PLT
	movq	%rax, v(%rip)
	testq	%rax, %rax
	je	.L128
	testq	%rbx, %rbx
	je	.L128
	movdqa	.LC13(%rip), %xmm2
	pxor	%xmm3, %xmm3
	movdqa	.LC8(%rip), %xmm4
	xorl	%edx, %edx
	movdqa	%xmm3, %xmm5
	movdqa	.LC12(%rip), %xmm7
	movdqa	.LC14(%rip), %xmm6
	pcmpgtd	%xmm2, %xmm5
	.p2align 4,,10
	.p2align 3
.L122:
	movdqa	%xmm4, %xmm0
	movdqa	%xmm3, %xmm8
	movdqa	%xmm5, %xmm9
	pcmpgtd	%xmm0, %xmm8
	pmuludq	%xmm0, %xmm9
	movdqa	%xmm0, %xmm1
	pmuludq	%xmm2, %xmm1
	movdqa	%xmm5, %xmm10
	paddd	%xmm7, %xmm4
	pmuludq	%xmm2, %xmm8
	paddq	%xmm9, %xmm8
	movdqa	%xmm3, %xmm9
	psllq	$32, %xmm8
	paddq	%xmm8, %xmm1
	movdqa	%xmm0, %xmm8
	psrlq	$32, %xmm8
	pcmpgtd	%xmm8, %xmm9
	pmuludq	%xmm8, %xmm10
	pmuludq	%xmm2, %xmm8
	pmuludq	%xmm2, %xmm9
	paddq	%xmm10, %xmm9
	psllq	$32, %xmm9
	paddq	%xmm9, %xmm8
	shufps	$221, %xmm8, %xmm1
	pshufd	$216, %xmm1, %xmm1
	paddd	%xmm0, %xmm1
	psrad	$2, %xmm1
	movdqa	%xmm1, %xmm8
	pslld	$3, %xmm8
	psubd	%xmm1, %xmm8
	movdqa	%xmm3, %xmm1
	psubd	%xmm8, %xmm0
	paddd	%xmm6, %xmm0
	pcmpgtd	%xmm0, %xmm1
	movdqa	%xmm0, %xmm8
	punpckldq	%xmm1, %xmm8
	movups	%xmm8, (%rax,%rdx)
	movdqa	%xmm0, %xmm8
	punpckhdq	%xmm1, %xmm8
	cvtdq2pd	%xmm0, %xmm1
	pshufd	$238, %xmm0, %xmm0
	movups	%xmm1, (%rbx,%rdx)
	movups	%xmm8, 16(%rax,%rdx)
	cvtdq2pd	%xmm0, %xmm0
	movups	%xmm0, 16(%rbx,%rdx)
	addq	$32, %rdx
	cmpq	$160000000, %rdx
	jne	.L122
	movl	$3, %r8d
	movl	$20000000, %ecx
	xorl	%eax, %eax
	movl	$8, %edx
	leaq	.LC15(%rip), %rsi
	movl	$2, %edi
	call	__printf_chk@PLT
	leaq	d_reduction(%rip), %r8
	leaq	d_atomic(%rip), %rcx
	leaq	d_critical(%rip), %rdx
	leaq	d_secuencial(%rip), %rsi
	leaq	.LC16(%rip), %rdi
	call	tanda
	leaq	l_reduction(%rip), %r8
	leaq	l_atomic(%rip), %rcx
	leaq	l_critical(%rip), %rdx
	leaq	l_secuencial(%rip), %rsi
	leaq	.LC17(%rip), %rdi
	call	tanda
	movl	bien(%rip), %eax
	movl	$2, %edi
	leaq	.LC10(%rip), %rdx
	leaq	.LC18(%rip), %rsi
	testl	%eax, %eax
	leaq	.LC9(%rip), %rax
	cmovne	%rax, %rdx
	xorl	%eax, %eax
	call	__printf_chk@PLT
	movq	x(%rip), %rdi
	call	free@PLT
	movq	v(%rip), %rdi
	call	free@PLT
	xorl	%eax, %eax
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	ret
.L128:
	.cfi_restore_state
	movq	stderr(%rip), %rcx
	movl	$12, %edx
	movl	$1, %esi
	leaq	.LC11(%rip), %rdi
	call	fwrite@PLT
	movq	x(%rip), %rdi
	call	free@PLT
	movq	v(%rip), %rdi
	call	free@PLT
	movl	$1, %eax
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE50:
	.size	main, .-main
	.data
	.align 4
	.type	bien, @object
	.size	bien, 4
bien:
	.long	1
	.local	v
	.comm	v,8,8
	.local	x
	.comm	x,8,8
	.section	.rodata.cst8,"aM",@progbits,8
	.align 8
.LC1:
	.long	966823146
	.long	1177108057
	.align 8
.LC7:
	.long	0
	.long	1104006501
	.section	.rodata.cst16,"aM",@progbits,16
	.align 16
.LC8:
	.long	0
	.long	1
	.long	2
	.long	3
	.align 16
.LC12:
	.long	4
	.long	4
	.long	4
	.long	4
	.align 16
.LC13:
	.long	-1840700269
	.long	-1840700269
	.long	-1840700269
	.long	-1840700269
	.align 16
.LC14:
	.long	1
	.long	1
	.long	1
	.long	1
	.ident	"GCC: (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
