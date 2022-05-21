			# This code was produced by the aTHO_ Compiler
			# Revision 1.0 | Last update 23 jan 2022(c) 2021 Aubertin Emmanuel from https://athomisos.fr
			# This Compiler is a fork of CERI Compiler (framagit.org/jourlin/cericompiler)
# ----------------- DeclarationPart ----------------- #
	.data
FormatString:    .string "%c"
FormatString1:    .string "%llu\n"
	.align 8
i:	.quad 0
i1: .quad 0
# ----------------- FunctionPart ----------------- #
# ----------------- StatementPart ----------------- #
	.text		# The following lines contain the program
	.globl main	# The main function must be visible from outside
main:			# The main function body :
	movq %rsp, %rbp	# Save the position of the stack's top
movq $0, %rcx # Get Start value
For1:
	cmp $4, i1 # To KEYWORD
	jae EndFor1
push i1
	push $1
	pop %rbx
	pop %rax
	addq	%rbx, %rax	# ADD
	push %rax
	pop i1

	push $1
	push i
	pop %rbx
	pop %rax
	addq	%rbx, %rax	# ADD
	push %rax
	pop i

	jmp For1
EndFor1:
	push $0
	pop i1
	movq %rbp, %rsp		# Restore the position of the stack's top
	movl     $1, %eax
	movl     $0, %ebx
	int     $0x80			# Return from main function
