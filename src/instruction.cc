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


std::string InstructionOperand :: str ()
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
        else ss << std::hex << this->id;
        ss << ")";
            return ss.str();
    }
    
    return ss.str();
}


uint64_t InstructionOperand :: str_to_id (std::string str)
{
    uint64_t id = 1 << 31;
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


std::string Instruction :: str ()
{
    return str_formatter("???", "");
}

std::string InstructionSyscall :: str ()
{
    return str_formatter("syscall", "");
}

std::string InstructionAssign :: str ()
{
    return str_formatter("assign", this->dst.str() + " = " + this->src.str());
}

std::string InstructionStore :: str ()
{
    std::stringstream ss;
    
    ss << bits << "[" << this->dst.str() << "] = " << this->src.str();
    return str_formatter("store", ss.str());
}

std::string InstructionLoad :: str ()
{
    std::stringstream ss;
    ss << this->dst.str() << " = " << bits << "[" << this->src.str() << "]";
    return str_formatter("load", ss.str());
}

std::string InstructionNot :: str ()
{
    return str_formatter("not", this->dst.str() + " = ~" + this->src.str());
}

std::string InstructionBrc :: str()
{   
    return str_formatter("brc", this->cond.str() + " ? " + this->dst.str());
}

std::string InstructionSignExtend :: str()
{   
    return str_formatter("sext", this->dst.str() + " <- " + this->src.str());
}

std::string InstructionHlt :: str()
{
    return str_formatter("hlt", "");
}

std::string InstructionBinOp :: binop_str (std::string mnemonic, std::string op, std::string dst, std::string lhs, std::string rhs)
{
    return Instruction::str_formatter(mnemonic, dst + " = " + lhs + " " + op + " " + rhs);
}

std::string InstructionMul :: str ()
{
    return binop_str("mul", "*", dst.str(), lhs.str(), rhs.str());
}

std::string InstructionDiv :: str ()
{
    return binop_str("div", "/", dst.str(), lhs.str(), rhs.str());
}

std::string InstructionMod :: str ()
{
    return binop_str("mod", "%", dst.str(), lhs.str(), rhs.str());
}


std::string InstructionAdd :: str ()
{
    return binop_str("add", "+", dst.str(), lhs.str(), rhs.str());
}


std::string InstructionSub :: str ()
{
    return binop_str("sub", "-", dst.str(), lhs.str(), rhs.str());
}


std::string InstructionXor :: str ()
{
    return binop_str("xor", "^", dst.str(), lhs.str(), rhs.str());
}


std::string InstructionOr :: str ()
{
    return binop_str("or", "|", dst.str(), lhs.str(), rhs.str());
}


std::string InstructionAnd :: str ()
{
    return binop_str("and", "&", dst.str(), lhs.str(), rhs.str());
}


std::string InstructionShl :: str ()
{
    return binop_str("shl", "<<", dst.str(), lhs.str(), rhs.str());
}


std::string InstructionShr :: str ()
{
    return binop_str("shr", ">>", dst.str(), lhs.str(), rhs.str());
}


std::string InstructionCmpOp :: cmpop_str (std::string mnemonic, std::string op, std::string dst, std::string lhs, std::string rhs)
{
    return Instruction::str_formatter(mnemonic, dst + " = " + lhs + " " + op + " " + rhs);
}

std::string InstructionCmpEq :: str ()
{
    return cmpop_str ("cmpEq", "==", dst.str(), lhs.str(), rhs.str());
}

std::string InstructionCmpLeu :: str ()
{
    return cmpop_str ("cmpLeu", "<=", dst.str(), lhs.str(), rhs.str());
}

std::string InstructionCmpLes :: str ()
{
    return cmpop_str ("cmpLes", "<=", dst.str(), lhs.str(), rhs.str());
}

std::string InstructionCmpLtu :: str ()
{
    return cmpop_str ("cmpLtu", "<", dst.str(), lhs.str(), rhs.str());
}

std::string InstructionCmpLts :: str ()
{
    return cmpop_str ("cmpLts", "<", dst.str(), lhs.str(), rhs.str());
}
