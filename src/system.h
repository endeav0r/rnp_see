#ifndef system_HEADER
#define system_HEADER

#include "registers.h"
#include "symbolicvalue.h"
#include "staticvector.h"

class System {
    public :
        StaticVector <SymbolicValue> variables;
        Registers registers;
        
        SymbolicValue & g_variable   (size_t index);
        SymbolicValue & new_variable (SymbolicValue);
};

#endif
