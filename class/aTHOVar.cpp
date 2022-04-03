//--------------------------------------------------------------------------------------|
//----- Auteur :        Aubertin Emmanuel               |  For: Class for variable  ----|
//----- Description :   A class for variable            |                           ----|
//----- Contact :       https://athomisos.fr            |                           ----|
//--------------------------------------------------------------------------------------|

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
    } else {
        this->TypeOfVar = NONE;
    }
}

VarTYPE aTHOVar::getType()
{
    return this->TypeOfVar;
}
