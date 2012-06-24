#include "symbolicvalue.h"

#include <iostream>
#include <sstream>

int64_t SymbolicValue :: g_svalue () const
{
    switch (bits) {
    case 1  : return (int8_t)  value & 0x1;
    case 8  : return (int8_t)  value;
    case 16 : return (int16_t) value;
    case 32 : return (int32_t) value;
    case 64 : return (int64_t) value;
    }
    return value;
}

uint64_t SymbolicValue :: g_value () const
{
    switch (bits) {
    case 1  : return (uint8_t)  value & 0x1;
    case 8  : return (uint8_t)  value;
    case 16 : return (uint16_t) value;
    case 32 : return (uint32_t) value;
    case 64 : return (uint64_t) value;
    }
    return value;
}

const std::string SymbolicValue :: str () const
{
    std::stringstream ss;

    if (wild) ss << "(" << bits << " wild)";
    else ss << "(" << bits << " 0x" << std::hex << this->g_value() << ")";
    
    return ss.str();
}

const std::string SymbolicValueNot :: str () const
{
    return std::string("~(") + src.str() + ")";
}

#define SVSTR(OPER, CLASS) \
const std::string CLASS :: str () const \
{                                 \
    return std::string("(") + lhs.str() + " ##OPER## " + rhs.str() + ")"; \
}

SVSTR(+,   SymbolicValueAdd)
SVSTR(&,   SymbolicValueAnd)
SVSTR(<=s, SymbolicValueCmpLes)
SVSTR(<=u, SymbolicValueCmpLeu)
SVSTR(<s,  SymbolicValueCmpLts)
SVSTR(<u,  SymbolicValueCmpLtu)
SVSTR(==,  SymbolicValueEq)
SVSTR(*,   SymbolicValueMul)
SVSTR(|,   SymbolicValueOr)
SVSTR(<<,  SymbolicValueShl)
SVSTR(>>,  SymbolicValueShr)
SVSTR(-,   SymbolicValueSub)
SVSTR(^,   SymbolicValueXor)

const SymbolicValue SymbolicValue :: operator~ () const
{
    if (not wild) return SymbolicValue(this->bits, ~(this->value));
    else return SymbolicValueNot(*this);
}

const SymbolicValue SymbolicValue :: signExtend () const
{
    return SymbolicValue(64, g_svalue());
}

#define SVOPERATOR(OPER, CLASS) \
const SymbolicValue SymbolicValue :: operator OPER (const SymbolicValue & rhs) const \
{                                                                                 \
    if ((not this->wild) && (not rhs.wild))                                       \
        return SymbolicValue(this->bits, this->g_value() OPER rhs.g_value());             \
    else                                                                          \
        return CLASS(*this, rhs);                                                 \
}

SVOPERATOR(+,  SymbolicValueAdd)
SVOPERATOR(&,  SymbolicValueAnd)
SVOPERATOR(*,  SymbolicValueMul)
SVOPERATOR(|,  SymbolicValueOr)
SVOPERATOR(<<, SymbolicValueShl)
SVOPERATOR(>>, SymbolicValueShr)
SVOPERATOR(-,  SymbolicValueSub)
SVOPERATOR(^,  SymbolicValueXor)


#define SVCMP(OPER, CLASS) \
const SymbolicValue SymbolicValue :: operator OPER (const SymbolicValue & rhs) const \
{                                                                                    \
    if ((not this->wild) && (not rhs.wild)) {                                        \
        if (this->g_value() OPER rhs.g_value()) return SymbolicValue(1, 1);                  \
        else return SymbolicValue(1, 0);                                             \
    }                                                                                \
    else                                                                             \
        return CLASS(*this, rhs);                                                    \
}

SVCMP(==, SymbolicValueEq)

const SymbolicValue SymbolicValue :: cmpLes (const SymbolicValue & rhs) const
{
    if ((not this->wild) && (not rhs.wild)) {
        if (g_svalue() <= rhs.g_svalue()) return SymbolicValue(1, 1);
        else return SymbolicValue(1, 0);
    }
    else
        return SymbolicValueCmpLtu(*this, rhs);
}

const SymbolicValue SymbolicValue :: cmpLeu (const SymbolicValue & rhs) const
{
    if ((not this->wild) && (not rhs.wild)) {
        if (g_value() <= rhs.g_value()) return SymbolicValue(1, 1);
        else return SymbolicValue(1, 0);
    }
    else
        return SymbolicValueCmpLtu(*this, rhs);
}

const SymbolicValue SymbolicValue :: cmpLts (const SymbolicValue & rhs) const
{
    if ((not this->wild) && (not rhs.wild)) {
        if (g_svalue() < rhs.g_svalue()) return SymbolicValue(1, 1);
        else return SymbolicValue(1, 0);
    }
    else
        return SymbolicValueCmpLts(*this, rhs);
}

const SymbolicValue SymbolicValue :: cmpLtu (const SymbolicValue & rhs) const
{
    if ((not this->wild) && (not rhs.wild)) {
        if (g_value() < rhs.g_value()) return SymbolicValue(1, 1);
        else return SymbolicValue(1, 0);
    }
    else
        return SymbolicValueCmpLtu(*this, rhs);
}