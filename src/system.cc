#include "system.h"


SymbolicValue & System :: g_variable (size_t index)
{
    return this->variables.get(index);
}

SymbolicValue & System :: new_variable (SymbolicValue symval)
{
    this->variables.push_back(symval);
    return this->variables.back();
}
