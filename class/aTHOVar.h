#include <iostream>
#include <string>

using namespace std;


enum VarTYPE {INT, STRING, NONE};

class aTHOVar 
{
    private:
        VarTYPE TypeOfVar;

    public:
        aTHOVar(const char* InputName, const char* InputType);
        string name;
        VarTYPE getType();
};