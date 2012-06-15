#include "symbolicvalue.h"

#include <iostream>
#include <sstream>

std::string SymbolicValue :: to_str ()
{
    std::stringstream ss;
    
    ss << "(" << bits << " 0x" << std::hex << this->value << ")";
    
    return ss.str();
}
