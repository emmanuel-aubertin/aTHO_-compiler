//--------------------------------------------------------------------------------------|
//----- Auteur :        Aubertin Emmanuel               |  For: Class for variable  ----|
//----- Description :   A class for variable            |                           ----|
//----- Contact :       https://athomisos.fr            |                           ----|
//--------------------------------------------------------------------------------------|


// J'ai décidé de faire une class car pour l'avenir le code dans compilateur.cpp pour etre optimiser en utilisant des methode par example.
#include <string>
#include <iostream>
#include <cstdlib>
#include <set>
#include <FlexLexer.h>
#include <cstring>
#include <fstream>
#include <algorithm>
#include "aTHOVar.h"

using namespace std;

aTHOVar::aTHOVar(const char* InputName, const char* InputType)
{
    this->name = InputName;
    if ( strcmp(InputType, "int") == 0)
    {
        this->TypeOfVar = INT;
    }
    else if ( strcmp(InputType, "str") == 0)
    {
        this->TypeOfVar = STRING;
    } 
    else if ( strcmp(InputType, "flt") == 0)
    {
        this->TypeOfVar = FLOAT;
    }
    else {
        this->TypeOfVar = NONE;
    }
}

VarTYPE aTHOVar::getType()
{
    return this->TypeOfVar;
}
