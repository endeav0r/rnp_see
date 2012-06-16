#include "symbolicvalue.h"

#include <iostream>
#include <sstream>

int64_t SymbolicValue :: g_svalue () const
{
    switch (bits) {
    case 8  : return (int8_t)  value;
    case 16 : return (int16_t) value;
    case 32 : return (int32_t) value;
    case 64 : return (int64_t) value;
    }
    return value;
}

std::string SymbolicValue :: str () const
{
    std::stringstream ss;

    if (wild) ss << "(" << bits << " wild)";
    else ss << "(" << bits << " 0x" << std::hex << this->value << ")";
    
    return ss.str();
}

#define SVSTR(OPER, CLASS) \
std::string CLASS :: str () const \
{                                 \
    return std::string("(") + lhs.str() + " ##OPER## " + rhs.str() + ")"; \
}

SVSTR(+,  SymbolicValueAdd)
SVSTR(&,  SymbolicValueAnd)
SVSTR(<,  SymbolicValueCmpLts)
SVSTR(==, SymbolicValueEq)
SVSTR(-,  SymbolicValueSub)
SVSTR(^,  SymbolicValueXor)

const SymbolicValue SymbolicValue :: signExtend () const
{
    return SymbolicValue(64, g_svalue());
}

#define SVOPERATOR(OPER, CLASS) \
const SymbolicValue SymbolicValue :: operator OPER (const SymbolicValue & rhs) const \
{                                                                                 \
    if ((not this->wild) && (not rhs.wild))                                       \
        return SymbolicValue(this->bits, this->value OPER rhs.value);             \
    else                                                                          \
        return CLASS(*this, rhs);                                                 \
}

SVOPERATOR(+, SymbolicValueAdd)
SVOPERATOR(&, SymbolicValueAnd)
SVOPERATOR(-, SymbolicValueSub)
SVOPERATOR(^, SymbolicValueXor)


#define SVCMP(OPER, CLASS) \
const SymbolicValue SymbolicValue :: operator OPER (const SymbolicValue & rhs) const \
{                                                                                    \
    if ((not this->wild) && (not rhs.wild)) {                                        \
        if (this->value OPER rhs.value) return SymbolicValue(1, 1);                  \
        else return SymbolicValue(1, 0);                                             \
    }                                                                                \
    else                                                                             \
        return CLASS(*this, rhs);                                                    \
}

SVCMP(==, SymbolicValueEq)



const SymbolicValue SymbolicValue :: cmpLts (const SymbolicValue & rhs) const
{
    if ((not this->wild) && (not rhs.wild)) {
        if (g_svalue() < rhs.g_svalue()) return SymbolicValue(1, 1);
        else return SymbolicValue(1, 0);
    }
    else
        return SymbolicValueCmpLts(*this, rhs);
}