			# This code was produced by the aTHO_ Compiler
			# Revision 1.0 | Last update 23 jan 2022(c) 2021 Aubertin Emmanuel from https://athomisos.fr
			# This Compiler is a fork of CERI Compiler (framagit.org/jourlin/cericompiler)
# ----------------- DeclarationPart ----------------- #
	.data
FormatString:    .string "%c" # For char
FormatString1:    .string "%llu\n"
FormatString2:	.string "%lf" # For Float
TrueSTR:	.string "[1mTRUE[0m"
FalseSTR:	.string "[1mFALSE[0m"
firstInt:	.quad 0
secondInt:	.quad 0
firstChar:	.byte 0
secondChar:	.byte 0
firstDouble:	.double 0.0
secondDouble:	.double 0.0
# ----------------- FunctionPart ----------------- #
# ----------------- StatementPart ----------------- #
	.text		# The following lines contain the program
	.globl main	# The main function must be visible from outside
main:			# The main function body :
	movq %rsp, %rbp	# Save the position of the stack's top

	movq %rbp, %rsp		# Restore the position of the stack's top
	movl     $1, %eax
	movl     $0, %ebx
	int     $0x80			# Return from main function
