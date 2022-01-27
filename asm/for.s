			# This code was produced by the aTHO_ Compiler
			# Revision 1.0 | Last update 23 jan 2022(c) 2021 Aubertin Emmanuel from https://athomisos.fr
			# This Compiler is a fork of CERI Compiler (framagit.org/jourlin/cericompiler)
# ----------------- DeclarationPart ----------------- #
	.data
FormatString:    .string "%c"
FormatString1:    .string "%llu\n"
	.align 8
a:	.quad 0
x:	.quad 0
i0: .quad 0
STR2:	.string "\tIN FOR\n"

# ----------------- StatementPart ----------------- #
	.text		# The following lines contain the program
	.globl main	# The main function must be visible from outside
main:			# The main function body :
	movq %rsp, %rbp	# Save the position of the stack's top
	push $1
	pop x
	push $0
	pop a
movq $0, %rcx # Get Start value
For0:

	cmp $4, i0 # To KEYWORD
	jae EndFor0
	push i0
	push $1
	pop %rbx
	pop %rax
	addq	%rbx, %rax	# ADD
	push %rax
	pop i0


	movl $4, %eax # sys_write
	movl $1, %ebx # write to file descriptor 1 (stdout)
	movl $STR2, %ecx # pointer to the string to print
	movl $12, %edx # asks to print 5 characters our of the string passed in %ecx
	int $0x80 # call the system call	

	jmp For0
EndFor0:
	push a
	pop %rdx                     # The value to be displayed
	movq $FormatString1, %rsi    # "%llu\n" 
	movl    $1, %edi 
	movl    $0, %eax 
	call    __printf_chk@PLT

	movq %rbp, %rsp		# Restore the position of the stack's top
	ret			# Return from main function
