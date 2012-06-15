#ifndef symbolicvalue_HEADER
#define symbolicvalue_HEADER

#include <inttypes.h>

#include <string>

class SymbolicValue {
    public :
        std::string name;
        int         bits;
        uint64_t    value;
        bool        wild; // all values possible
    
        SymbolicValue (std::string name, int bits, uint64_t value)
            : name(name), bits(bits), value(value), wild(false) {}
        SymbolicValue (std::string name, int bits)
            : name(name), bits(bits), value(0), wild(true) {}
        
        std::string to_str ();
};

class SymbolicValueBinOp : public SymbolicValue {
    private :
        SymbolicValue & left;
        SymbolicValue & right;
    public :
        SymbolicValueBinOp (std::string name, int bits, SymbolicValue & left, SymbolicValue & right)
            : SymbolicValue(name, bits), left(left), right(right) {}
        SymbolicValueBinOp (SymbolicValue & left, SymbolicValue & right)
            : SymbolicValue("", 64), left(left), right(right) {}
};

class SymbolicValueAnd         : public SymbolicValueBinOp {
    public :
        SymbolicValueAnd (SymbolicValue & left, SymbolicValue & right)
            : SymbolicValueBinOp("and", 64, left, right) {}
};

class SymbolicValueOr          : public SymbolicValueBinOp {};
class SymbolicValueXor         : public SymbolicValueBinOp {};
class SymbolicValueShl         : public SymbolicValueBinOp {
    public :
        SymbolicValueShl (SymbolicValue & left, SymbolicValue & right)
            : SymbolicValueBinOp(left, right) {}
};
class SymbolicValueLessThan    : public SymbolicValueBinOp {};
class SymbolicValueGreaterThan : public SymbolicValueBinOp {};
class SymbolicValueNotEquals   : public SymbolicValueBinOp {};

#endif
