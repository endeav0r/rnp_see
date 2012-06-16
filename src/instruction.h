/*
 * This instruction set borrows heavily from RREIL, detailed in the paper
 * Precise Static Analysis of Binaries by Extracting Relational Information
 * www2.in.tum.de/bib/files/sepp11precise.pdf
 */

#ifndef instruction_HEADER
#define instruction_HEADER

#include <iostream>
#include <string>

#include <inttypes.h>

#include "symbolicvalue.h"

#define OPTYPE_INVALID  0
#define OPTYPE_VAR      1
#define OPTYPE_CONSTANT 2
#define OPTYPE_SIGNED   8


class InstructionOperandTmpVar {
    public :
        static InstructionOperandTmpVar & get()
        {
            static InstructionOperandTmpVar instance;
            return instance;
        }
        uint64_t next ();
    private :
        uint64_t next_id;
        InstructionOperandTmpVar ();
        InstructionOperandTmpVar (InstructionOperandTmpVar &);
        void operator = (InstructionOperandTmpVar &);
};


class InstructionOperand {
    private :
        uint64_t    id;
        int         type;
        int         bits;
        uint64_t    value;
        std::string name;
        
        void setid ();
    public :
        InstructionOperand () : id(0), type(OPTYPE_INVALID), bits(0), value(0) {}
        InstructionOperand (int type, int bits, uint64_t value, std::string name);
        InstructionOperand (int type, int bits, uint64_t value);
        InstructionOperand (int type, int bits);
        InstructionOperand (int type, int bits, std::string name);
        
        void        sign    ();
        int         g_type  () { return type; }
        uint64_t    g_value () { return value; }
        int         g_bits  () { return bits; }
        std::string g_name  () { return name; }
        uint64_t    g_id    () { return id; }

        static uint64_t str_to_id (std::string);
        
        std::string str ();
        
        bool operator == (const InstructionOperand & rhs);
};

/*********************
 * INSTRUCTIONS      *
 ********************/
 

class InstructionTmpVar {
    public :
        static InstructionTmpVar & get ()
        {
            static InstructionTmpVar instance;
            return instance;
        }
        uint64_t next() { return next_id++; }
    private :
        uint64_t next_id;
        InstructionTmpVar () { next_id = 0; }
        InstructionTmpVar (InstructionTmpVar &);
        void operator = (InstructionTmpVar &);
};

class Instruction {
    private :
        uint64_t id;
        uint64_t address;
        uint32_t size;
    
    public :
        
        Instruction (uint64_t address, uint32_t size) : address(address), size(size)
        {
            InstructionTmpVar & tmp = InstructionTmpVar :: get();
            id = tmp.next();
        }
        virtual ~Instruction() {}
        
        virtual std::string str ();
        std::string str_formatter (std::string mnemonic, std::string args);
        
        uint64_t g_address () { return address; }
        uint32_t g_size    () { return size; }
        uint64_t g_id      () { return id; }
};

/*********************
 * BASE INSTRUCTIONS *
 ********************/

class InstructionLoad : public Instruction {
    private :
        int bits;
        InstructionOperand dst;
        InstructionOperand src;
    public :
        InstructionLoad (uint64_t address, uint32_t size, int bits, InstructionOperand & dst, InstructionOperand & src)
            : Instruction(address, size), bits(bits), dst(dst), src(src) {}
        std::string str();
        InstructionOperand g_dst () { return dst; }
        InstructionOperand g_src () { return src; }
};

class InstructionStore : public Instruction {
    private :
        int bits;
        InstructionOperand dst;
        InstructionOperand src;
    public :
        InstructionStore (uint64_t address, uint32_t size, int bits, InstructionOperand & dst, InstructionOperand & src)
            : Instruction(address, size), bits(bits), dst(dst), src(src) {}
        std::string str();
        InstructionOperand g_dst () { return dst; }
        InstructionOperand g_src () { return src; }
};

class InstructionBrc : public Instruction {
    private :
        InstructionOperand cond;
        InstructionOperand dst;
    public :
        InstructionBrc (uint64_t address, uint32_t size, InstructionOperand & cond, InstructionOperand & dst)
            : Instruction(address, size), cond(cond), dst(dst) {}
        std::string str();
        InstructionOperand g_cond () { return cond; }
        InstructionOperand g_dst  () { return dst; }
};

class InstructionAssign : public Instruction {
    private :
        InstructionOperand dst;
        InstructionOperand src;
    public :
        InstructionAssign (uint64_t address, uint32_t size, InstructionOperand & dst, InstructionOperand & src)
            : Instruction(address, size), dst(dst), src(src) {}
        std::string str();
        InstructionOperand g_dst () { return dst; }
        InstructionOperand g_src () { return src; }
};

class InstructionCall : public Instruction {
    public :
        InstructionOperand dst;
        
        InstructionCall (uint64_t address, uint32_t size, InstructionOperand dst)
            : Instruction(address, size), dst(dst) {}
        std::string str ();
};

class InstructionRet : public Instruction {
    public :
        InstructionRet (uint64_t address, uint32_t size)
            : Instruction(address, size) {}
        std::string str ();
};

class InstructionNot : public Instruction {
    private :
        InstructionOperand dst;
        InstructionOperand src;
    public :
        InstructionNot (uint64_t address, uint32_t size, InstructionOperand dst, InstructionOperand src)
            : Instruction(address, size), dst(dst), src(src) {}
        std::string str ();
        InstructionOperand g_dst () { return dst; }
        InstructionOperand g_src () { return src; }
};

class InstructionSignExtend : public Instruction {
    private :
        InstructionOperand dst;
        InstructionOperand src;
    public :
        InstructionSignExtend (uint64_t address, uint32_t size, InstructionOperand dst, InstructionOperand src)
            : Instruction(address, size), dst(dst), src(src) {}
        std::string str ();
        InstructionOperand g_dst () { return dst; }
        InstructionOperand g_src () { return src; }
};

class InstructionHlt : public Instruction {
    public :
        InstructionHlt (uint64_t address, uint32_t size)
            : Instruction(address, size) {}
        std::string str ();
};

/********************
 * BASE STATEMENTS  *
 *******************/

class InstructionBaseStmt : public Instruction {
    public :
        InstructionBaseStmt (uint64_t address, uint32_t size)
            : Instruction(address, size) {}
};

/***********
 * BINOPS  *
 **********/

class InstructionBinOp : public InstructionBaseStmt {
    protected :
        InstructionOperand dst;
        InstructionOperand lhs;
        InstructionOperand rhs;
    public :
        
        InstructionBinOp (uint64_t address,
                          uint32_t size,
                          InstructionOperand dst,
                          InstructionOperand lhs,
                          InstructionOperand rhs)
            : InstructionBaseStmt(address, size), dst(dst), lhs(lhs), rhs(rhs) {}
        InstructionOperand g_dst () { return dst; }
        InstructionOperand g_lhs () { return lhs; }
        InstructionOperand g_rhs () { return rhs; }
        std::string binop_str (std::string mnemonic, std::string op, std::string dst, std::string lhs, std::string rhs);
};

class InstructionAdd  : public InstructionBinOp {
    public :
        InstructionAdd (uint64_t address, uint32_t size, InstructionOperand dst, InstructionOperand lhs, InstructionOperand rhs)
            : InstructionBinOp(address, size, dst, lhs, rhs) {}
        std::string str();
};

class InstructionSub  : public InstructionBinOp {
    public :
        InstructionSub (uint64_t address, uint32_t size, InstructionOperand dst, InstructionOperand lhs, InstructionOperand rhs)
            : InstructionBinOp(address, size, dst, lhs, rhs) {}
        std::string str();
};

class InstructionMul  : public InstructionBinOp {
    public :
        InstructionMul (uint64_t address, uint32_t size, InstructionOperand dst, InstructionOperand lhs, InstructionOperand rhs)
            : InstructionBinOp(address, size, dst, lhs, rhs) {}
        std::string str();
};

class InstructionDiv  : public InstructionBinOp {};
class InstructionDivs : public InstructionBinOp {};
class InstructionShl  : public InstructionBinOp {
    public :
        InstructionShl (uint64_t address, uint32_t size, InstructionOperand dst, InstructionOperand lhs, InstructionOperand rhs)
            : InstructionBinOp(address, size, dst, lhs, rhs) {}
        std::string str();
};
class InstructionShr  : public InstructionBinOp {
    public :
        InstructionShr (uint64_t address, uint32_t size, InstructionOperand dst, InstructionOperand lhs, InstructionOperand rhs)
            : InstructionBinOp(address, size, dst, lhs, rhs) {}
        std::string str();
};
class InstructionShrs : public InstructionBinOp {};
class InstructionMod  : public InstructionBinOp {};

class InstructionAnd  : public InstructionBinOp {
    public :
        InstructionAnd (uint64_t address, uint32_t size, InstructionOperand dst, InstructionOperand lhs, InstructionOperand rhs)
            : InstructionBinOp(address, size, dst, lhs, rhs) {}
        std::string str();
};

class InstructionOr   : public InstructionBinOp {
    public :
        InstructionOr (uint64_t address, uint32_t size, InstructionOperand dst, InstructionOperand lhs, InstructionOperand rhs)
            : InstructionBinOp(address, size, dst, lhs, rhs) {}
        std::string str();
};

class InstructionXor  : public InstructionBinOp {
    public :
        InstructionXor (uint64_t address, uint32_t size, InstructionOperand dst, InstructionOperand lhs, InstructionOperand rhs)
            : InstructionBinOp(address, size, dst, lhs, rhs) {}
        std::string str();
};

/***********
 * CMPOPS  *
 **********/

class InstructionCmpOp  : public InstructionBaseStmt {
    protected :
        InstructionOperand dst;
        InstructionOperand lhs;
        InstructionOperand rhs;
    public :
        
        InstructionCmpOp (uint64_t address, uint32_t size, InstructionOperand & dst, InstructionOperand & lhs, InstructionOperand & rhs)
            : InstructionBaseStmt(address, size), dst(dst), lhs(lhs), rhs(rhs) {}
        InstructionOperand g_dst () { return dst; }
        InstructionOperand g_lhs () { return lhs; }
        InstructionOperand g_rhs () { return rhs; }
        std::string cmpop_str (std::string mnemonic, std::string op, std::string dst, std::string lhs, std::string rhs);
};

class InstructionCmpEq  : public InstructionCmpOp {
    public :
        InstructionCmpEq (uint64_t address, uint32_t size, InstructionOperand & dst, InstructionOperand & lhs, InstructionOperand & rhs)
            : InstructionCmpOp(address, size, dst, lhs, rhs) {}
        std::string str();

};
class InstructionCmpLeu : public InstructionCmpOp {
    public :
        InstructionCmpLeu (uint64_t address, uint32_t size, InstructionOperand & dst, InstructionOperand & lhs, InstructionOperand & rhs)
            : InstructionCmpOp(address, size, dst, lhs, rhs) {}
        std::string str();
};

class InstructionCmpLes : public InstructionCmpOp {
    public :
        InstructionCmpLes (uint64_t address, uint32_t size, InstructionOperand & dst, InstructionOperand & lhs, InstructionOperand & rhs)
            : InstructionCmpOp(address, size, dst, lhs, rhs) {}
        std::string str();
};

class InstructionCmpLtu : public InstructionCmpOp {
    public :
        InstructionCmpLtu (uint64_t address, uint32_t size, InstructionOperand & dst, InstructionOperand & lhs, InstructionOperand & rhs)
            : InstructionCmpOp(address, size, dst, lhs, rhs) {}
        std::string str();
};

class InstructionCmpLts : public InstructionCmpOp {
    public :
        InstructionCmpLts (uint64_t address, uint32_t size, InstructionOperand & dst, InstructionOperand & lhs, InstructionOperand & rhs)
            : InstructionCmpOp(address, size, dst, lhs, rhs) {}
        std::string str();
};



#endif
