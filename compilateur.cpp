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
#include "class/aTHOVar.h";


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
enum TYPE {INTEGER, BOOLEAN};



// Prototypage :
void IfStatement(void);
void WhileStatement(void);
void ForStatement(void);
void BlockStatement(void);
void DisplayStatement(void);
void VarStatement(void);
TYPE Expression(void);
int getLength(string);
//VARTYPE Number(void);

int strcounter = 0;
size_t tempPosition; // !! Need to be reset after all use
string tempBlock;
string OutDeclarationPart;
string OutStatementPart;


TOKEN current;				// Current token

FlexLexer* lexer = new yyFlexLexer; // This is the flex tokeniser
// tokens can be read using lexer->yylex()
// lexer->yylex() returns the type of the lexicon entry (see enum TOKEN in tokeniser.h)
// and lexer->YYText() returns the lexicon entry as a string

	
set<string> DeclaredVariables;
map<string,const aTHOVar*&> MapVar;

unsigned long TagNumber=0;

bool IsDeclared(const char *id){
	return DeclaredVariables.find(id) != DeclaredVariables.end();
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
	OutStatementPart += "\tpush ";
	OutStatementPart += lexer->YYText();
	OutStatementPart += "\n";
	current=(TOKEN) lexer->yylex();
	return INTEGER;
}

TYPE Number(void){
	OutStatementPart +=  "\tpush $" + to_string(atoi(lexer->YYText())) + "\n";
	current=(TOKEN) lexer->yylex();
	return INTEGER;
}

TYPE Factor(void){
	TYPE typeExp;
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
		OutStatementPart +=  "\tpop %rbx\n\
\tpop %rax\n";	// get second operand
		switch(mulop){
			case AND:
				OutStatementPart +=  "\tmulq	%rbx\n\
\tpush %rax\t# AND\n";	// store result
				break;
			case MUL:
				OutStatementPart +=  "\tmulq	%rbx\n\
\tpush %rax\t# MUL\n";	// store result
				break;
			case DIV:
				OutStatementPart +=  "\tmovq $0, %rdx\n\
\tdiv %rbx\n\
\tpush %rax\t# DIV\n";		// store result
				break;
			case MOD:
				OutStatementPart +=  "\tmovq $0, %rdx\n\
\tdiv %rbx\n\
\tpush %rdx\t# MOD\n";		// store result
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
		OutStatementPart +=  "\tpop %rbx\n\
\tpop %rax\n";	// get second operand
		switch(adop){
			case OR:
				OutStatementPart += "\taddq	%rbx, %rax\t# OR\n";// operand1 OR operand2
				break;			
			case ADD:
				OutStatementPart += "\taddq	%rbx, %rax\t# ADD\n";	// add both operands
				break;			
			case SUB:	
				OutStatementPart +="\tsubq	%rbx, %rax\t# SUB\n";	// substract both operands
				break;
			default:
				Error("opérateur additif inconnu");
		}
		OutStatementPart += "\tpush %rax\n";			// store result
	}
	return firstTerm;
}

// DeclarationPart := "[" Ident {"," Ident} "]"
void DeclarationPart(void){
	OutDeclarationPart +=  "\t.data\n\
FormatString:    .string \"%c\"\n\
FormatString1:    .string \"%llu\\n\"\n";
	if(current == RBRACKET){
		#ifdef DEBUG
			std::cout << "# IN [" << endl;
		#endif
		OutDeclarationPart += "\t.align 8\n";
		
		current=(TOKEN) lexer->yylex();
		if(current != ID)
			Error("Un identificater était attendu");
		OutDeclarationPart +=   lexer->YYText();
		OutDeclarationPart +=   ":\t.quad 0\n";
		DeclaredVariables.insert(lexer->YYText());
		const aTHOVar * MaVar = new aTHOVar(lexer->YYText(), "none");
		string VarName = (string)lexer->YYText();
		MapVar.emplace(VarName, MaVar); // Same as insert
		// add in map for type
		current=(TOKEN) lexer->yylex();
		while(current == COMMA){
			current=(TOKEN) lexer->yylex();
			if(current != ID)
				Error("Un identificateur était attendu");
			OutDeclarationPart +=   lexer->YYText();
			OutDeclarationPart +=   ":\t.quad 0\n";

			DeclaredVariables.insert(lexer->YYText());
			const aTHOVar * MaVar = new aTHOVar(lexer->YYText(), "none");
			string VarName = (string)lexer->YYText();
			MapVar.emplace(VarName, MaVar); // Same as insert
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
		OutStatementPart += "\tpop %rax\n\
\tpop %rbx\n\
\tcmpq %rax, %rbx\n";

		switch(oprel){
			case EQU:
				#ifdef DEBUG
					cout << "\t\t# EQU expression" << endl;
				#endif
				OutStatementPart += "\tje Vrai" + to_string(TagNumber) + "\t# If equal\n";
				break;
			case DIFF:
				#ifdef DEBUG
					cout << "\t\t# DIFF expression" << endl;
				#endif
				OutStatementPart += "\tjne Vrai" + to_string(TagNumber) + "\t# If different\n";
				break;
			case SUPE:
				#ifdef DEBUG
					cout << "\t\t# SUPE expression" << endl;
				#endif
				OutStatementPart += "\tjae Vrai" + to_string(TagNumber) + "\t# If above or equal\n";
				break;
			case INFE:
				#ifdef DEBUG
					cout << "\t\t# INFE expression" << endl;
				#endif
				OutStatementPart += "\tjbe Vrai" + to_string(TagNumber) + "\t# If below or equal\n";
				break;
			case INF:
				#ifdef DEBUG
					cout << "\t\t# INF expression" << endl;
				#endif
				OutStatementPart += "\tjb Vrai" + to_string(TagNumber) + "\t# If below\n";
				break;
			case SUP:
				#ifdef DEBUG
					cout << "\t\t# SUP expression" << endl;
				#endif
				OutStatementPart += "\tja Vrai" + to_string(TagNumber) + "\t# If above\n";
				break;
			default:
				Error("Opérateur de comparaison inconnu");
		}
		tempPosition = OutStatementPart.length();
		
		//OutStatementPart += "REPLACE"+ to_string(++TagNumber);
	}
	#ifdef DEBUG
		cout << "# End of Expression()" << endl;
	#endif
	return BOOLEAN;

}

// AssignementStatement := Identifier ":=" Expression
// Non type variable
void AssignementStatement(void){
	string variable;
	if(current != ID)
		Error("Identificateur attendu");
	if(!IsDeclared(lexer->YYText())){
		cerr  <<  "Erreur : Variable '" << lexer->YYText() << "' non déclarée" << endl;
		exit(-1);
	}
	variable=lexer->YYText();
	current=(TOKEN) lexer->yylex();
	if(current != ASSIGN)
		Error("caractères ':=' attendus");
	current=(TOKEN) lexer->yylex();
	Expression();
	OutStatementPart +=  "\tpop " + variable +"\n";
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
			
			if( strcmp(lexer->YYText(),"EXIT" ) == 0){
				#ifdef DEBUG
					std::cout << "# EXIT STATEMENT" << endl ;
				#endif
				OutStatementPart += "\tmovl $1, %eax  # System call number 1: exit()\n\tmovl $0, %ebx  # Exits with exit status 0\n\tint $0x80\t# Interupte prog\n";
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

// DISPLAY <Expression>
void DisplayStatement(void){
	unsigned long long tag=++TagNumber;
	#ifdef DEBUGDISP
		std::cout << "# ----------- In DisplayStatement(void) -----------" << endl;
	#endif

	OutStatementPart += "\n# DISPLAY ";
	current = (TOKEN) lexer->yylex();

	if(current == STRINGCONST){
		//std::cout << "\t.string " << lexer->YYText() << endl;
		//std::cout << "\tmov .string " << lexer->YYText() <<", FormatString                     # The value to be displayed" << endl;
		string currentWord = lexer->YYText();
		OutStatementPart += currentWord + "\n";
		

		currentWord.erase(0, 1); // erase the first "
		currentWord.erase(currentWord.size() - 1 ); // erase the " of the end
		// Need to do a length func
		int len = getLength(currentWord); // Not good, because of \n \t ect..

		OutDeclarationPart += "STR" + to_string(TagNumber) + ":";
		OutDeclarationPart += "\t.string \"" + currentWord + "\"\n";
		/*OutDeclarationPart += "len" + to_string(TagNumber) + ":\n\tpush $" + to_string(len) + ", len";*/

		OutStatementPart += "\tmovl $4, %eax # sys_write\n\
\tmovl $1, %ebx # write to file descriptor 1 (stdout)\n\
\tmovl $STR" + to_string(TagNumber) + ", %ecx # pointer to the string to print\n\
\tmovl $" + to_string(len) + ", %edx # asks to print " + to_string(len) + " characters our of the string passed in %ecx\n\
\tint $0x80 # call the system call\n";

		// std::cout << "\t.print " << lexer->YYText() << endl; // print de "compilation"
		current = (TOKEN) lexer->yylex();
	} else { // can be only a int for nowS
		#ifdef DEBUGDISP
			std::cout << "# OTHER" << endl;
		#endif
		OutStatementPart += lexer->YYText();
		OutStatementPart += "\n";
		Expression();
		OutStatementPart += "\tpop %rdx                     # The value to be displayed\n\
\tmovq $FormatString1, %rsi    # \"%llu\\n\" \n\
\tmovl    $1, %edi \n\
\tmovl    $0, %eax \n\
\tcall    __printf_chk@PLT\n";
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
	OutStatementPart += "while" + to_string(tag) + ":\n";

	if(current == RPARENT && strcmp(lexer->YYText(),"(")  ==  0){
		expType = Expression();
		if(expType != BOOLEAN){
			Error("Boolean expression needed !");
		}
	} else {
		Error("Expression needed ! `WHILE <Expression> DO <Statement>.`");
	}

	OutStatementPart +=  "\tjmp EndWhile" + to_string(tag) + "\nVrai" + to_string(tag)+":\n";
	if(current == KEYWORD && strcmp(lexer->YYText(),"DO")  ==  0){
		current = (TOKEN) lexer->yylex();
		BlockStatement();
	} else {
		Error("DO missing !");
	}
	OutStatementPart += "jmp while" + to_string(tag) + "\n\
EndWhile" + to_string(tag) + ":\n";
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
		OutStatementPart += "movq $" ;
		OutStatementPart +=  lexer->YYText(); 
		OutStatementPart += ", %rcx # Get Start value\n";
		current = (TOKEN) lexer->yylex();
	} else {
		Error("Need Digit");
	}

	OutDeclarationPart += "i" + to_string(tag) + ": .quad 0\n";
	OutStatementPart +="For" + to_string(tag) + ":\n";

	if( current == KEYWORD || strcmp(lexer->YYText(), "To") == 0){
		current = (TOKEN) lexer->yylex(); // Get digit after To
		OutStatementPart +=  "\tcmp $";
		OutStatementPart +=  lexer->YYText();
		OutStatementPart += ", i" + to_string(tag) + " # To KEYWORD\n\
\tjae EndFor"+ to_string(tag) + "\n";
		current = (TOKEN) lexer->yylex(); // Get Do
	} else {
		Error("To requiered");
	}

	if( current == KEYWORD || strcmp(lexer->YYText(), "DO") == 0 ){
		OutStatementPart += "push i" + to_string(tag) + "\n\
\tpush $1\n\
\tpop %rbx\n\
\tpop %rax\n\
\taddq	%rbx, %rax	# ADD\n\
\tpush %rax\n\
\tpop i" + to_string(tag) + "\n\n";
		current = (TOKEN) lexer->yylex(); // Get Expression
		BlockStatement();
		#ifdef DEBUG
			std::cout << "# After BlockStatement();" << lexer->YYText() << endl;
			OutStatementPart += "\n# afterBlockStatement();\n";
		#endif
	} else {
		Error("DO requiered");
	}
	OutStatementPart += "\n\tjmp For" + to_string(tag) + "\n\
EndFor" + to_string(tag) + ":\n\
\tpush $0\n\
\tpop i" + to_string(tag);

	#ifdef DEBUG
		std::cout << "# End of for" << lexer->YYText() << endl;
		OutStatementPart += "\n# End of for\n\n\n";
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
			OutStatementPart += "\t\t # WORD ==> ";
			OutStatementPart += lexer->YYText();
			OutStatementPart += "\n";
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
		}/*
	} else {
		Error("END Missing");
	}*/
	#ifdef DEBUG
		std::cout << "# End of BlockStatement" << lexer->YYText() << endl;
		OutStatementPart += "\n# End of BlockStatement\n\n\n";
	#endif
}

// "IF" Expression "THEN" Action ["ELSE" Action (or can be a another if)]
void IfStatement(void){
	#ifdef DEBUG
		std::cout << "# ----------- IFStatement(void) -----------" << endl;
	#endif
	TYPE expType;
	unsigned long long tag=++TagNumber; // For unique name in asm program
	//OutStatementPart += "TAG ===> " + to_string(tag) + "\n TagNumber ===> " + to_string(TagNumber) + "\n";
	//current = (TOKEN) lexer->yylex(); // Get and cast next word
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
	if(expType != BOOLEAN){
		Error("Boolean expression needed !");
	}
	//OutStatementPart += "\tpop %rax\t# Get the result of expression\n";
	//std::cout << "\tcmpq $0, %rax\t# Compare " << endl;
	//OutStatementPart += "\tje VRAI" + to_string(tag) + "\t# jmp à VRAI" + to_string(tag) + " if false\n\tjmp Else10";
	
	#ifdef DEBUG
		std::cout << "# WORD ==> " << lexer->YYText() << endl;
	#endif

	if(  current != KEYWORD || strcmp(lexer->YYText(),"THEN")  !=  0 ) { // Mémo en pascal on écrit IF Machin THEN JeFaisMachin
		Error(" Un 'THEN' attendu, en pascal on écrit IF Machin THEN JeFaisMachin");
	}
	current=(TOKEN) lexer->yylex();
	OutStatementPart += "Vrai" + to_string(tag) + ":\t# if true skip else\n";
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
		OutStatementPart.insert( tempPosition, "\tjmp Else"+ to_string(tag) +"\n");
		OutStatementPart += "\tjmp Next"+ to_string(tag) +"\nElse"  + to_string(tag) + ":\n";
		current=(TOKEN) lexer->yylex();
		BlockStatement();
		OutStatementPart += "Next" + to_string(tag) + ":\n";
	} else {
		//Error("ELSE or END missing !");
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

	list<string> TempVar;

	while (strcmp(lexer->YYText(), ":") != 0){
		if(strcmp(lexer->YYText(), ",") != 0){
			#ifdef DEBUG
				std::cout << "# NEW VAR ==>" <<  lexer->YYText()<< endl;
			#endif
			TempVar.push_front(lexer->YYText());
		}
		current=(TOKEN) lexer->yylex();
	}
	
	if(strcmp(lexer->YYText(), ":") == 0){
		current=(TOKEN) lexer->yylex();
	}
	string TypeOfVar = lexer->YYText();
	#ifdef DEBUG
		std::cout << "# TYPE is ==>" << TypeOfVar << endl;
	#endif
	current=(TOKEN) lexer->yylex();


	if(TypeOfVar == "INT"){
		for(const string & var : TempVar){
			OutDeclarationPart += var + ":\t.quad 0\n";
			DeclaredVariables.insert(var);
		}
		return;
	}
	if(TypeOfVar == "STR"){
		for(const string & var : TempVar){
			OutDeclarationPart += var + ":\t.string \"\"\n";
			DeclaredVariables.insert(var);
		}
		return;
	}
	if(TypeOfVar == "FLOAT"){
		for(const string & var : TempVar){
			OutDeclarationPart += var + ":\t.double 0.0\n";
			DeclaredVariables.insert(var);
		}
		return;
	}

	Error("Unrecognized type");
}

// StatementPart := Statement {";" Statement} "."
void StatementPart(void){
	OutStatementPart += "\t.text\t\t# The following lines contain the program\n\
\t.globl main\t# The main function must be visible from outside\n\
main:\t\t\t# The main function body :\n\
\tmovq %rsp, %rbp\t# Save the position of the stack's top\n";
	Statement();
	while(current == SEMICOLON || current == KEYWORD){
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
