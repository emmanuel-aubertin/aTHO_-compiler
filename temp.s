			# This code was produced by the aTHO_ Compiler
			# Revision 1.0 | Last update 23 jan 2022(c) 2021 Aubertin Emmanuel from https://athomisos.fr
			# This Compiler is a fork of CERI Compiler (framagit.org/jourlin/cericompiler)
# IN [
# ----------- In Statement(void) ----------- 
# # TOKEN == ID

# -- Expression(void) --

# IN void SimpleExpression(void)
		# First part ==>0
# End of Expression()
# ----------- In Statement(void) ----------- 
# TOKEN == KEYWORD
# DISPLAY STATEMENT
# ----------- In Statement(void) ----------- 
# TOKEN == KEYWORD
# DISPLAY STATEMENT
# ----------- In Statement(void) ----------- 
# TOKEN == KEYWORD
# DISPLAY STATEMENT
# ----------- In Statement(void) ----------- 
# TOKEN == KEYWORD
# DISPLAY STATEMENT
# ----------- In Statement(void) ----------- 
# TOKEN == KEYWORD
# FOR STATEMENT
# ----------- ForStatement(void) -----------
	# FOR i = 0
# ----------- BlockStatement(void) -----------
# ----------- In Statement(void) ----------- 
# TOKEN == KEYWORD
# FOR STATEMENT
# ----------- ForStatement(void) -----------
	# FOR i = 0
# ----------- BlockStatement(void) -----------
# ----------- In Statement(void) ----------- 
# # TOKEN == ID

# -- Expression(void) --

# IN void SimpleExpression(void)
		# First part ==>0
# End of Expression()
	# CURRENT ==> ;
# ----------- In Statement(void) ----------- 
# TOKEN == KEYWORD
# DISPLAY STATEMENT
	# CURRENT ==> ;
# ----------- In Statement(void) ----------- 
# TOKEN == KEYWORD
# DISPLAY STATEMENT

# -- Expression(void) --

# IN void SimpleExpression(void)
		# First part ==>0
# End of Expression()
	# CURRENT ==> ;
	# Before End of  BlockStatement
# End of BlockStatementEND
# After BlockStatement();END
# End of forEND
	# Before End of  BlockStatement
# End of BlockStatementDISPLAY
# After BlockStatement();DISPLAY
# End of forDISPLAY
# ----------- In Statement(void) ----------- 
# TOKEN == KEYWORD
# DISPLAY STATEMENT
# ----------- In Statement(void) ----------- 
# TOKEN == KEYWORD
# DISPLAY STATEMENT
# ----------- In Statement(void) ----------- 
# TOKEN == KEYWORD
# IF STATEMENT
# ----------- IFStatement(void) -----------
	# Reading the Exp :

# -- Expression(void) --

# IN void SimpleExpression(void)

# -- Expression(void) --

# IN void SimpleExpression(void)
		# First part ==>0
		# OP ==>3

# IN void SimpleExpression(void)
		# Second part ==>0
		# SUP expression
# End of Expression()
		# First part ==>1
# End of Expression()
# WORD ==> THEN
# WHILE ! ELSE || END
		# WORD == DISPLAY
# ----------- In Statement(void) ----------- 
# TOKEN == KEYWORD
# DISPLAY STATEMENT
		# WORD == DISPLAY
# ----------- In Statement(void) ----------- 
# TOKEN == KEYWORD
# DISPLAY STATEMENT

# -- Expression(void) --

# IN void SimpleExpression(void)
		# First part ==>0
# End of Expression()
		# WORD == i
# ----------- In Statement(void) ----------- 
# # TOKEN == ID

# -- Expression(void) --

# IN void SimpleExpression(void)
		# First part ==>0
# End of Expression()
# PLZ BE A ELSE => ELSELigne n°24
# ELSE
# ----------- BlockStatement(void) -----------
# ----------- In Statement(void) ----------- 
# TOKEN == KEYWORD
# DISPLAY STATEMENT
	# CURRENT ==> ;
# ----------- In Statement(void) ----------- 
# TOKEN == KEYWORD
# DISPLAY STATEMENT

# -- Expression(void) --

# IN void SimpleExpression(void)
		# First part ==>0
# End of Expression()
	# CURRENT ==> ;
# ----------- In Statement(void) ----------- 
# # TOKEN == ID

# -- Expression(void) --

# IN void SimpleExpression(void)
		# First part ==>0
# End of Expression()
	# CURRENT ==> ;
	# Before End of  BlockStatement
# End of BlockStatementDISPLAY
# ----------- In Statement(void) ----------- 
# TOKEN == KEYWORD
# DISPLAY STATEMENT
# ----------- In Statement(void) ----------- 
# TOKEN == KEYWORD
# DISPLAY STATEMENT
# ----------- In Statement(void) ----------- 
# # TOKEN == ID

# -- Expression(void) --

# IN void SimpleExpression(void)
		# First part ==>0
# End of Expression()
# ----------- In Statement(void) ----------- 
# TOKEN == KEYWORD
# DISPLAY STATEMENT
# ----------- In Statement(void) ----------- 
# TOKEN == KEYWORD
# DISPLAY STATEMENT

# -- Expression(void) --

# IN void SimpleExpression(void)
		# First part ==>0
# End of Expression()
# ----------- In Statement(void) ----------- 
# TOKEN == KEYWORD
# WHILE STATEMENT
# ----------- WhileStatement(void) -----------

# -- Expression(void) --

# IN void SimpleExpression(void)

# -- Expression(void) --

# IN void SimpleExpression(void)
		# First part ==>0
		# OP ==>2

# IN void SimpleExpression(void)
		# Second part ==>0
		# INF expression
# End of Expression()
		# First part ==>1
# End of Expression()
# ----------- BlockStatement(void) -----------
# ----------- In Statement(void) ----------- 
# # TOKEN == ID

# -- Expression(void) --

# IN void SimpleExpression(void)
		# First part ==>0
# End of Expression()
	# CURRENT ==> ;
# ----------- In Statement(void) ----------- 
# TOKEN == KEYWORD
# DISPLAY STATEMENT
	# CURRENT ==> ;
# ----------- In Statement(void) ----------- 
# TOKEN == KEYWORD
# DISPLAY STATEMENT

# -- Expression(void) --

# IN void SimpleExpression(void)
		# First part ==>0
# End of Expression()
	# CURRENT ==> ;
	# Before End of  BlockStatement
# End of BlockStatementDISPLAY
# ----------- In Statement(void) ----------- 
# TOKEN == KEYWORD
# DISPLAY STATEMENT
# ----------- In Statement(void) ----------- 
# TOKEN == KEYWORD
# DISPLAY STATEMENT
# ----------------- DeclarationPart ----------------- #
	.data
FormatString:    .string "%c"
FormatString1:    .string "%llu\n"
	.align 8
i:	.quad 0
STR1:	.string "Test of Beautiful feature :\n"
STR2:	.string "\tNormal \033[5mBlink \033[0mNormal\n"
STR3:	.string "Beautiful feature test : \033[1m[OK]\033[0m\n\n"
STR4:	.string "FOR statement test :\n"
i4: .quad 0
i5: .quad 0
STR7:	.string "\ti = "
STR9:	.string "FOR statement test : \033[1m[OK]\033[0m\n\n"
STR10:	.string "IF statement test :\n"
STR13:	.string "\tIf is not ok i = "
STR15:	.string "\tIf is ok i = "
STR17:	.string "IF statement test : \033[1m[OK]\033[0m\n"
STR18:	.string "WHILE statement test :\n"
STR19:	.string "\ti start at  "
STR23:	.string "\ti = "
STR25:	.string "WHILE statement test : \033[1m[OK]\033[0m\n"
STR26:	.string "\033[1mGoodbye\033[0m\n"
# ----------------- StatementPart ----------------- #
	.text		# The following lines contain the program
	.globl main	# The main function must be visible from outside
main:			# The main function body :
	movq %rsp, %rbp	# Save the position of the stack's top
	push $0
	pop i

# DISPLAY "Test of Beautiful feature :\n"
	movl $4, %eax # sys_write
	movl $1, %ebx # write to file descriptor 1 (stdout)
	movl $STR1, %ecx # pointer to the string to print
	movl $28, %edx # asks to print 28 characters our of the string passed in %ecx
	int $0x80 # call the system call
# DISPLAY "\tNormal \033[5mBlink \033[0mNormal\n"
	movl $4, %eax # sys_write
	movl $1, %ebx # write to file descriptor 1 (stdout)
	movl $STR2, %ecx # pointer to the string to print
	movl $29, %edx # asks to print 29 characters our of the string passed in %ecx
	int $0x80 # call the system call
# DISPLAY "Beautiful feature test : \033[1m[OK]\033[0m\n\n"
	movl $4, %eax # sys_write
	movl $1, %ebx # write to file descriptor 1 (stdout)
	movl $STR3, %ecx # pointer to the string to print
	movl $39, %edx # asks to print 39 characters our of the string passed in %ecx
	int $0x80 # call the system call
# DISPLAY "FOR statement test :\n"
	movl $4, %eax # sys_write
	movl $1, %ebx # write to file descriptor 1 (stdout)
	movl $STR4, %ecx # pointer to the string to print
	movl $21, %edx # asks to print 21 characters our of the string passed in %ecx
	int $0x80 # call the system callmovq $0, %rcx # Get Start value
For4:
	cmp $4, i4 # To KEYWORD
	jae EndFor4
push i4
	push $1
	pop %rbx
	pop %rax
	addq	%rbx, %rax	# ADD
	push %rax
	pop i4

		 # WORD ==> FOR
movq $0, %rcx # Get Start value
For5:
	cmp $2, i5 # To KEYWORD
	jae EndFor5
push i5
	push $1
	pop %rbx
	pop %rax
	addq	%rbx, %rax	# ADD
	push %rax
	pop i5

		 # WORD ==> i
	push $1
	push i
	pop %rbx
	pop %rax
	addq	%rbx, %rax	# ADD
	push %rax
	pop i
		 # WORD ==> DISPLAY

# DISPLAY "\ti = "
	movl $4, %eax # sys_write
	movl $1, %ebx # write to file descriptor 1 (stdout)
	movl $STR7, %ecx # pointer to the string to print
	movl $5, %edx # asks to print 5 characters our of the string passed in %ecx
	int $0x80 # call the system call		 # WORD ==> DISPLAY

# DISPLAY i
	push i
	pop %rdx                     # The value to be displayed
	movq $FormatString1, %rsi    # "%llu\n" 
	movl    $1, %edi 
	movl    $0, %eax 
	call    __printf_chk@PLT

# End of BlockStatement



# afterBlockStatement();

	jmp For5
EndFor5:
	push $0
	pop i5
# End of for



# End of BlockStatement



# afterBlockStatement();

	jmp For4
EndFor4:
	push $0
	pop i4
# End of for



# DISPLAY "FOR statement test : \033[1m[OK]\033[0m\n\n"
	movl $4, %eax # sys_write
	movl $1, %ebx # write to file descriptor 1 (stdout)
	movl $STR9, %ecx # pointer to the string to print
	movl $35, %edx # asks to print 35 characters our of the string passed in %ecx
	int $0x80 # call the system call
# DISPLAY "IF statement test :\n"
	movl $4, %eax # sys_write
	movl $1, %ebx # write to file descriptor 1 (stdout)
	movl $STR10, %ecx # pointer to the string to print
	movl $20, %edx # asks to print 20 characters our of the string passed in %ecx
	int $0x80 # call the system call	push i
	
	push $0
	pop %rax
	pop %rbx
	cmpq %rax, %rbx
	je VRAI10	# jmp à Else10 if false
	jmp Else10
VRAI10:
# DISPLAY "\tIf is not ok i = "
	movl $4, %eax # sys_write
	movl $1, %ebx # write to file descriptor 1 (stdout)
	movl $STR13, %ecx # pointer to the string to print
	movl $18, %edx # asks to print 18 characters our of the string passed in %ecx
	int $0x80 # call the system call
# DISPLAY i
	push i
	pop %rdx                     # The value to be displayed
	movq $FormatString1, %rsi    # "%llu\n" 
	movl    $1, %edi 
	movl    $0, %eax 
	call    __printf_chk@PLT
	push $5
	pop i
	jmp Next10	# if true skip else
Else10:
		 # WORD ==> DISPLAY

# DISPLAY "\tIf is ok i = "
	movl $4, %eax # sys_write
	movl $1, %ebx # write to file descriptor 1 (stdout)
	movl $STR15, %ecx # pointer to the string to print
	movl $14, %edx # asks to print 14 characters our of the string passed in %ecx
	int $0x80 # call the system call		 # WORD ==> DISPLAY

# DISPLAY i
	push i
	pop %rdx                     # The value to be displayed
	movq $FormatString1, %rsi    # "%llu\n" 
	movl    $1, %edi 
	movl    $0, %eax 
	call    __printf_chk@PLT
		 # WORD ==> i
	push $0
	pop i

# End of BlockStatement


Next10:

# DISPLAY "IF statement test : \033[1m[OK]\033[0m\n"
	movl $4, %eax # sys_write
	movl $1, %ebx # write to file descriptor 1 (stdout)
	movl $STR17, %ecx # pointer to the string to print
	movl $33, %edx # asks to print 33 characters our of the string passed in %ecx
	int $0x80 # call the system call
# DISPLAY "WHILE statement test :\n"
	movl $4, %eax # sys_write
	movl $1, %ebx # write to file descriptor 1 (stdout)
	movl $STR18, %ecx # pointer to the string to print
	movl $23, %edx # asks to print 23 characters our of the string passed in %ecx
	int $0x80 # call the system call	push $0
	pop i

# DISPLAY "\ti start at  "
	movl $4, %eax # sys_write
	movl $1, %ebx # write to file descriptor 1 (stdout)
	movl $STR19, %ecx # pointer to the string to print
	movl $13, %edx # asks to print 13 characters our of the string passed in %ecx
	int $0x80 # call the system call
# DISPLAY i
	push i
	pop %rdx                     # The value to be displayed
	movq $FormatString1, %rsi    # "%llu\n" 
	movl    $1, %edi 
	movl    $0, %eax 
	call    __printf_chk@PLT
While20:
	push i
	push $4
	pop %rax
	pop %rbx
	cmpq %rax, %rbx
	jb Vrai22	# If below
	push $0		# False
	jmp Suite22
Vrai22:	push $0xFFFFFFFFFFFFFFFF		# True
Suite22:
	pop %rax 
	cmp $0, %rax
	je EndWhile20
		 # WORD ==> i
	push $1
	push i
	pop %rbx
	pop %rax
	addq	%rbx, %rax	# ADD
	push %rax
	pop i
		 # WORD ==> DISPLAY

# DISPLAY "\ti = "
	movl $4, %eax # sys_write
	movl $1, %ebx # write to file descriptor 1 (stdout)
	movl $STR23, %ecx # pointer to the string to print
	movl $5, %edx # asks to print 5 characters our of the string passed in %ecx
	int $0x80 # call the system call		 # WORD ==> DISPLAY

# DISPLAY i
	push i
	pop %rdx                     # The value to be displayed
	movq $FormatString1, %rsi    # "%llu\n" 
	movl    $1, %edi 
	movl    $0, %eax 
	call    __printf_chk@PLT

# End of BlockStatement


jmp While20
EndWhile20:


# DISPLAY "WHILE statement test : \033[1m[OK]\033[0m\n"
	movl $4, %eax # sys_write
	movl $1, %ebx # write to file descriptor 1 (stdout)
	movl $STR25, %ecx # pointer to the string to print
	movl $36, %edx # asks to print 36 characters our of the string passed in %ecx
	int $0x80 # call the system call
# DISPLAY "\033[1mGoodbye\033[0m\n"
	movl $4, %eax # sys_write
	movl $1, %ebx # write to file descriptor 1 (stdout)
	movl $STR26, %ecx # pointer to the string to print
	movl $16, %edx # asks to print 16 characters our of the string passed in %ecx
	int $0x80 # call the system call
	movq %rbp, %rsp		# Restore the position of the stack's top
	ret			# Return from main function
