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

#ifndef symbolicvalue_HEADER
#define symbolicvalue_HEADER

#include <inttypes.h>

#include <iostream>
#include <string>

#include "uint.h"

class SymbolicValue {
    protected :
        UInt value;
        bool wild; // all values possible
    
    public :
        SymbolicValue ();
        SymbolicValue (int bits, uint64_t value64);
        SymbolicValue (int bits);
        SymbolicValue (UInt value);
        
        virtual const std::string str () const;

        const SymbolicValue extend (int bits) const;
        
        UInt     g_value  () const { return value;             }
        uint64_t g_uint64 () const { return value.g_value64(); }
        int      g_bits   () const { return value.g_bits();    }
        bool     g_wild   () const { return wild;              }

        const SymbolicValue operator +  (const SymbolicValue & rhs) const;
        const SymbolicValue operator -  (const SymbolicValue & rhs) const;
        const SymbolicValue operator *  (const SymbolicValue & rhs) const;
        const SymbolicValue operator /  (const SymbolicValue & rhs) const;
        const SymbolicValue operator %  (const SymbolicValue & rhs) const;
        const SymbolicValue operator &  (const SymbolicValue & rhs) const;
        const SymbolicValue operator |  (const SymbolicValue & rhs) const;
        const SymbolicValue operator ^  (const SymbolicValue & rhs) const;
        const SymbolicValue operator == (const SymbolicValue & rhs) const;
        const SymbolicValue operator != (const SymbolicValue & rhs) const;
        const SymbolicValue operator >> (const SymbolicValue & rhs) const;
        const SymbolicValue operator << (const SymbolicValue & rhs) const;
        const SymbolicValue operator ~  () const;

        const SymbolicValue cmpLes      (const SymbolicValue & rhs) const;
        const SymbolicValue cmpLeu      (const SymbolicValue & rhs) const;
        const SymbolicValue cmpLts      (const SymbolicValue & rhs) const;
        const SymbolicValue cmpLtu      (const SymbolicValue & rhs) const;

        const SymbolicValue signExtend  (int bits) const;
};

class SymbolicValueNot : public SymbolicValue {
    protected :
        const SymbolicValue src;
    public :
        SymbolicValueNot (const SymbolicValue & src) : src(src) { wild = true; }
        const std::string str () const;
};

class SymbolicValueBinOp : public SymbolicValue {
    protected :
        const SymbolicValue lhs;
        const SymbolicValue rhs;
    public :
        SymbolicValueBinOp (const SymbolicValue & lhs, const SymbolicValue & rhs)
         : lhs(lhs), rhs(rhs) { wild = true; }
};

#define SVBINOPCLASS(OPERATION) \
class SymbolicValue##OPERATION : public SymbolicValueBinOp { \
    public :                                                 \
        SymbolicValue##OPERATION (const SymbolicValue & lhs, const SymbolicValue & rhs) \
            : SymbolicValueBinOp(lhs, rhs) { wild = true;} \
        const std::string str () const; \
};


SVBINOPCLASS(Add)
SVBINOPCLASS(And)
SVBINOPCLASS(CmpLes)
SVBINOPCLASS(CmpLeu)
SVBINOPCLASS(CmpLts)
SVBINOPCLASS(CmpLtu)
SVBINOPCLASS(Div)
SVBINOPCLASS(Eq)
SVBINOPCLASS(Mod)
SVBINOPCLASS(Mul)
SVBINOPCLASS(Or)
SVBINOPCLASS(Sub)
SVBINOPCLASS(Xor)
SVBINOPCLASS(Shl)
SVBINOPCLASS(Shr)

#endif
