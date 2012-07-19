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

enum {
    SVT_NONE,
    SVT_CONSTANT,
    SVT_ADD,
    SVT_AND,
    SVT_CMPLES,
    SVT_CMPLEU,
    SVT_CMPLTS,
    SVT_CMPLTU,
    SVT_DIV,
    SVT_EQ,
    SVT_MOD,
    SVT_MUL,
    SVT_NOT,
    SVT_OR,
    SVT_SEXT,
    SVT_SHL,
    SVT_SHR,
    SVT_SUB,
    SVT_XOR
};

namespace z3 { class expr; class context; }

class SymbolicValueSSA {
    public :
        static SymbolicValueSSA & get ()
        {
            static SymbolicValueSSA instance;
            return instance;
        }
        uint64_t next() { return next_id++; }
    private :
        uint64_t next_id;
        SymbolicValueSSA () : next_id(0) {}
        SymbolicValueSSA (SymbolicValueSSA &);
        void operator = (SymbolicValueSSA &);
};

class SymbolicValue {
    protected :
        int type;
        uint64_t ssa;
        UInt     value;
        bool     wild; // all values possible
        SymbolicValue * lhs;
        SymbolicValue * rhs;

        std::string z3_name () const;
    
    public :
        SymbolicValue ();
        SymbolicValue (int bits, uint64_t value64);
        SymbolicValue (int bits);
        SymbolicValue (const UInt & value);
        SymbolicValue (int type,
                       const SymbolicValue & lhs,
                       const SymbolicValue & rhs);
        ~SymbolicValue ();

        SymbolicValue & operator = (const SymbolicValue & rhs);
        SymbolicValue & operator = (SymbolicValue & rhs);
        SymbolicValue & operator = (const SymbolicValue & rhs) const;
        SymbolicValue & operator = (SymbolicValue & rhs) const;
        
        virtual const std::string str () const;

        const SymbolicValue extend      (int bits) const;
        const SymbolicValue signExtend  (int bits) const;
        
        UInt     g_value  () const { return value;             }
        uint64_t g_uint64 () const { return value.g_value64(); }
        int      g_bits   () const { return value.g_bits();    }
        bool     g_wild   () const { return wild;              }

        SymbolicValue operator +  (const SymbolicValue & rhs) const;
        SymbolicValue operator -  (const SymbolicValue & rhs) const;
        SymbolicValue operator *  (const SymbolicValue & rhs) const;
        SymbolicValue operator /  (const SymbolicValue & rhs) const;
        SymbolicValue operator %  (const SymbolicValue & rhs) const;
        SymbolicValue operator &  (const SymbolicValue & rhs) const;
        SymbolicValue operator |  (const SymbolicValue & rhs) const;
        SymbolicValue operator ^  (const SymbolicValue & rhs) const;
        SymbolicValue operator >> (const SymbolicValue & rhs) const;
        SymbolicValue operator << (const SymbolicValue & rhs) const;
        SymbolicValue operator == (const SymbolicValue & rhs) const;

        SymbolicValue cmpLes      (const SymbolicValue & rhs) const;
        SymbolicValue cmpLeu      (const SymbolicValue & rhs) const;
        SymbolicValue cmpLts      (const SymbolicValue & rhs) const;
        SymbolicValue cmpLtu      (const SymbolicValue & rhs) const;

        SymbolicValue operator ~  () const;

        // creates a z3 expression which evaluates this SymbolicValue in the
        // given z3 context
        virtual z3::expr context    (z3::context & c) const;
        virtual z3::expr contextCmp (z3::context & c, z3::expr && cond) const;

        // asserts a wild symbolic value can equal the given value
        virtual bool svassert (const SymbolicValue & value);
};


#endif
