			# This code was produced by the aTHO Compiler
			# This Compiler is a fork of CERI Compiler (framagit.org/jourlin/cericompiler)
	.data
	.align 8
a:	.quad 0
b:	.quad 0
c:	.quad 0
z:	.quad 0
	.text		# The following lines contain the program
	.globl main	# The main function must be visible from outside
main:			# The main function body :
	movq %rsp, %rbp	# Save the position of the stack's top
	push $4
	pop z
	push z
	push $2
	pop %rax
	pop %rbx
	cmpq %rax, %rbx
	jb Vrai2	# If below
	push $0		# False
	jmp Suite2
Vrai2:	push $0xFFFFFFFFFFFFFFFF		# True
Suite2:
	pop %rax	# Get the result of expression
	je Else0	# jmp à Else0 if la comparaison est fausse
	push $8
	push $3
	pop %rbx
	pop %rax
	addq	%rbx, %rax	# ADD
	push %rax
	pop b
	jmp Next0	# Si le if est fais on skip le else
Else0:
Next0:
	movq %rbp, %rsp		# Restore the position of the stack's top
	ret			# Return from main function
