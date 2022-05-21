//--------------------------------------------------------------------------------------|
//---- Auteur :        Aubertin Emmanuel               	|  For: aTHO_compiler   	----|
//---- Description :   A compiler from a very simple Pascal-like structured    		----|
//----	                language LL(k) to 64-bit 80x86 Assembly langage        		----|
//---- Contact :       https://athomisos.fr                                    		----|
//--------------------------------------------------------------------------------------|


// SYSCALL => printf SYS_read | gcc -include sys/syscall.h -E -
//         => vim /usr/include/x86_64-linux-gnu/asm/unistd_64.h

#include <string>
#include <iostream>
#include <cstdlib>
#include <set>
#include <FlexLexer.h>
#include "tokeniser.h"
#include <cstring>
#include <fstream>
#include <map>
#include <list>
#include <algorithm>



std::string PROGNAME="aTHO_ Compiler";
std::string FILE_NAME= __FILE__;
std::string RELEASE="Revision 1.0 | Last update 23 jan 2022";
std::string AUTHOR="Aubertin Emmanuel";
std::string COPYRIGHT="(c) 2021 "+ AUTHOR + " from https://athomisos.fr";

void print_release() {
    std::cout << RELEASE << COPYRIGHT << std::endl;
}

using namespace std;

//Enable All DEBUG mode
//#define DEBUG // if define the asm wil #be have a lot of useless com

//DEBUG DISPLAY
//#define DEBUGDISP

enum OPREL {EQU, DIFF, INF, SUP, INFE, SUPE, WTFR};
enum OPADD {ADD, SUB, OR, WTFA};
enum OPMUL {MUL, DIV, MOD, AND ,WTFM};
enum TYPE {INT, BOOL, CHAR, DOUBLE, ARRAY, STR};



// Prototypage :
void IfStatement(void);
void WhileStatement(void);
void ForStatement(void);
void BlockStatement(void);
void DisplayStatement(void);
void ProcedureStatement(void);
void VarStatement(void);
TYPE Expression(void);
int getLength(string);
//VARTYPE Number(void);

bool isFunc = false;
int strcounter = 0;
size_t tempPosition; // !! Need to be reset after all use
string tempBlock;
string OutDeclarationPart;
string OutStatementPart;
string OutFunctionPart;


TOKEN current;				// Current token

FlexLexer* lexer = new yyFlexLexer; // This is the flex tokeniser
// tokens can be read using lexer->yylex()
// lexer->yylex() returns the type of the lexicon entry (see enum TOKEN in tokeniser.h)
// and lexer->YYText() returns the lexicon entry as a string

		
set<string> DeclaredFunc;
map<string, enum TYPE> DeclaredVariables;	// Store declared variables and their types
map<string, unsigned long long> SizeDeclaredArrays;  // Store the size of the declared arrays
map<string, enum TYPE> TypeDeclaredArrays;  // Store the type of the declared arrays


unsigned long TagNumber=0;

bool IsDeclared(const char *id){
	return DeclaredVariables.find(id) != DeclaredVariables.end();
}

bool IsFunc(const char *id){
	return DeclaredFunc.find(id) != DeclaredFunc.end();
}

void Error(string s){
	cerr  <<  "Ligne n°" << lexer->lineno() << ", lu : '" << lexer->YYText() << "'(" << current << "), mais ";
	cerr <<  s  <<  endl;
	exit(-1);
}

// Program := [DeclarationPart] StatementPart
// DeclarationPart := "[" Letter {"," Letter} "]"
// StatementPart := Statement {";" Statement} "."
// Statement := AssignementStatement
// AssignementStatement := Letter "=" Expression

// Expression := SimpleExpression [RelationalOperator SimpleExpression]
// SimpleExpression := Term {AdditiveOperator Term}
// Term := Factor {MultiplicativeOperator Factor}
// Factor := Number | Letter | "(" Expression ")"| "!" Factor
// Number := Digit{Digit}

// AdditiveOperator := "+" | "-" | " || "
// MultiplicativeOperator := "*" | "/" | "%" | " && "
// RelationalOperator := " == " | " != " | "<" | ">" | "<=" | ">="  
// Digit := "0"|"1"|"2"|"3"|"4"|"5"|"6"|"7"|"8"|"9"
// Letter := "a"|...|"z"
	
TYPE Identifier(void){
	if(!IsDeclared(lexer->YYText())){
		cerr << "Erreur : Variable '"<<lexer->YYText()<<"' non déclarée"<<endl;
		exit(-1);
	}

	enum TYPE VarType = DeclaredVariables[lexer->YYText()];

	if(isFunc){
		OutFunctionPart  += "\tpush ";
		OutFunctionPart += lexer->YYText();
		OutFunctionPart += "\n";
	} else {
		OutStatementPart += "\tpush ";
		OutStatementPart += lexer->YYText();
		OutStatementPart += "\n";
	}

	current=(TOKEN) lexer->yylex();
	return VarType;
}

TYPE Number(void){
		if(isFunc){
		OutFunctionPart += "\tpush $" + to_string(atoi(lexer->YYText())) + "\n";
	} else {
		OutStatementPart +=  "\tpush $" + to_string(atoi(lexer->YYText())) + "\n";
	}
	current=(TOKEN) lexer->yylex();
	return INT;
}

TYPE Factor(void){
	enum TYPE typeExp;
	if(current == RPARENT){
		current=(TOKEN) lexer->yylex();
		typeExp = Expression();
		if(current != LPARENT)
			Error("')' était attendu");		// ")" expected
		else
			current=(TOKEN) lexer->yylex();
	}
	else{
		if (current == NUMBER){
			typeExp = Number();
		} else {
			if(current == ID){
				typeExp = Identifier();
			} else {
				Error("'(' ou chiffre ou lettre attendue");
			}
		}
	}
	return typeExp;
}

// MultiplicativeOperator := "*" | "/" | "%" | " && "
OPMUL MultiplicativeOperator(void){
	OPMUL opmul;
	if(strcmp(lexer->YYText(),"*") == 0)
		opmul=MUL;
	else if(strcmp(lexer->YYText(),"/") == 0)
		opmul=DIV;
	else if(strcmp(lexer->YYText(),"%") == 0)
		opmul=MOD;
	else if(strcmp(lexer->YYText()," && ") == 0)
		opmul=AND;
	else opmul=WTFM;
	current=(TOKEN) lexer->yylex();
	return opmul;
}

// Term := Factor {MultiplicativeOperator Factor}
TYPE Term(void){
	TYPE firstFactor, secondFactor;
	OPMUL mulop;
	firstFactor = Factor();
	while(current == MULOP){
		mulop=MultiplicativeOperator();		// Save operator in local variable
		secondFactor = Factor();
		if(firstFactor != secondFactor){ // If not same type
			Error("Type non compatible !");
		}
		if(isFunc){
			OutFunctionPart +="\tpop %rbx\n\
\tpop %rax\n";	// get second operand
		} else {
			OutStatementPart +=  "\tpop %rbx\n\
\tpop %rax\n";	// get second operand	
		}
		
		switch(mulop){
			case AND:
				if(isFunc){
					OutFunctionPart += "\tmulq	%rbx\n\
\tpush %rax\t# AND\n";	// store result
				} else {
					OutStatementPart += "\tmulq	%rbx\n\
\tpush %rax\t# AND\n";	// store result
				}

				break;
			case MUL:
				if(isFunc){
					OutFunctionPart += "\tmulq	%rbx\n\
\tpush %rax\t# MUL\n";	// store result
				} else {
					OutStatementPart +=  "\tmulq	%rbx\n\
\tpush %rax\t# MUL\n";	// store result
				}

				break;
			case DIV:
				if(isFunc){
					OutFunctionPart +="\tmovq $0, %rdx\n\
\tdiv %rbx\n\
\tpush %rax\t# DIV\n";		// store result
				} else {
				OutStatementPart +=  "\tmovq $0, %rdx\n\
\tdiv %rbx\n\
\tpush %rax\t# DIV\n";		// store result
				}

				break;
			case MOD:
				if(isFunc){
					OutFunctionPart +=  "\tmovq $0, %rdx\n\
\tdiv %rbx\n\
\tpush %rdx\t# MOD\n";		// store result
				} else {
				OutStatementPart +=  "\tmovq $0, %rdx\n\
\tdiv %rbx\n\
\tpush %rdx\t# MOD\n";		// store result
				}

				break;
			default:
				Error("opérateur multiplicatif attendu");
		}
	}
	return firstFactor;
}

// AdditiveOperator := "+" | "-" | " || "
OPADD AdditiveOperator(void){
	OPADD opadd;
	if(strcmp(lexer->YYText(),"+") == 0)
		opadd=ADD;
	else if(strcmp(lexer->YYText(),"-") == 0)
		opadd=SUB;
	else if(strcmp(lexer->YYText()," || ") == 0)
		opadd=OR;
	else opadd=WTFA;
	current=(TOKEN) lexer->yylex();
	return opadd;
}

// SimpleExpression := Term {AdditiveOperator Term}
TYPE SimpleExpression(void){
	#ifdef DEBUG
		std::cout << endl << "# IN void SimpleExpression(void)" << endl;
	#endif
	TYPE firstTerm, secondTerm;
	OPADD adop;
	firstTerm = Term();
	while(current == ADDOP){
		adop=AdditiveOperator();		// Save operator in local variable
		secondTerm = Term();
		if(firstTerm != secondTerm){// If not same type
			Error("Type non compatible !");
		}
		if(isFunc){
			OutFunctionPart +=  "\tpop %rbx\n\
			\tpop %rax\n";	// get second operand
		} else {
			OutStatementPart +=  "\tpop %rbx\n\
			\tpop %rax\n";	// get second operand
		}


		switch(adop){
			case OR:
				if(isFunc){
					OutFunctionPart +="\taddq	%rbx, %rax\t# OR\n";// operand1 OR operand2
				} else {
					OutStatementPart += "\taddq	%rbx, %rax\t# OR\n";// operand1 OR operand2
				}
				
				break;			
			case ADD:
				if(isFunc){
					OutFunctionPart +=  "\taddq	%rbx, %rax\t# ADD\n";	// add both operands
				} else {
					OutStatementPart += "\taddq	%rbx, %rax\t# ADD\n";	// add both operands
				}
				break;			
			case SUB:	
				if(isFunc){
					OutFunctionPart +="\tsubq	%rbx, %rax\t# SUB\n";	// substract both operands
				} else {
					OutStatementPart +="\tsubq	%rbx, %rax\t# SUB\n";	// substract both operands
				}
				break;
			default:
				Error("opérateur additif inconnu");
		}
		if(isFunc){
			OutFunctionPart + "\tpush %rax\n";	 // store result
		} else {
			OutStatementPart += "\tpush %rax\n";	 // store result
		}
				
	}
	return firstTerm;
}

// DeclarationPart := "[" Ident {"," Ident} "]"
void DeclarationPart(void){
	TYPE currentTYPE = INT;
	string VarName;
	OutDeclarationPart +=  "\t.data\n\
FormatString:    .string \"%c\" # For char\n\
FormatString1:    .string \"%llu\\n\"\n\
FormatString2:\t.string \"%lf\" # For Float\n\
TrueSTR:\t.string \"\033[1mTRUE\033[0m\"\n\
FalseSTR:\t.string \"\033[1mFALSE\033[0m\"\n";
	if(current == RBRACKET){
		#ifdef DEBUG
			std::cout << "# IN [" << endl;
		#endif
		OutDeclarationPart += "\t.align 8\n";
		
		current=(TOKEN) lexer->yylex();
		if(current != ID)
			Error("Un identificater était attendu");
		OutDeclarationPart +=   lexer->YYText();
		VarName = (string)lexer->YYText();
		OutDeclarationPart +=   ":\t.quad 0\n";
		
		DeclaredVariables[VarName]=currentTYPE;
		//DeclaredVariables.insert(lexer->YYText());
		VarName = (string)lexer->YYText();
		DeclaredVariables[VarName]=currentTYPE;
		// add in map for type
		current=(TOKEN) lexer->yylex();
		while(current == COMMA){
			current=(TOKEN) lexer->yylex();
			if(current != ID)
				Error("Un identificateur était attendu");
			OutDeclarationPart +=   lexer->YYText();
			OutDeclarationPart +=   ":\t.quad 0\n";

			//DeclaredVariables.insert(lexer->YYText());
			VarName = (string)lexer->YYText();
			DeclaredVariables[VarName]=currentTYPE;
			// need to add map of aTHOVar
			current=(TOKEN) lexer->yylex();
		}
		current=(TOKEN) lexer->yylex();
	}
	
}

// RelationalOperator := " = " | " != " | "<" | ">" | "<=" | ">="  
OPREL RelationalOperator(void){
	OPREL oprel;
	if(strcmp(lexer->YYText(),"=") == 0)
		oprel=EQU;
	else if(strcmp(lexer->YYText(),"!=") == 0)
		oprel=DIFF;
	else if(strcmp(lexer->YYText(),"<") == 0)
		oprel=INF;
	else if(strcmp(lexer->YYText(),">") == 0)
		oprel=SUP;
	else if(strcmp(lexer->YYText(),"<=") == 0)
		oprel=INFE;
	else if(strcmp(lexer->YYText(),">=") == 0)
		oprel=SUPE;
	else oprel=WTFR;
	current=(TOKEN) lexer->yylex();
	return oprel;
}

// Expression := SimpleExpression [RelationalOperator SimpleExpression]
TYPE Expression(void){
	TYPE firstPart, secondPart; 
	#ifdef DEBUG
		std::cout << endl << "# -- Expression(void) --" << endl;
	#endif
	OPREL oprel;
	firstPart = SimpleExpression();
	#ifdef DEBUG
		std::cout << "\t\t# First part ==>" << firstPart << endl;
	#endif
	if(current == RELOP){
		oprel=RelationalOperator();
		#ifdef DEBUG
			std::cout << "\t\t# OP ==>" << oprel << endl;
		#endif
		secondPart = SimpleExpression();
		#ifdef DEBUG
			std::cout << "\t\t# Second part ==>" << firstPart << endl;
		#endif
		if(firstPart != secondPart){ // If not same type
			Error("Type non compatible !");
		}


		if( firstPart != DOUBLE){
			if(isFunc){
				OutFunctionPart +="\tpop %rax\n\
	\tpop %rbx\n\
	\tcmpq %rax, %rbx\n";
			} else {
				OutStatementPart += "\tpop %rax\n\
	\tpop %rbx\n\
	\tcmpq %rax, %rbx\n";
			}
		} else {
			if(isFunc){
				OutFunctionPart +="\tfldl	(%rsp)\n\
					\tfldl	8(%rsp)\t# operand 1 in %st(0) && operand2 in %st(1)\n\
					\t addq $16, %rsp\n\
					\tfcomip %st(1)\n\
					\tfaddp %st(1)\n";
			} else {
				OutStatementPart +="\tfldl	(%rsp)\n\
					\tfldl	8(%rsp)\t# operand 1 in %st(0) && operand2 in %st(1)\n\
					\t addq $16, %rsp\n\
					\tfcomip %st(1)\n\
					\tfaddp %st(1)\n";
			}
		}


		switch(oprel){
			case EQU:
				#ifdef DEBUG
					cout << "\t\t# EQU expression" << endl;
				#endif
				if(isFunc){
					OutFunctionPart += "\tje Vrai" + to_string(TagNumber) + "\t# If equal\n";;
				} else {
					OutStatementPart += "\tje Vrai" + to_string(TagNumber) + "\t# If equal\n";
				}
				break;
			case DIFF:
				#ifdef DEBUG
					cout << "\t\t# DIFF expression" << endl;
				#endif
				if(isFunc){
					OutFunctionPart += "\tjne Vrai" + to_string(TagNumber) + "\t# If different\n";
				} else {
					OutStatementPart += "\tjne Vrai" + to_string(TagNumber) + "\t# If different\n";
				}
				
				break;
			case SUPE:
				#ifdef DEBUG
					cout << "\t\t# SUPE expression" << endl;
				#endif
				if(isFunc){
					OutFunctionPart += "\tjae Vrai" + to_string(TagNumber) + "\t# If above or equal\n";
				} else {
					OutStatementPart += "\tjae Vrai" + to_string(TagNumber) + "\t# If above or equal\n";
				}
				
				break;
			case INFE:
				#ifdef DEBUG
					cout << "\t\t# INFE expression" << endl;
				#endif
				if(isFunc){
					OutFunctionPart += "\tjbe Vrai" + to_string(TagNumber) + "\t# If below or equal\n";
				} else {
					OutStatementPart += "\tjbe Vrai" + to_string(TagNumber) + "\t# If below or equal\n";
				}
				
				break;
			case INF:
				#ifdef DEBUG
					cout << "\t\t# INF expression" << endl;
				#endif
				if(isFunc){
					OutFunctionPart += "\tjb Vrai" + to_string(TagNumber) + "\t# If below\n";
				} else {
					OutStatementPart += "\tjb Vrai" + to_string(TagNumber) + "\t# If below\n";
				}
				
				break;
			case SUP:
				#ifdef DEBUG
					cout << "\t\t# SUP expression" << endl;
				#endif
				if(isFunc){
					OutFunctionPart += "\tja Vrai" + to_string(TagNumber) + "\t# If above\n";
				} else {
					OutStatementPart += "\tja Vrai" + to_string(TagNumber) + "\t# If above\n";
				}
				
				break;
			default:
				Error("Opérateur de comparaison inconnu");
		}
		if(isFunc){
			tempPosition = OutFunctionPart.length();
		} else {
			tempPosition = OutStatementPart.length();
		}
		
	#ifdef DEBUG
		cout << "# End of Expression()" << endl;
	#endif
	return BOOL;
	}
	return firstPart;
}

// AssignementStatement := Identifier ":=" Expression
// Non type variable
void AssignementStatement(void){
	#ifdef DEBUG
		std::cout << "# ----------- In AssignementStatement(void) ----------- " << endl;
	#endif
	string variable;
	if(current != ID)
		Error("Identificateur attendu");
	if(!IsDeclared(lexer->YYText()) && !IsFunc(lexer->YYText())){
		cerr  <<  "Erreur : Variable ou procedure'" << lexer->YYText() << "' non déclarée" << endl;
		exit(-1);
	}
	variable=lexer->YYText();

	if(IsDeclared(lexer->YYText())){
		#ifdef DEBUG
			std::cout << "# C'est une variable !! " << endl;
		#endif
		current=(TOKEN) lexer->yylex();
		if(current != ASSIGN)
			Error("caractères ':=' attendus");
		current=(TOKEN) lexer->yylex();
		enum TYPE VarType = DeclaredVariables[variable];
		#ifdef DEBUG
			cout << "# AVANT IF " << lexer->YYText() << endl;
			cout << "# TYPE  " << VarType << endl;
			cout << "# SIZE  " << getLength(lexer->YYText()) << endl;
			cout << "# current  " << current << endl;
		#endif	
		

		if(current == STRINGCONST && getLength(lexer->YYText()) == 3) {
			string chartemp = lexer->YYText();
			#ifdef DEBUG
				cout << "# in if" << endl << "\t# pop '" << chartemp[1] <<"'\n";
			#endif
			if(isFunc){
				OutFunctionPart +=  "\tpop '"; 
				OutFunctionPart += chartemp[1];
				OutFunctionPart +=  "'\n";
			} else {
				OutStatementPart += "\tpop '"; 
				OutStatementPart += chartemp[1];
				OutStatementPart +=  "'\n";
			}
			current=(TOKEN) lexer->yylex();
			current=(TOKEN) lexer->yylex();
			return;
		}
		enum TYPE ExpType = DOUBLE;
		if(VarType != DOUBLE){
			ExpType = Expression();
		}
		

		if(VarType != ExpType){
			Error("Can't different type");
		}

		switch(VarType){
			case DOUBLE:
				cout << "# IN DOUBLE" << endl;
				if(isFunc){
					OutFunctionPart +=  "\tpush ";
					OutFunctionPart += lexer->YYText();
					OutFunctionPart +=  "pop %rax\n\
					\tmovb %al, " + variable +"\n";
				} else {
					OutStatementPart +=  "\tpop %rax\n\
					\tmovb %al, " + variable +"\n";
				}
				current=(TOKEN) lexer->yylex();
			break;
			default:
				cout << "# IN DEFAULT" << endl;
				if(isFunc){
					OutFunctionPart +=  "\tpop " + variable +"\n";
				} else {
					OutStatementPart +=  "\tpop " + variable +"\n";
				}
			break;
		}
	}

	if(IsFunc(lexer->YYText())){
		#ifdef DEBUG
			std::cout << "# C'est une fonction !! " << endl;
		#endif
		if(isFunc){
			OutFunctionPart += "\tcall " + variable +"\n";
		} else {
			OutStatementPart +=  "\tcall " + variable +"\n";
		}
		current=(TOKEN) lexer->yylex();
	}
}

// Statement := AssignementStatement
void Statement(void){
	if(strcmp(lexer->YYText(),"" ) != 0){
		#ifdef DEBUG
			std::cout << "# ----------- In Statement(void) ----------- " << endl;
		#endif

		if((TOKEN) current == KEYWORD){
			#ifdef DEBUG
				std::cout << "# TOKEN == KEYWORD"<< endl;
			#endif

			if( strcmp(lexer->YYText(),"IF" ) == 0){
				#ifdef DEBUG
					std::cout << "# IF STATEMENT" << endl ;
				#endif
				IfStatement();
				return;
			}
			
			if( strcmp(lexer->YYText(),"THEN") == 0 || strcmp(lexer->YYText(),"ELSE") == 0){ // If we have then or else outside of an if do ..
				#ifdef DEBUG
					std::cout << "# THEN or ELSE outside of IF" << endl ;
				#endif
				Error("You need to be in a IF");
				return;
			} 
			
			if( strcmp(lexer->YYText(),"WHILE" ) == 0){
				#ifdef DEBUG
					std::cout << "# WHILE STATEMENT" << endl ;
				#endif
				WhileStatement();
				return;
			}
			if( strcmp(lexer->YYText(),"FOR" ) == 0){
				#ifdef DEBUG
					std::cout << "# FOR STATEMENT" << endl ;
				#endif
				ForStatement();
				return;
			}
			
			if( strcmp(lexer->YYText(),"BEGIN" ) == 0){
				#ifdef DEBUG
					std::cout << "# BLOCK STATEMENT" << endl ;
				#endif
				BlockStatement();
				return;
			}
			if( strcmp(lexer->YYText(),"DO" ) == 0){
				#ifdef DEBUG
					std::cout << "# BLOCK STATEMENT" << endl ;
				#endif
				BlockStatement(); 
				return;
			}
			if( strcmp(lexer->YYText(),"END" ) == 0){
				#ifdef DEBUG
					std::cout << "# END STATEMENT" << endl ;
				#endif
				Statement();
				return;
			}
			
			if( strcmp(lexer->YYText(),"DISPLAY" ) == 0){
				#ifdef DEBUG
					std::cout << "# DISPLAY STATEMENT" << endl ;
				#endif
				DisplayStatement();
				return;
			}

			if( strcmp(lexer->YYText(),"PROCEDURE" ) == 0){
				#ifdef DEBUG
					std::cout << "# PROCEDURE STATEMENT" << endl ;
				#endif
				ProcedureStatement();
				return;
			}
			
			if( strcmp(lexer->YYText(),"EXIT" ) == 0){
				#ifdef DEBUG
					std::cout << "# EXIT STATEMENT" << endl ;
				#endif
					if(isFunc){
						OutFunctionPart +="\tmovl $1, %eax  # System call number 1: exit()\n\tmovl $0, %ebx  # Exits with exit status 0\n\tint $0x80\t# Interupte prog\n";
					} else {
						OutStatementPart += "\tmovl $1, %eax  # System call number 1: exit()\n\tmovl $0, %ebx  # Exits with exit status 0\n\tint $0x80\t# Interupte prog\n";
					}
				
				Statement(); // But continue to compile
				return;
			}
			
			if( strcmp(lexer->YYText(),"VAR" ) == 0) {
				VarStatement();
				return;
			}

			Error("Unkwon keyword :");
		}
		if( current == ID ){
			#ifdef DEBUG
				std::cout << "# # TOKEN == ID" << endl;
			#endif
			AssignementStatement();
			return;
		}
		if( strcmp(lexer->YYText(), ".") == 0){
			return;
		}
		Error("Instruction unreachable !");
		}
}

int getLength(string input) {
	int length = 0;
	#ifdef DEBUGDISP
		std::cout << "# getLength(" << input << ")" << endl;
	#endif
	for(int i = 0; i < input.length(); i++){
		#ifdef DEBUGDISP
			std::cout << "# Testing ==> " << input[i] << ")" << endl;
		#endif
		if( input[i] == '\\'){
				#ifdef DEBUGDISP
					std::cout << "# In \\ if" << input << ")" << endl;
				#endif
				i++;
			if(input[i] == '0' || input[i] == 'e' || input[i] == 'x'){ // if something like \033[0;31m (RED COLOR)
				#ifdef DEBUGDISP
					std::cout << "# In 2nd if" << input << ")" << endl;
				#endif
				while(input[i] != 'm'){
					if(input[i] == 'm'){
						break;
					}
					i++;
					if(i > input.length()){ 
						Error("OULALA");
					}		
				}
				length +=4;
			} else {
				#ifdef DEBUGDISP
					std::cout << "\t# COUNT ==>\"" << input[i] << "\"" <<endl;
				#endif
				length +=1;
			}
		} else {
			#ifdef DEBUGDISP
				std::cout << "\t# COUNT ==>\"" << input[i] << "\"" <<endl;
			#endif
			length +=1;
		}
	}
	#ifdef DEBUGDISP
		std::cout << "# Length of \"" << input << "\" ==> " << length <<endl;
	#endif
	return length;
}

enum TYPE Type(void){
	if(current!=KEYWORD)
		Error("Type needed !");

	if( current == STRINGCONST){
		current=(TOKEN) lexer->yylex();
		return STR;
	}
	
	if( strcmp(lexer->YYText(),"BOOL") == 0 ){
		current=(TOKEN) lexer->yylex();
		return BOOL;
	}

	if( strcmp(lexer->YYText(),"INT") == 0 ){
		current=(TOKEN) lexer->yylex();
		return INT;
	}

	if( strcmp(lexer->YYText(),"DOUBLE") == 0 ){
		current=(TOKEN) lexer->yylex();
		return DOUBLE;
	}
	if( strcmp(lexer->YYText(),"CHAR") == 0 ){
		current=(TOKEN) lexer->yylex();
		return CHAR;
	}

	if( strcmp(lexer->YYText(), "ARRAY") == 0 ) {
		current = (TOKEN) lexer->yylex();
		return ARRAY;
	}
	Error("Unrecognized type, plz read the doc !");
	
}

void ProcedureStatement(void) {
	isFunc = true;

	#ifdef DEBUG
		std::cout << "# ----------- WhileStatement(void) -----------" << endl;
	#endif
	unsigned long long tag=++TagNumber;
	current = (TOKEN) lexer->yylex();
	#ifdef DEBUG
		std::cout << "# NAME OF PROC ==>" << lexer->YYText() << endl;
	#endif
	DeclaredFunc.insert(lexer->YYText());
	OutFunctionPart += lexer->YYText();
	OutFunctionPart += " :\n";

	current = (TOKEN) lexer->yylex();
	#ifdef DEBUG
		std::cout << "# In Bracket " << lexer->YYText() << endl;
	#endif
	current = (TOKEN) lexer->yylex();


	current = (TOKEN) lexer->yylex();
	#ifdef DEBUG
		std::cout << "# : " << lexer->YYText() << endl;
	#endif
	current = (TOKEN) lexer->yylex();
	current = (TOKEN) lexer->yylex();

	BlockStatement();

	#ifdef DEBUG
		std::cout << "# END OF PROC " << lexer->YYText() << endl;
	#endif
	OutFunctionPart += "ret\n";
	isFunc = false;
}

// DISPLAY <Expression>
void DisplayStatement(void){
	unsigned long long tag=++TagNumber;
	#ifdef DEBUGDISP
		std::cout << "# ----------- In DisplayStatement(void) -----------" << endl;
	#endif

	if(isFunc){
		OutFunctionPart +=" \n# DISPLAY ";
	} else {
		OutStatementPart += "\n# DISPLAY ";
	}
	
	current = (TOKEN) lexer->yylex();

	if(current == STRINGCONST){
		string currentWord = lexer->YYText();
			if(isFunc){
				OutFunctionPart += currentWord + "\n";
			} else {
				OutStatementPart += currentWord + "\n";
			}
		

		currentWord.erase(0, 1); // erase the first "
		currentWord.erase(currentWord.size() - 1 ); // erase the " of the end
		int len = getLength(currentWord); // Not good, because of \n \t ect..
		
		OutDeclarationPart += "STR" + to_string(TagNumber) + ":";
		OutDeclarationPart += "\t.string \"" + currentWord + "\"\n";
		if(isFunc){
			OutFunctionPart += "\tmovl $4, %eax # sys_write\n\
\tmovl $1, %ebx # write to file descriptor 1 (stdout)\n\
\tmovl $STR" + to_string(TagNumber) + ", %ecx # pointer to the string to print\n\
\tmovl $" + to_string(len) + ", %edx # asks to print " + to_string(len) + " characters our of the string passed in %ecx\n\
\tint $0x80 # call the system call\n";
		} else {
			OutStatementPart += "\tmovl $4, %eax # sys_write\n\
\tmovl $1, %ebx # write to file descriptor 1 (stdout)\n\
\tmovl $STR" + to_string(TagNumber) + ", %ecx # pointer to the string to print\n\
\tmovl $" + to_string(len) + ", %edx # asks to print " + to_string(len) + " characters our of the string passed in %ecx\n\
\tint $0x80 # call the system call\n";

		}
		
		current = (TOKEN) lexer->yylex();
	} else { // can be only a int for nowS
		#ifdef DEBUGDISP
			std::cout << "# OTHER" << endl;
		#endif
		if(isFunc){
			OutFunctionPart += lexer->YYText();
			OutFunctionPart += "\n";
		} else {
			OutStatementPart += lexer->YYText();
			OutStatementPart += "\n";
		}
		enum TYPE TypeExp = Expression();
		cout << "TYPE = " << TypeExp << endl;
		switch(TypeExp){
			case CHAR:
				if(isFunc){
					OutFunctionPart += "\tpop %rdx                     # TYPE CHAR\n\
			\tmovq $FormatString, %rsi  \n\
			\tmovl    $1, %edi \n\
			\tmovl    $0, %eax \n\
			\tcall    __printf_chk@PLT\n";
				} else {
					OutStatementPart += "\tpop %rdx                     # TYPE CHAR\n\
			\tmovq $FormatString, %rsi   \n\
			\tmovl    $1, %edi \n\
			\tmovl    $0, %eax \n\
			\tcall    __printf_chk@PLT\n";
			}
			break;
			case INT:
				if(isFunc){
					OutFunctionPart += "\tpop %rdx                     # TYPE INT\n\
			\tmovq $FormatString1, %rsi    # \"%llu\\n\" \n\
			\tmovl    $1, %edi \n\
			\tmovl    $0, %eax \n\
			\tcall    __printf_chk@PLT\n";
				} else {
					OutStatementPart += "\tpop %rdx                     # TYPE INT\n\
			\tmovq $FormatString1, %rsi    # \"%llu\\n\" \n\
			\tmovl    $1, %edi \n\
			\tmovl    $0, %eax \n\
			\tcall    __printf_chk@PLT\n";
			}
			break;
			case DOUBLE:
				if(isFunc){
					OutFunctionPart += "\tmovsd (%rsp), %xmm0\t# put stack on top of %xmm0\n\
					\tsubq	$16, %rsp\t\t# allocation for 3 additional doubles\n\
				\tmovsd %xmm0, 8(%rsp)\n\
				\tmovq $FormatString2, %rdi\t# \"%lf\\n\"\n\
				\tmovq	$1, %rax\n\
				\tcall	printf\n\
				nop\n\
				\taddq $24, %rsp\n";
				} else {
					OutStatementPart += "\tmovsd (%rsp), %xmm0\t# put stack on top of %xmm0\n\
					\tsubq	$16, %rsp\t\t# add allocation for more doubles\n\
				\tmovsd %xmm0, 8(%rsp)\n\
				\tmovq $FormatString2, %rdi\n\
				\tmovq	$1, %rax\n\
				\tcall	printf\n\
				nop\n\
				\taddq $24, %rsp\n";
				}
			break;
			case BOOL:
				if(isFunc){
					OutFunctionPart += "\tpop %rdx\t# Print True if 1 and False if 0\n\
				\tcmpq $0, %rdx\n\
				\tje False" + std::to_string(tag) + "\n\
				\tmovq $TrueSTR, %rdi\t#  put true in rdi\n\
				\tjmp Next" + std::to_string(tag) + "\n\
				False" + std::to_string(tag) + ":\n\
				\tmovq $FalseSTR, %rdi\t# put false in rdi \n\
				Next" + std::to_string(tag) + ":\n\
				\tcall	puts@PLT\t# print rdi\n";
				} else {
					OutStatementPart += "\tpop %rdx\t# Print True if 1 and False if 0\n\
				\tcmpq $0, %rdx\n\
				\tje False" + std::to_string(tag) + "\n\
				\tmovq $TrueSTR, %rdi\t#  put true in rdi\n\
				\tjmp Next" + std::to_string(tag) + "\n\
				False" + std::to_string(tag) + ":\n\
				\tmovq $FalseSTR, %rdi\t# put false in rdi \n\
				Next" + std::to_string(tag) + ":\n\
				\tcall	puts@PLT\t# print rdi\n";
				}
			break;
			default:
				Error("Unrecognized type, plz read the doc !");
			break;
			
		}
	}

}

// WHILE <Expression> DO <Statement> 	
void WhileStatement(void){
	TYPE expType;
	#ifdef DEBUG
		std::cout << "# ----------- WhileStatement(void) -----------" << endl;
	#endif
	unsigned long long tag=++TagNumber;
	current = (TOKEN) lexer->yylex();
	if(isFunc){
		OutFunctionPart += "while" + to_string(tag) + ":\n";
	} else {
		OutStatementPart += "while" + to_string(tag) + ":\n";
	}


	if(current == RPARENT && strcmp(lexer->YYText(),"(")  ==  0){
		expType = Expression();
		if(expType != BOOL){
			Error("BOOL expression needed !");
		}
	} else {
		Error("Expression needed ! `WHILE <Expression> DO <Statement>.`");
	}
	if(isFunc){
		OutFunctionPart += "\tjmp EndWhile" + to_string(tag) + "\nVrai" + to_string(tag)+":\n";
	} else {
		OutStatementPart +=  "\tjmp EndWhile" + to_string(tag) + "\nVrai" + to_string(tag)+":\n";
	}
	
	if(current == KEYWORD && strcmp(lexer->YYText(),"DO")  ==  0){
		current = (TOKEN) lexer->yylex();
		BlockStatement();
	} else {
		Error("DO missing !");
	}
		if(isFunc){
		OutFunctionPart +="jmp while" + to_string(tag) + "\n\
EndWhile" + to_string(tag) + ":\n";
	} else {
			OutStatementPart += "jmp while" + to_string(tag) + "\n\
EndWhile" + to_string(tag) + ":\n";
	}

}

// FOR <AssignementStatement> To <Expression> DO <Statement>
void ForStatement(void){
	#ifdef DEBUG
		std::cout << "# ----------- ForStatement(void) -----------" << endl;
	#endif
	unsigned long long tag=++TagNumber;
	current = (TOKEN) lexer->yylex();

	#ifdef DEBUG
		std::cout << "\t# FOR i = " << lexer->YYText() << endl;
	#endif
	
	if( current == NUMBER){
			if(isFunc){
				OutFunctionPart += "movq $" ;
				OutFunctionPart +=  lexer->YYText(); 
				OutFunctionPart += ", %rcx # Get Start value\n";
			} else {
				OutStatementPart += "movq $" ;
				OutStatementPart +=  lexer->YYText(); 
				OutStatementPart += ", %rcx # Get Start value\n";
			}

		current = (TOKEN) lexer->yylex();
	} else {
		Error("Need Digit");
	}

	OutDeclarationPart += "i" + to_string(tag) + ": .quad 0\n";
	if(isFunc){
		OutFunctionPart += "For" + to_string(tag) + ":\n";
	} else {
		OutStatementPart += "For" + to_string(tag) + ":\n";
	}


	if( current == KEYWORD || strcmp(lexer->YYText(), "To") == 0){
		current = (TOKEN) lexer->yylex(); // Get digit after To
		if(isFunc){
			OutFunctionPart +=  "\tcmp $";
			OutFunctionPart +=  lexer->YYText();
			OutFunctionPart += ", i" + to_string(tag) + " # To KEYWORD\n\
\tjae EndFor"+ to_string(tag) + "\n";
		} else {
		OutStatementPart +=  "\tcmp $";
		OutStatementPart +=  lexer->YYText();
		OutStatementPart += ", i" + to_string(tag) + " # To KEYWORD\n\
\tjae EndFor"+ to_string(tag) + "\n";
		}


		current = (TOKEN) lexer->yylex(); // Get Do
	} else {
		Error("To requiered");
	}

	if( current == KEYWORD || strcmp(lexer->YYText(), "DO") == 0 ){
		if(isFunc){
			OutFunctionPart += "push i" + to_string(tag) + "\n\
\tpush $1\n\
\tpop %rbx\n\
\tpop %rax\n\
\taddq	%rbx, %rax	# ADD\n\
\tpush %rax\n\
\tpop i" + to_string(tag) + "\n\n";
		} else {
			OutStatementPart += "push i" + to_string(tag) + "\n\
\tpush $1\n\
\tpop %rbx\n\
\tpop %rax\n\
\taddq	%rbx, %rax	# ADD\n\
\tpush %rax\n\
\tpop i" + to_string(tag) + "\n\n";
		}

		current = (TOKEN) lexer->yylex(); // Get Expression
		BlockStatement();
		#ifdef DEBUG
			std::cout << "# After BlockStatement();" << lexer->YYText() << endl;
		#endif
	} else {
		Error("DO requiered");
	}
	if(isFunc){
		OutFunctionPart += "\n\tjmp For" + to_string(tag) + "\n\
EndFor" + to_string(tag) + ":\n\
\tpush $0\n\
\tpop i" + to_string(tag);
	} else {
		OutStatementPart += "\n\tjmp For" + to_string(tag) + "\n\
EndFor" + to_string(tag) + ":\n\
\tpush $0\n\
\tpop i" + to_string(tag);
	}

	#ifdef DEBUG
		std::cout << "# End of for" << lexer->YYText() << endl;
	#endif
}

// "BEGIN" Statement { ";" Statement } "END"
void BlockStatement(void){
	#ifdef DEBUG
		std::cout << "# ----------- BlockStatement(void) -----------" << endl;
	#endif
	// unsigned long long tag=++TagNumber; Use less here
	/*if( current == KEYWORD){
		Statement();
	}*/
	while ( current != KEYWORD || strcmp(lexer->YYText(), "END") != 0)
	{
		#ifdef DEBUG
			cout << "\t\t # WORD ==> "<< lexer->YYText()<< endl;
		#endif
		if( strcmp(lexer->YYText(), "END") != 0){
			Statement();
		} else{
			current = (TOKEN) lexer->yylex();// skip the ;
			break;
		}
		if(strcmp(lexer->YYText(), ";") == 0){
			#ifdef DEBUG
				std::cout << "\t# CURRENT ==> " << lexer->YYText() << endl;
			#endif
			current = (TOKEN) lexer->yylex();// skip the ;
		} else {
			//Error(";");
		}
	}
	#ifdef DEBUG
		std::cout << "\t# Before End of  BlockStatement" << endl;
	#endif
	//current = (TOKEN) lexer->yylex();
	//if( current == KEYWORD && strcmp(lexer->YYText(), "END") == 0){
		current = (TOKEN) lexer->yylex();
		if(strcmp(lexer->YYText(), ";") == 0){
			current = (TOKEN) lexer->yylex(); // skip ;
		}
	#ifdef DEBUG
		std::cout << "# End of BlockStatement" << lexer->YYText() << endl;
	#endif
}


// "IF" Expression "THEN" Action ["ELSE" Action (or can be a another if)]
void IfStatement(void){
	#ifdef DEBUG
		std::cout << "# ----------- IFStatement(void) -----------" << endl;
	#endif
	TYPE expType;
	unsigned long long tag=++TagNumber; // For unique name in asm program

	if(current != KEYWORD || strcmp(lexer->YYText(),"IF")  !=  0){
		Error("In void IfStatement(void) without if");
	}
	current = (TOKEN) lexer->yylex();


	if(current != RPARENT && strcmp(lexer->YYText(),"(")  !=  0){
		Error("Une exp était attendu");
	}
	#ifdef DEBUG
		cout << "\t# Reading the Exp :" << endl;
		#define DEBUGEXP
	#endif
	expType = Expression();
	if(expType != BOOL){
		Error("BOOL expression needed !");
	}

	
	#ifdef DEBUG
		std::cout << "# WORD ==> " << lexer->YYText() << endl;
	#endif

	if(  current != KEYWORD || strcmp(lexer->YYText(),"THEN")  !=  0 ) { // Mémo en pascal on écrit IF Machin THEN JeFaisMachin
		Error(" Un 'THEN' attendu, en pascal on écrit IF Machin THEN JeFaisMachin");
	}
	current=(TOKEN) lexer->yylex();
	if(isFunc){
		OutFunctionPart +=  "Vrai" + to_string(tag) + ":\t# if true skip else\n";;
	} else {
		OutStatementPart += "Vrai" + to_string(tag) + ":\t# if true skip else\n";
	}
	
	// WHILE ! ELSE || END
	#ifdef DEBUG
		cout << "# WHILE ! ELSE || END" << endl;
	#endif
	while(true){
		if(strcmp(lexer->YYText(),"ELSE" ) == 0 || strcmp(lexer->YYText(),"END" ) == 0){
			break;
		}
		#ifdef DEBUG
			cout << "\t\t# WORD == " << lexer->YYText() <<endl;
		#endif
		Statement();
		current=(TOKEN) lexer->yylex();
	}
	#ifdef DEBUG
		cout << "# PLZ BE A ELSE => " << lexer->YYText() << "Ligne n°" << lexer->lineno() << endl;
	#endif
	if( current == KEYWORD && strcmp(lexer->YYText(),"ELSE" ) == 0){
		#ifdef DEBUG
			std::cout << "# ELSE" << endl;
		#endif
		if(isFunc){
			OutFunctionPart.insert( tempPosition, "\tjmp Else"+ to_string(tag) +"\n");
			OutFunctionPart += "\tjmp Next"+ to_string(tag) +"\nElse"  + to_string(tag) + ":\n";
		} else {
		OutStatementPart.insert( tempPosition, "\tjmp Else"+ to_string(tag) +"\n");
		OutStatementPart += "\tjmp Next"+ to_string(tag) +"\nElse"  + to_string(tag) + ":\n";	
		}
		current=(TOKEN) lexer->yylex();
		BlockStatement();
		if(isFunc){
			OutFunctionPart += "Next" + to_string(tag) + ":\n";
		} else {
			OutStatementPart += "Next" + to_string(tag) + ":\n";
		}
		
	} else {
		Error("ELSE or END missing !");
	}
}

// VAR a,b....: TYPE;
void VarStatement(void){
	#ifdef DEBUG
		std::cout << "# ----------- VarStatement(void) -----------" << endl;
		std::cout << "# CURENT ==>" <<  lexer->YYText()<< endl;
	#endif
	if(current == KEYWORD && strcmp(lexer->YYText(), "VAR") == 0 ){
		current=(TOKEN) lexer->yylex();
	}

	set<string> TempVar;

	while (strcmp(lexer->YYText(), ":") != 0){
		if(strcmp(lexer->YYText(), ",") != 0){
			#ifdef DEBUG
				std::cout << "# NEW VAR ==>" <<  lexer->YYText()<< endl;
			#endif
			TempVar.insert(lexer->YYText());
		}
		current=(TOKEN) lexer->yylex();
	}
	
	if(strcmp(lexer->YYText(), ":") == 0){
		current=(TOKEN) lexer->yylex();
	}
	TYPE TypeOfVar = Type();
	#ifdef DEBUG
		std::cout << "# TYPE is ==>" << TypeOfVar << endl;
	#endif
	current=(TOKEN) lexer->yylex();

	for (set<string>::iterator it=TempVar.begin(); it!=TempVar.end(); ++it){
		switch(TypeOfVar){
				case BOOL:
				case INT:
					OutDeclarationPart += *it + ":\t.quad 0\n";
					break;
				case DOUBLE:
					OutDeclarationPart += *it + ":\t.double 0.0\n";
					break;
				case CHAR:
					OutDeclarationPart += *it + ":\t.byte 0\n";
					break;
				default:
					Error("Unrecognized type, plz read the doc !");
		}
		DeclaredVariables[*it]=TypeOfVar;
	}


	//Error("Unrecognized type");
}

// StatementPart := Statement {";" Statement} "."
void StatementPart(void){
	OutStatementPart += "\t.text\t\t# The following lines contain the program\n\
\t.globl main\t# The main function must be visible from outside\n\
main:\t\t\t# The main function body :\n\
\tmovq %rsp, %rbp\t# Save the position of the stack's top\n";
	Statement();
	while(current == SEMICOLON || current == KEYWORD || current == ID){
		if(current == SEMICOLON){
			current=(TOKEN) lexer->yylex();
		}
		Statement();
	}
	if(current != DOT)
		Error("caractère '.' attendu");
	current=(TOKEN) lexer->yylex();
}

// Program := [DeclarationPart] StatementPart
void Program(void){
	DeclarationPart();
	StatementPart();	
}

int main(int argc,char** argv){	// First version : Source code on standard input and assembly code on standard output
	// Header for gcc assembler / linker
	if (! __cplusplus >= 201402L ){
		Error("You must have C++14 or newer");
	}
	std::cout  	<<  "\t\t\t# This code was produced by the " << PROGNAME << endl;
	std::cout	<<	"\t\t\t# "; print_release();
	std::cout  	<<  "\t\t\t# This Compiler is a fork of CERI Compiler (framagit.org/jourlin/cericompiler)" << endl;
	// Let's proceed to the analysis and code production
	current = (TOKEN) lexer->yylex(); // save current token

	Program();
	std::cout << "# ----------------- DeclarationPart ----------------- #" << endl << OutDeclarationPart << 
	"# ----------------- FunctionPart ----------------- #" << endl << OutFunctionPart << 
	"# ----------------- StatementPart ----------------- #" << endl << OutStatementPart << endl;
	// Trailer for the gcc assembler / linker
	std::cout  <<  "\tmovq %rbp, %rsp\t\t# Restore the position of the stack's top" << endl;
	std::cout  <<  "\tmovl     $1, %eax" << endl;
	std::cout  <<  "\tmovl     $0, %ebx" << endl;
	std::cout  <<  "\tint     $0x80\t\t\t# Return from main function" << endl;
	if(current != FEOF){
		cerr  << "Caractères en trop à la fin du programme : [" << current << "]";
		Error("."); // unexpected characters at the end of program
	}
}
