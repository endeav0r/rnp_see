/*
    Copyright 2012 Alex Eubanks (endeavor[at]rainbowsandpwnies.com)

    This file is part of rnp_see ( http://github.com/endeav0r/rnp_see/ )

    rnp_see is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
        if (value.cmpLes(rhs.g_value()))
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
        if (value.cmpLts(rhs.g_value()))
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