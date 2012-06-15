#include "instruction.h"

#include "debug.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

InstructionOperandTmpVar :: InstructionOperandTmpVar ()
{
    this->next_id = 0x1000;
}

uint64_t InstructionOperandTmpVar :: next ()
{
    return this->next_id++;
}

void InstructionOperand :: sign ()
{
    int64_t sv;

    switch (bits) {
    case 8  : sv = (int8_t)  value; this->value = sv; break;
    case 16 : sv = (int16_t) value; this->value = sv; break;
    case 32 : sv = (int32_t) value; this->value = sv; break;
    case 64 : sv = (int64_t) value; this->value = sv; break;
    }
    
    type |= OPTYPE_SIGNED;
}

void InstructionOperand :: setid ()
{
    // variables with names get an identifier based on their name
    // with some special sauce to prevent identifical identifiers
    if ((type == OPTYPE_VAR) && (name != "")) {
        id = str_to_id(name);
    }

    // otherwise the variable gets a unique identifier
    else {
        InstructionOperandTmpVar & tmpvar = InstructionOperandTmpVar :: get();
        id = tmpvar.next();
    }
}

InstructionOperand :: InstructionOperand (int type, int bits, uint64_t value)
    : type(type), bits(bits), value(value), name("")
{
    setid();

    if ((! type & OPTYPE_VAR) && (! type & OPTYPE_CONSTANT))
        throw std::runtime_error("InstructionOperand expected OPTYPE_VAR or OPTYPE_CONSTANT");
    
    if (type & OPTYPE_SIGNED)
        sign();
}

InstructionOperand :: InstructionOperand (int type, int bits, std::string name)
    : type(type), bits(bits), value(0), name(name)
{
    setid();
}

InstructionOperand :: InstructionOperand (int type, int bits)
    : type(type), bits(bits), value(0), name("")
{
    if (type != OPTYPE_VAR)
        throw std::runtime_error("InstructionOperand expected OPTYPE_VAR");
    
    setid();
}


std::string InstructionOperand :: to_str ()
{
    std::stringstream ss;
    
    ss << "(";
    switch (this->type & 0x7) {
    case OPTYPE_CONSTANT :
        ss << "CONST " << this->bits << " "
           << "0x" << std::hex << this->value << ")";
        break;
    case OPTYPE_VAR   :
        ss << "VAR " << this->bits << " ";
        if (this->name != "") ss << this->name;
        else ss << this->id;
        ss << ")";
            return ss.str();
    }
    
    return ss.str();
}


uint64_t InstructionOperand :: str_to_id (std::string str)
{
    uint64_t id = 0x01234567;
    id <<= 32;
    id |= 0xfcedba98;
    for (size_t i = 0; i < str.length(); i++) {
        id += (int) str[i] << (8 * (i % 8));
    }
    return id;
}


bool InstructionOperand :: operator == (const InstructionOperand & rhs)
{
    if (id == rhs.id)
        return true;
    return false;
}



std::string align_mnemonic (std::string mnemonic)
{
    while (mnemonic.length() < 8) mnemonic += " ";
    return mnemonic;
}


std::string Instruction :: str_formatter (std::string mnemonic, std::string args)
{
    std::stringstream ss;
    
    ss << std::hex << this->address << " " << align_mnemonic(mnemonic) << args;
    return ss.str();
}


std::string Instruction :: to_str ()
{
    return str_formatter("???", "");
}

std::string InstructionAssign :: to_str ()
{
    return str_formatter("assign", this->dst.to_str() + " = " + this->src.to_str());
}

std::string InstructionStore :: to_str ()
{
    std::stringstream ss;
    
    ss << bits << "[" << this->dst.to_str() << "] = " << this->src.to_str();
    return str_formatter("store", ss.str());
}

std::string InstructionLoad :: to_str ()
{
    return str_formatter("load", this->dst.to_str() + " = [" + this->src.to_str() + "]");
}

std::string InstructionCall :: to_str ()
{
    return str_formatter("call", dst.to_str());
}

std::string InstructionRet :: to_str ()
{
    return str_formatter("ret", "");
}

std::string InstructionNot :: to_str ()
{
    return str_formatter("not", this->dst.to_str() + " = ~" + this->src.to_str());
}

std::string InstructionBrc :: to_str()
{   
    return str_formatter("brc", this->cond.to_str() + " ? " + this->dst.to_str());
}

std::string InstructionSignExtend :: to_str()
{   
    return str_formatter("sext", this->dst.to_str() + " <- " + this->src.to_str());
}

std::string InstructionHlt :: to_str()
{
    return str_formatter("hlt", "");
}

std::string InstructionBinOp :: binop_to_str (std::string mnemonic, std::string op, std::string dst, std::string lhs, std::string rhs)
{
    return Instruction::str_formatter(mnemonic, dst + " = " + lhs + " " + op + " " + rhs);
}

std::string InstructionMul :: to_str ()
{
    return binop_to_str("mul", "*", dst.to_str(), lhs.to_str(), rhs.to_str());
}


std::string InstructionAdd :: to_str ()
{
    return binop_to_str("add", "+", dst.to_str(), lhs.to_str(), rhs.to_str());
}


std::string InstructionSub :: to_str ()
{
    return binop_to_str("sub", "-", dst.to_str(), lhs.to_str(), rhs.to_str());
}


std::string InstructionXor :: to_str ()
{
    return binop_to_str("xor", "^", dst.to_str(), lhs.to_str(), rhs.to_str());
}


std::string InstructionOr :: to_str ()
{
    return binop_to_str("or", "|", dst.to_str(), lhs.to_str(), rhs.to_str());
}


std::string InstructionAnd :: to_str ()
{
    return binop_to_str("and", "&", dst.to_str(), lhs.to_str(), rhs.to_str());
}


std::string InstructionShl :: to_str ()
{
    return binop_to_str("shl", "<<", dst.to_str(), lhs.to_str(), rhs.to_str());
}


std::string InstructionShr :: to_str ()
{
    return binop_to_str("shr", ">>", dst.to_str(), lhs.to_str(), rhs.to_str());
}


std::string InstructionCmpOp :: cmpop_to_str (std::string mnemonic, std::string op, std::string dst, std::string lhs, std::string rhs)
{
    return Instruction::str_formatter(mnemonic, dst + " = " + lhs + " " + op + " " + rhs);
}

std::string InstructionCmpEq :: to_str ()
{
    return cmpop_to_str ("cmpEq", "==", dst.to_str(), lhs.to_str(), rhs.to_str());
}

std::string InstructionCmpLeu :: to_str ()
{
    return cmpop_to_str ("cmpLeu", "<=", dst.to_str(), lhs.to_str(), rhs.to_str());
}

std::string InstructionCmpLes :: to_str ()
{
    return cmpop_to_str ("cmpLes", "<=", dst.to_str(), lhs.to_str(), rhs.to_str());
}

std::string InstructionCmpLtu :: to_str ()
{
    return cmpop_to_str ("cmpLtu", "<", dst.to_str(), lhs.to_str(), rhs.to_str());
}

std::string InstructionCmpLts :: to_str ()
{
    return cmpop_to_str ("cmpLts", "<", dst.to_str(), lhs.to_str(), rhs.to_str());
}
