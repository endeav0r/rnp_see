#include "symbolicvalue.h"

#include <iostream>
#include <sstream>

SymbolicValue :: SymbolicValue ()
{
    wild = false;
    value = UInt(0);
}

SymbolicValue :: SymbolicValue (int bits, uint64_t value64)
{
    wild = false;
    value = UInt(bits, value64);
}

SymbolicValue :: SymbolicValue (int bits)
{
    wild = true;
    value = UInt(bits);
}

SymbolicValue :: SymbolicValue (UInt value)
{
    wild = false;
    this->value = value;
}

const std::string SymbolicValue :: str () const
{
    std::stringstream ss;

    if (wild) ss << "(" << g_bits() << " wild)";
    else ss << "(" << g_bits() << " " << value.str() << ")";
    
    return ss.str();
}

const std::string SymbolicValueNot :: str () const
{
    return std::string("~(") + src.str() + ")";
}

const SymbolicValue SymbolicValue :: extend (int bits) const {
    return SymbolicValue(value.extend(bits));
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
SVSTR(/,   SymbolicValueDiv)
SVSTR(==,  SymbolicValueEq)
SVSTR(%,   SymbolicValueMod)
SVSTR(*,   SymbolicValueMul)
SVSTR(|,   SymbolicValueOr)
SVSTR(<<,  SymbolicValueShl)
SVSTR(>>,  SymbolicValueShr)
SVSTR(-,   SymbolicValueSub)
SVSTR(^,   SymbolicValueXor)

const SymbolicValue SymbolicValue :: operator~ () const
{
    if (not wild) return SymbolicValue(~value);
    else return SymbolicValueNot(*this);
}

const SymbolicValue SymbolicValue :: signExtend (int bits) const
{
    return SymbolicValue(value.sign_extend(bits));
}

#define SVOPERATOR(OPER, CLASS) \
const SymbolicValue SymbolicValue :: operator OPER (const SymbolicValue & rhs) const \
{                                                                                 \
    if ((not this->wild) && (not rhs.wild))                                       \
        return SymbolicValue(this->g_value() OPER rhs.g_value());             \
    else                                                                          \
        return CLASS(*this, rhs);                                                 \
}

SVOPERATOR(+,  SymbolicValueAdd)
SVOPERATOR(&,  SymbolicValueAnd)
SVOPERATOR(/,  SymbolicValueDiv)
SVOPERATOR(%,  SymbolicValueMod)
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
        if (this->g_value() OPER rhs.g_value())                                      \
            return SymbolicValue(1, 1);                                              \
        else                                                                         \
            return SymbolicValue(1, 0);                                              \
    }                                                                                \
    else                                                                             \
        return CLASS(*this, rhs);                                                    \
}

SVCMP(==, SymbolicValueEq)
SVCMP(!=, SymbolicValueEq)

const SymbolicValue SymbolicValue :: cmpLes (const SymbolicValue & rhs) const
{
    if ((not this->wild) && (not rhs.wild)) {
        int bits = value.g_bits() > rhs.value.g_bits() 
                   ? value.g_bits() 
                   : rhs.value.g_bits();
        if (value.sign_extend(bits) <= rhs.value.sign_extend(bits))
            return SymbolicValue(1, 1);
        else
            return SymbolicValue(1, 0);
    }
    else
        return SymbolicValueCmpLtu(*this, rhs);
}

const SymbolicValue SymbolicValue :: cmpLeu (const SymbolicValue & rhs) const
{
    if ((not this->wild) && (not rhs.wild)) {
        if (value <= rhs.value)
            return SymbolicValue(1, 1);
        else
            return SymbolicValue(1, 0);
    }
    else
        return SymbolicValueCmpLtu(*this, rhs);
}

const SymbolicValue SymbolicValue :: cmpLts (const SymbolicValue & rhs) const
{
    if ((not this->wild) && (not rhs.wild)) {
        int bits = value.g_bits() > rhs.value.g_bits() 
                   ? value.g_bits() 
                   : rhs.value.g_bits();
        if (value.sign_extend(bits) < rhs.value.sign_extend(bits))
            return SymbolicValue(1, 1);
        else
            return SymbolicValue(1, 0);
    }
    else
        return SymbolicValueCmpLts(*this, rhs);
}

const SymbolicValue SymbolicValue :: cmpLtu (const SymbolicValue & rhs) const
{
    if ((not this->wild) && (not rhs.wild)) {
        if (value < rhs.value)
            return SymbolicValue(1, 1);
        else
            return SymbolicValue(1, 0);
    }
    else
        return SymbolicValueCmpLtu(*this, rhs);
}