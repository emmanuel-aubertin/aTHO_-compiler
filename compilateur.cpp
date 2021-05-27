//  A compiler from a very simple Pascal-like structured language LL(k)
//  to 64-bit 80x86 Assembly langage
//  Copyright (C) 2019 Pierre Jourlin
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

// Build with "make compilateur"


#include <string>
#include <iostream>
#include <cstdlib>
#include <set>
#include <FlexLexer.h>
#include "tokeniser.h"
#include <cstring>

using namespace std;


#define DEBUG // if define the asm wil be have a lot of useless com

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
TYPE Expression(void);
//VARTYPE Number(void);

TOKEN current;				// Current token


FlexLexer* lexer = new yyFlexLexer; // This is the flex tokeniser
// tokens can be read using lexer->yylex()
// lexer->yylex() returns the type of the lexicon entry (see enum TOKEN in tokeniser.h)
// and lexer->YYText() returns the lexicon entry as a string

	
set<string> DeclaredVariables;
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
	cout  <<  "\tpush " << lexer->YYText() << endl;
	current=(TOKEN) lexer->yylex();
	return INTEGER;
}

TYPE Number(void){
	cout  << "\tpush $" << atoi(lexer->YYText()) << endl;
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
		cout  <<  "\tpop %rbx" << endl;	// get first operand
		cout  <<  "\tpop %rax" << endl;	// get second operand
		switch(mulop){
			case AND:
				cout  <<  "\tmulq	%rbx" << endl;	// a * b -> %rdx:%rax
				cout  <<  "\tpush %rax\t# AND" << endl;	// store result
				break;
			case MUL:
				cout  <<  "\tmulq	%rbx" << endl;	// a * b -> %rdx:%rax
				cout  <<  "\tpush %rax\t# MUL" << endl;	// store result
				break;
			case DIV:
				cout  <<  "\tmovq $0, %rdx" << endl; 	// Higher part of numerator  
				cout  <<  "\tdiv %rbx" << endl;			// quotient goes to %rax
				cout  <<  "\tpush %rax\t# DIV" << endl;		// store result
				break;
			case MOD:
				cout  <<  "\tmovq $0, %rdx" << endl; 	// Higher part of numerator  
				cout  <<  "\tdiv %rbx" << endl;			// remainder goes to %rdx
				cout  <<  "\tpush %rdx\t# MOD" << endl;		// store result
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
		cout << endl << "# IN void SimpleExpression(void)" << endl;
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
		cout  <<  "\tpop %rbx" << endl;	// get first operand
		cout  <<  "\tpop %rax" << endl;	// get second operand
		switch(adop){
			case OR:
				cout  <<  "\taddq	%rbx, %rax\t# OR" << endl;// operand1 OR operand2
				break;			
			case ADD:
				cout  <<  "\taddq	%rbx, %rax\t# ADD" << endl;	// add both operands
				break;			
			case SUB:	
				cout  <<  "\tsubq	%rbx, %rax\t# SUB" << endl;	// substract both operands
				break;
			default:
				Error("opérateur additif inconnu");
		}
		cout  <<  "\tpush %rax" << endl;			// store result
	}
	return firstTerm;
}

// DeclarationPart := "[" Ident {"," Ident} "]"
void DeclarationPart(void){
	cout  <<  "\t.data" << endl;
	cout << "FormatString:    .string \"%c\"" << endl;
	cout << "FormatString1:    .string \"%llu\\n\"" << endl;
	if(current == RBRACKET){
		#ifdef DEBUG
			cout << "# IN [" << endl;
		#endif
		cout  <<  "\t.align 8" << endl;
		
		current=(TOKEN) lexer->yylex();
		if(current != ID)
			Error("Un identificater était attendu");
		cout  <<  lexer->YYText()  <<  ":\t.quad 0" << endl;
		DeclaredVariables.insert(lexer->YYText());
		current=(TOKEN) lexer->yylex();
		while(current == COMMA){
			current=(TOKEN) lexer->yylex();
			if(current != ID)
				Error("Un identificateur était attendu");
			cout  <<  lexer->YYText()  <<  ":\t.quad 0" << endl;
			DeclaredVariables.insert(lexer->YYText());
			current=(TOKEN) lexer->yylex();
		}
		current=(TOKEN) lexer->yylex();
	}
	
}

// RelationalOperator := " == " | " != " | "<" | ">" | "<=" | ">="  
OPREL RelationalOperator(void){
	OPREL oprel;
	if(strcmp(lexer->YYText()," = ") == 0)
		oprel=EQU;
	else if(strcmp(lexer->YYText()," != ") == 0)
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
		cout << endl << "# IN void Expression(void)" << endl;
	#endif
	OPREL oprel;
	firstPart = SimpleExpression();
	if(current == RELOP){
		oprel=RelationalOperator();
		secondPart = SimpleExpression();
		if(firstPart != secondPart){ // If not same type
			Error("Type non compatible !");
		}
		cout  <<  "\tpop %rax" << endl;
		cout  <<  "\tpop %rbx" << endl;
		cout  <<  "\tcmpq %rax, %rbx" << endl;
		switch(oprel){
			case EQU:
				cout  <<  "\tje Vrai" << ++TagNumber << "\t# If equal" << endl;
				break;
			case DIFF:
				cout  <<  "\tjne Vrai" << ++TagNumber << "\t# If different" << endl;
				break;
			case SUPE:
				cout  <<  "\tjae Vrai" << ++TagNumber << "\t# If above or equal" << endl;
				break;
			case INFE:
				cout  <<  "\tjbe Vrai" << ++TagNumber << "\t# If below or equal" << endl;
				break;
			case INF:
				cout  <<  "\tjb Vrai" << ++TagNumber << "\t# If below" << endl;
				break;
			case SUP:
				cout  <<  "\tja Vrai" << ++TagNumber << "\t# If above" << endl;
				break;
			default:
				Error("Opérateur de comparaison inconnu");
		}
		cout  <<  "\tpush $0\t\t# False" << endl;
		cout  <<  "\tjmp Suite" << TagNumber << endl;
		cout  <<  "Vrai" << TagNumber << ":\tpush $0xFFFFFFFFFFFFFFFF\t\t# True" << endl;	
		cout  <<  "Suite" << TagNumber << ":" << endl;
	}
	return BOOLEAN;
}

// AssignementStatement := Identifier ":=" Expression
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
	cout  <<  "\tpop " << variable << endl;
}

// Statement := AssignementStatement
void Statement(void){
	if(strcmp(lexer->YYText(),"" ) != 0){

		#ifdef DEBUG
			cout << "# ----------- In Statement(void) ----------- " << endl;
		#endif

		if((TOKEN) current == KEYWORD){
			#ifdef DEBUG
				cout << "# TOKEN == KEYWORD"<< endl;
			#endif

			if( strcmp(lexer->YYText(),"IF" ) == 0){
				#ifdef DEBUG
					cout << "# IF STATEMENT" << endl ;
				#endif
				IfStatement();
			} else if( strcmp(lexer->YYText(),"THEN") == 0 || strcmp(lexer->YYText(),"ELSE") == 0){ // If we have then or else outside of an if do ..
				#ifdef DEBUG
					cout << "# THEN or ELSE outside of IF" << endl ;
				#endif
				Error("You need to be in a IF");
			} else if( strcmp(lexer->YYText(),"WHILE" ) == 0){
				#ifdef DEBUG
					cout << "# WHILE STATEMENT" << endl ;
				#endif
				WhileStatement();
			} else if( strcmp(lexer->YYText(),"FOR" ) == 0){
				#ifdef DEBUG
					cout << "# FOR STATEMENT" << endl ;
				#endif
				ForStatement();
			} else if( strcmp(lexer->YYText(),"BEGIN" ) == 0){
				#ifdef DEBUG
					cout << "# BLOCK STATEMENT" << endl ;
				#endif
				BlockStatement(); 
			} else if( strcmp(lexer->YYText(),"DO" ) == 0){
				#ifdef DEBUG
					cout << "# BLOCK STATEMENT" << endl ;
				#endif
				BlockStatement(); 
			} else if( strcmp(lexer->YYText(),"END" ) == 0){
				#ifdef DEBUG
					cout << "# END STATEMENT" << endl ;
				#endif
				Statement();
			}else if( strcmp(lexer->YYText(),"DISPLAY" ) == 0){
				#ifdef DEBUG
					cout << "# DISPLAY STATEMENT" << endl ;
				#endif
				DisplayStatement();
			}else if( strcmp(lexer->YYText(),"EXIT" ) == 0){
				#ifdef DEBUG
					cout << "# EXIT STATEMENT" << endl ;
				#endif
				cout << "\tmovl $1, %eax  # System call number 1: exit()" << endl;
    			cout << "\tmovl $0, %ebx  # Exits with exit status 0 " << endl;
				cout << "\tint $0x80\t# Interupte prog" << endl;
				Statement();
			} else {
			Error("Not code yet");
			}
		} else {
			if( current == ID ){
				#ifdef DEBUG
					cout << "# # TOKEN == ID" << endl;
				#endif
				AssignementStatement();
			} else {
				Error("Instruction unreachable !");
			}
		}
	}
}


// DISPLAY <Expression>
void DisplayStatement(void){
	unsigned long long tag=TagNumber++;
	#ifdef DEBUG
		cout << "# ----------- In DisplayStatement(void) -----------" << endl;
	#endif
	current = (TOKEN) lexer->yylex();

	if(current == STRINGCONST){
		//cout << "\t.string " << lexer->YYText() << endl;
		//cout << "\tmov .string " << lexer->YYText() <<", FormatString                     # The value to be displayed" << endl;
		string currentWord = lexer->YYText();
		#ifdef DEBUG
			cout << "# WORD ==> " << currentWord << endl;
		#endif
		currentWord.erase(0, 1); // erase the first "
		currentWord.erase(currentWord.size() - 1 ); // erase the " of the end
		cout << "# PRINT STRING : " << currentWord << endl;
		for(char& c : currentWord) {		
			cout << "\tmovb $'" << c << "', %al"<<endl;
			cout << "\tpush %rax"<<endl;
			cout << "\tpop %rsi\t"<<endl;
			cout << "\tmovq $FormatString, %rdi\t# \"%c\\n\""<<endl;
			cout << "\tmovl	$0, %eax"<<endl;
			cout << "\tcall	printf@PLT"<<endl;
		}

		// cout << "\t.print " << lexer->YYText() << endl; // print de "compilation"
		current = (TOKEN) lexer->yylex();
	} else { // can be only a int for now
		#ifdef DEBUG
			cout << "# OTHER" << endl;
		#endif
		Expression();
		cout << "\tpop %rdx                     # The value to be displayed" << endl;
		cout << "\tmovq $FormatString1, %rsi    # \"%llu\\n\"" << endl;
		cout << "\tmovl    $1, %edi" << endl;
		cout << "\tmovl    $0, %eax" << endl;
		cout << "\tcall    __printf_chk@PLT" << endl;
		 // cout << "\tint $0x80" << endl; // usless avec print f
	}
}


// WHILE <Expression> DO <Statement> 	
void WhileStatement(void){
	TYPE expType;
	#ifdef DEBUG
		cout << "# ----------- WhileStatement(void) -----------" << endl;
	#endif
	unsigned long long tag=TagNumber++;
	current = (TOKEN) lexer->yylex();
	cout << "While" << tag << ":" << endl;

	if(current == RPARENT && strcmp(lexer->YYText(),"(")  ==  0){
		expType = Expression();
		if(expType != BOOLEAN){
			Error("Boolean expression needed !");
		}
	} else {
		Error("Expression needed ! `WHILE <Expression> DO <Statement>.`");
	}

	cout << "\tpop %rax" << endl;
	cout << "\tcmp $0, %rax" << endl;
	cout << "\tje EndWhile" << tag <<endl;
	if(current == KEYWORD && strcmp(lexer->YYText(),"DO")  ==  0){
		current = (TOKEN) lexer->yylex();
		BlockStatement();
	} else {
		Error("DO missing !");
	}
	cout << "jmp While" << tag << endl;
	cout << "EndWhile" << tag << ":" << endl;
}


// FOR <AssignementStatement> To <Expression> DO <Statement>
void ForStatement(void){
	#ifdef DEBUG
		cout << "# ----------- ForStatement(void) -----------" << endl;
	#endif
	unsigned long long tag=TagNumber++;
	current = (TOKEN) lexer->yylex();

	#ifdef DEBUG
		cout << "# WORD = " << lexer->YYText() << endl;
	#endif
	
	if( current == NUMBER){
		cout << "movq $" << lexer->YYText() << ", %rcx # Get Start value" << endl;
		current = (TOKEN) lexer->yylex();
	} else {
		Error("Need Digit");
	}

	cout << "For" << tag << ":" << endl;
	cout << "\taddq	$1, %rcx \t# OR" << endl;

	if( current == KEYWORD || strcmp(lexer->YYText(), "To") == 0){
		current = (TOKEN) lexer->yylex(); // Get digit after To
		cout << "\tcmp $" << lexer->YYText() << ", %rcx # To KEYWORD" << endl;
		cout << "\tjae EndFor"<< tag << endl;
		current = (TOKEN) lexer->yylex(); // Get Do
	} else {
		Error("To requiered");
	}

	if( current == KEYWORD || strcmp(lexer->YYText(), "DO") == 0 ){
		current = (TOKEN) lexer->yylex(); // Get Expression
		BlockStatement();
	} else {
		Error("DO requiered");
	}
	cout << "\tjmp For" << tag << endl;
	cout << "EndFor" << tag << ":" << endl;
}

// "BEGIN" Statement { ";" Statement } "END"
void BlockStatement(void){
	#ifdef DEBUG
		cout << "# ----------- BlockStatement(void) -----------" << endl;
	#endif
	// unsigned long long tag=TagNumber++; Use less here
	if( current == KEYWORD){
		Statement();
	}
	while ( current != KEYWORD )
	{
		#ifdef DEBUG
			cout << "# CURRENT ==> " << lexer->YYText() << endl;
		#endif
		Statement();
		if(strcmp(lexer->YYText(), ";") == 0){
			#ifdef DEBUG
				cout << "# CURRENT ==> " << lexer->YYText() << endl;
			#endif
			current = (TOKEN) lexer->yylex();// skip ;
		} else {
			Error(";");
		}
	}
	#ifdef DEBUG
		cout << "# after while ==> " << lexer->YYText() << endl;
	#endif
	//current = (TOKEN) lexer->yylex();
	if( current == KEYWORD && strcmp(lexer->YYText(), "END") == 0){
		current = (TOKEN) lexer->yylex();
		if(strcmp(lexer->YYText(), ";") == 0){
			current = (TOKEN) lexer->yylex(); // skip ;
		}
	} else {
		Error("END Missing");
	}
	#ifdef DEBUG
		cout << "# BEFORE END  " << lexer->YYText() << endl;
	#endif
	//Statement();
}

// "IF" Expression "THEN" Action ["ELSE" Action (or can be a another if)]
void IfStatement(void){
	TYPE expType;
	unsigned long long tag=TagNumber++; // For unique name in asm program
	//current = (TOKEN) lexer->yylex(); // Get and cast next word
	if(current != KEYWORD || strcmp(lexer->YYText(),"IF")  !=  0){
		Error("In void IfStatement(void) without if");
	}else{
		current = (TOKEN) lexer->yylex();
	}
	if(current == RPARENT && strcmp(lexer->YYText(),"(")  ==  0){
		expType = Expression();
		if(expType != BOOLEAN){
			Error("Boolean expression needed !");
		}
	} else {
		Error("Une exp était attendu");
	}
	cout << "\tpop %rax\t# Get the result of expression" << endl;
	//cout << "\tcmpq $0, %rax\t# Compare " << endl;
	cout << "\tje Else" << tag << "\t# jmp à Else" << tag << " if la comparaison est fausse" << endl;
	
	#ifdef DEBUG
		cout << "# WORD ==> " << lexer->YYText() << endl;
	#endif

	if(  current != KEYWORD || strcmp(lexer->YYText(),"THEN")  !=  0 ) { // Mémo en pascal on écrit IF Machin THEN JeFaisMachin
		Error(" Un 'THEN' attendu, en pascal on écrit IF Machin THEN JeFaisMachin");
	}
	current=(TOKEN) lexer->yylex();
	Statement();

	cout << "\tjmp Next" << tag << "\t# Si le if est fais on skip le else" << endl;
	cout << "Else" << tag << ":" << endl;
	current=(TOKEN) lexer->yylex();
	if( current == KEYWORD || strcmp(lexer->YYText(),"ELSE" ) == 0){
		current=(TOKEN) lexer->yylex();
		Statement();
	}
	cout << "Next" << tag << ":" << endl;
	
}

// StatementPart := Statement {";" Statement} "."
void StatementPart(void){
	cout  <<  "\t.text\t\t# The following lines contain the program" << endl;
	cout  <<  "\t.globl main\t# The main function must be visible from outside" << endl;
	cout  <<  "main:\t\t\t# The main function body :" << endl;
	cout  <<  "\tmovq %rsp, %rbp\t# Save the position of the stack's top" << endl;
	Statement();
	while(current == SEMICOLON){
		current=(TOKEN) lexer->yylex();
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

int main(void){	// First version : Source code on standard input and assembly code on standard output
	// Header for gcc assembler / linker
	cout  <<  "\t\t\t# This code was produced by the aTHO Compiler" << endl;
	cout  <<  "\t\t\t# This Compiler is a fork of CERI Compiler (framagit.org/jourlin/cericompiler)" << endl;
	// Let's proceed to the analysis and code production
	current=(TOKEN) lexer->yylex();
	Program();
	// Trailer for the gcc assembler / linker
	cout  <<  "\tmovq %rbp, %rsp\t\t# Restore the position of the stack's top" << endl;
	cout  <<  "\tret\t\t\t# Return from main function" << endl;
	if(current != FEOF){
		cerr  << "Caractères en trop à la fin du programme : [" << current << "]";
		Error("."); // unexpected characters at the end of program
	}
}