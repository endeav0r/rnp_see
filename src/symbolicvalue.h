#ifndef symbolicvalue_HEADER
#define symbolicvalue_HEADER

#include <inttypes.h>

#include <string>

class SymbolicValue {
    protected :
        int         bits;
        uint64_t    value;
        bool        wild; // all values possible
    
    public :
        SymbolicValue () : bits(0), value(0), wild(false) {}
        SymbolicValue (int bits, uint64_t value) : bits(bits), value(value), wild(false) {}
        SymbolicValue (int bits)                 : bits(bits), value(0),     wild(true) {}
        
        virtual std::string str () const;

        uint64_t    g_value  () const { return value; }
        int64_t     g_svalue () const;
        int         g_bits   () const { return bits;  }
        bool        g_wild   () const { return wild;  }

        const SymbolicValue operator +  (const SymbolicValue & rhs) const;
        const SymbolicValue operator -  (const SymbolicValue & rhs) const;
        const SymbolicValue operator &  (const SymbolicValue & rhs) const;
        const SymbolicValue operator ^  (const SymbolicValue & rhs) const;
        const SymbolicValue operator == (const SymbolicValue & rhs) const;
        const SymbolicValue cmpLts      (const SymbolicValue & rhs) const;

        const SymbolicValue signExtend () const;
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
        std::string str () const; \
};


SVBINOPCLASS(Add)
SVBINOPCLASS(And)
SVBINOPCLASS(CmpLts)
SVBINOPCLASS(Eq)
SVBINOPCLASS(Sub)
SVBINOPCLASS(Xor)

#endif
