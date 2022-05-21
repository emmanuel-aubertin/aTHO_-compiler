			# This code was produced by the aTHO_ Compiler
			# Revision 1.0 | Last update 23 jan 2022(c) 2021 Aubertin Emmanuel from https://athomisos.fr
			# This Compiler is a fork of CERI Compiler (framagit.org/jourlin/cericompiler)
# IN DEFAULT
TYPE = 0
TYPE = 2
TYPE = 3
# ----------------- DeclarationPart ----------------- #
	.data
FormatString:    .string "%c" # For char
FormatString1:    .string "%llu\n"
FormatString2:	.string "%lf" # For Float
TrueSTR:	.string "[1mTRUE[0m"
FalseSTR:	.string "[1mFALSE[0m"
	.align 8
i:	.quad 0
STR1:	.string "We can display some static string\n"
STR2:	.string "\tNormal \033[5mBlink \033[0mNormal\n"
chartest:	.byte 0
doubletest:	.double 0.0
# ----------------- FunctionPart ----------------- #
# ----------------- StatementPart ----------------- #
	.text		# The following lines contain the program
	.globl main	# The main function must be visible from outside
main:			# The main function body :
	movq %rsp, %rbp	# Save the position of the stack's top
	push $6
	pop i

# DISPLAY "We can display some static string\n"
	movl $4, %eax # sys_write
	movl $1, %ebx # write to file descriptor 1 (stdout)
	movl $STR1, %ecx # pointer to the string to print
	movl $34, %edx # asks to print 34 characters our of the string passed in %ecx
	int $0x80 # call the system call

# DISPLAY "\tNormal \033[5mBlink \033[0mNormal\n"
	movl $4, %eax # sys_write
	movl $1, %ebx # write to file descriptor 1 (stdout)
	movl $STR2, %ecx # pointer to the string to print
	movl $29, %edx # asks to print 29 characters our of the string passed in %ecx
	int $0x80 # call the system call

# DISPLAY i
	push i
	pop %rdx                     # TYPE INT
				movq $FormatString1, %rsi    # "%llu\n" 
				movl    $1, %edi 
				movl    $0, %eax 
				call    __printf_chk@PLT
	pop 'a'

# DISPLAY chartest
	push chartest
	pop %rdx                     # TYPE CHAR
				movq $FormatString, %rsi   
				movl    $1, %edi 
				movl    $0, %eax 
				call    __printf_chk@PLT

# DISPLAY doubletest
	push doubletest
	movsd (%rsp), %xmm0	# put stack on top of %xmm0
						subq	$16, %rsp		# add allocation for more doubles
					movsd %xmm0, 8(%rsp)
					movq $FormatString2, %rdi
					movq	$1, %rax
					call	printf
				nop
					addq $24, %rsp

	movq %rbp, %rsp		# Restore the position of the stack's top
	movl     $1, %eax
	movl     $0, %ebx
	int     $0x80			# Return from main function
