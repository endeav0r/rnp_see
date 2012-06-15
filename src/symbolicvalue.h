#ifndef symbolicvalue_HEADER
#define symbolicvalue_HEADER

#include <inttypes.h>

#include <string>

class SymbolicValue {
    public :
        int         bits;
        uint64_t    value;
        bool        wild; // all values possible
    
        SymbolicValue () : bits(0), value(0), wild(false) {}
        SymbolicValue (int bits, uint64_t value)
         : bits(bits), value(value), wild(false) {}
        SymbolicValue (int bits)
         : bits(bits), value(0), wild(true) {}
        
        std::string to_str  ();
        uint64_t    g_value () { return value; }
        int         g_bits  () { return bits;  }
};

class SymbolicValueBinOp : public SymbolicValue {
    private :
        SymbolicValue & left;
        SymbolicValue & right;
    public :
        SymbolicValueBinOp (int bits, SymbolicValue & left, SymbolicValue & right)
         : SymbolicValue(bits), left(left), right(right) {}
};

class SymbolicValueAnd         : public SymbolicValueBinOp {
    public :
        SymbolicValueAnd (SymbolicValue & left, SymbolicValue & right)
            : SymbolicValueBinOp(64, left, right) {}
};

class SymbolicValueOr          : public SymbolicValueBinOp {};
class SymbolicValueXor         : public SymbolicValueBinOp {};
class SymbolicValueShl         : public SymbolicValueBinOp {
    public :
        SymbolicValueShl (SymbolicValue & left, SymbolicValue & right)
            : SymbolicValueBinOp(64, left, right) {}
};
class SymbolicValueLessThan    : public SymbolicValueBinOp {};
class SymbolicValueGreaterThan : public SymbolicValueBinOp {};
class SymbolicValueNotEquals   : public SymbolicValueBinOp {};

#endif
