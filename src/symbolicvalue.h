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
SVBINOPCLASS(Eq)
SVBINOPCLASS(Mul)
SVBINOPCLASS(Or)
SVBINOPCLASS(Sub)
SVBINOPCLASS(Xor)
SVBINOPCLASS(Shl)
SVBINOPCLASS(Shr)

#endif
