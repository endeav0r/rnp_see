#include "symbolicvalue.h"

#include <iostream>
#include <sstream>

std::string SymbolicValue :: to_str ()
{
    std::stringstream ss;
    
    //std::cout << "[" << this->name << "]" << std::endl;
    
    ss << "("
       << this->name
       << " "
       << std::hex
       << this->value
       << ")";
    
    return ss.str();
}
