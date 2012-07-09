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

#include "translator.h"

#include <udis86.h>

#include <cstdio>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "debug.h"

std::string ins_debug_str (ud_t * ud_obj)
{
    std::stringstream ss;
    
    ss << ud_insn_asm(ud_obj) << " "
       << "pfx_rex=0x" << std::hex << (int) ud_obj->pfx_rex << ", "
       << "pfx_seg=" << ud_type_DEBUG[ud_obj->pfx_seg] << ", "
       << "pfx_rep=" << (int) ud_obj->pfx_rep << ", "

       << "{type="   << ud_type_DEBUG[ud_obj->operand[0].type] << ", "
       << " size="   << (int) ud_obj->operand[0].size << ", "
       << " offset=" << (int) ud_obj->operand[0].offset << ", "
       << " scale="  << (int) ud_obj->operand[0].scale << ", "
       << " lval="   << (int) ud_obj->operand[0].lval.uqword << ", "
       << " index="   << ud_type_DEBUG[ud_obj->operand[0].index] << ", "
       << " base="   << ud_type_DEBUG[ud_obj->operand[0].base] << "}, "

       << "{type="   << ud_type_DEBUG[ud_obj->operand[1].type] << ", "
       << " size="   << (int) ud_obj->operand[1].size << ", "
       << " offset=" << (int) ud_obj->operand[1].offset << ", "
       << " scale="  << (int) ud_obj->operand[1].scale << ", "
       << " lval="   << (int) ud_obj->operand[1].lval.uqword << ", "
       << " index="   << ud_type_DEBUG[ud_obj->operand[1].index] << ", "
       << " base="   << ud_type_DEBUG[ud_obj->operand[1].base] << "}, "

       << "{type="   << ud_type_DEBUG[ud_obj->operand[2].type] << ", "
       << " size="   << (int) ud_obj->operand[2].size << ", "
       << " offset=" << (int) ud_obj->operand[2].offset << ", "
       << " scale="  << (int) ud_obj->operand[2].scale << ", "
       << " lval="   << (int) ud_obj->operand[2].lval.uqword << ", "
       << " index="   << ud_type_DEBUG[ud_obj->operand[2].index] << ", "
       << " base="   << ud_type_DEBUG[ud_obj->operand[2].base] << "}";
   
   return ss.str();
}

std::string Translator :: native_asm (uint8_t * data, int size)
{
    ud_t ud_obj;
    
    ud_init(&ud_obj);
    ud_set_mode(&ud_obj, 64);
    ud_set_syntax(&ud_obj, UD_SYN_INTEL);
    
    ud_set_input_buffer(&ud_obj, (unsigned char *) data, size);
    ud_disassemble(&ud_obj);

    return ud_insn_asm(&ud_obj);
}


std::list <Instruction *> Translator :: translate (uint64_t address, uint8_t * data, size_t size)
{
    instructions.clear();
    ud_t ud_obj;
    
    ud_init(&ud_obj);
    ud_set_mode(&ud_obj, 64);
    ud_set_syntax(&ud_obj, UD_SYN_INTEL);
    
    ud_set_input_buffer(&ud_obj, (unsigned char *) data, size);
    
    if (ud_disassemble(&ud_obj)) {
        switch (ud_obj.mnemonic) {
        case UD_Iadc       : adc       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Iadd       : add       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Iand       : And       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ibsf       : bsf       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ibt        : bt        (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Icall      : call      (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Icdqe      : cdqe      (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Icmova     : cmova     (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Icmovb     : cmovb     (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Icmovbe    : cmovbe    (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Icmovnz    : cmovnz    (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Icmovs     : cmovs     (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Icmovz     : cmovz     (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Icmp       : cmp       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Icmpxchg   : cmpxchg   (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Idec       : dec       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Idiv       : div       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ihlt       : hlt       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Iimul      : imul      (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Iinc       : inc       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ija        : ja        (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ijae       : jae       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ijb        : jb        (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ijbe       : jbe       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ijg        : jg        (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ijge       : jge       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ijl        : jl        (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ijle       : jle       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ijmp       : jmp       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ijns       : jns       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ijnz       : jnz       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ijs        : js        (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ijz        : jz        (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ilea       : lea       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ileave     : leave     (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Imov       : mov       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Imovd      : movd      (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Imovdqu    : movdqu    (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Imovq      : movq      (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Imovqa     : movqa     (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Imovsd     : movsd     (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Imovsq     : movsq     (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Imovsx     : movsx     (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Imovsxd    : movsxd    (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Imovzx     : movzx     (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Imul       : mul       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Inop       : nop       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Inot       : Not       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ineg       : neg       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ior        : Or        (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ipcmpeqb   : pcmpeqb   (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ipmovmskb  : pmovmskb  (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ipop       : pop       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ipshufd    : pshufd    (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ipunpcklbw : punpcklbw (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ipush      : push      (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ipxor      : pxor      (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Iret       : ret       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Irol       : rol       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Iror       : ror       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Isar       : sar       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Isbb       : sbb       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Iscasb     : scasb     (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Iseta      : seta      (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Isetg      : setg      (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Isetl      : setl      (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Isetle     : setle     (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Isetnz     : setnz     (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Isetz      : setz      (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ishl       : shl       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ishld      : shl       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ishr       : shr       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Istd       : std       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Istosd     : stosd     (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Isub       : sub       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Isyscall   : syscall   (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Itest      : test      (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ixor       : Xor       (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        default :
            std::stringstream ss;
            ss << "unhandled instruction [" << ud_insn_hex(&ud_obj) << " => "
            <<  ud_insn_asm(&ud_obj) << "]" << " " << ins_debug_str(&ud_obj);
            std::cerr << ss.str() << std::endl;
            throw std::runtime_error(ss.str());
        }

        if (ud_obj.pfx_rep) {
            std::cout << "pfx_rep" << std::endl;
            if (ud_obj.pfx_rep == UD_Irep) {
                switch (ud_obj.mnemonic) {
                case UD_Istosd :
                case UD_Imovsq : rep (&ud_obj, address + ud_insn_off(&ud_obj)); break;
                case UD_Iret   : break; // stupid amd branch predictor bug workaround
                default :
                    throw std::runtime_error("unhandled rep prefix");
                }
            }
            else if (ud_obj.pfx_rep == UD_Irepne) {
                std::cout << "repne" << std::endl;
                switch (ud_obj.mnemonic) {
                    case UD_Iscasb : repne(&ud_obj, address + ud_insn_off(&ud_obj)); break;
                default :
                    throw std::runtime_error("unhandled repne prefix");
                }
            }
        } 
    }
    else throw std::runtime_error("unable to disassemble instruction");
    
    return instructions;
}


int Translator :: register_bits (int reg)
{
    if ((reg >= UD_R_AL)  && (reg <= UD_R_R15B))    return 8;
    if ((reg >= UD_R_AX)  && (reg <= UD_R_R15W))    return 16;
    if ((reg >= UD_R_EAX) && (reg <= UD_R_R15D))    return 32;
    if ((reg >= UD_R_RAX) && (reg <= UD_R_R15))     return 64;
    if ((reg == UD_R_RIP) || (reg == UD_R_FS))      return 64;
    if ((reg >= UD_R_XMM0) && (reg <= UD_R_XMM15)) return 128;
    
    throw std::runtime_error("invalid register for register_bits");
    return -1;
}

int Translator :: register_to64 (int reg)
{
    if ((reg >= UD_R_AL) && (reg <= UD_R_R15B)) {
        switch (reg) {
        case UD_R_AL  : return UD_R_RAX;
        case UD_R_BL  : return UD_R_RBX;
        case UD_R_CL  : return UD_R_RCX;
        case UD_R_DL  : return UD_R_RDX;
        case UD_R_SPL : return UD_R_RSP;
        case UD_R_BPL : return UD_R_RBP;
        case UD_R_SP  : return UD_R_RSP;
        case UD_R_BP  : return UD_R_RBP;
        case UD_R_SIL : return UD_R_RSI;
        case UD_R_DIL : return UD_R_RDI;
        case UD_R_R8B : return UD_R_R8;
        case UD_R_R9B : return UD_R_R9;
        case UD_R_R10B : return UD_R_R10;
        case UD_R_R11B : return UD_R_R11;
        case UD_R_R12B : return UD_R_R12;
        case UD_R_R13B : return UD_R_R13;
        case UD_R_R14B : return UD_R_R14;
        case UD_R_R15B : return UD_R_R15;
        }
    }
    else if ((reg >= UD_R_AX) && (reg <= UD_R_R15W))
        return reg + (UD_R_RAX - UD_R_AX);
    else if ((reg >= UD_R_EAX) && (reg <= UD_R_R15D))
        return reg + (UD_R_RAX - UD_R_EAX);
    else if ((reg >= UD_R_RAX) && (reg <= UD_R_R15))
        return reg;
    else if ((reg >= UD_R_XMM0) && (reg <= UD_R_XMM15))
        return reg;
    else if ((reg == UD_R_FS) || (reg == UD_R_RIP)) return reg;

    throw std::runtime_error(std::string("")
                             + "register_to64 called on unsupported register: " 
                             + ud_type_DEBUG[reg]);
    return -1;
}

std::string register128_name (int reg, bool low)
{
    if ((reg >= UD_R_XMM0) && (reg <= UD_R_XMM15)) {
        std::string name = ud_type_DEBUG[reg];
        if (low)
            name += "_low";
        else
            name += "_high";
        return name;
    }
    throw std::runtime_error("invalid register for register128_name");
}


void Translator :: operand_set (ud_t * ud_obj, int operand_i, uint64_t address, InstructionOperand & value)
{
    size_t size = ud_insn_len(ud_obj);

    if (ud_obj->operand[operand_i].type == UD_OP_MEM) {
        InstructionOperand lhs = operand(ud_obj, operand_i, address);
        instructions.push_back(new InstructionStore(address,
                                                    ud_insn_len(ud_obj),
                                                    ud_obj->operand[operand_i].size,
                                                    lhs,
                                                    value));
    }
    // register assignments always take place on the full size of the register
    else if (ud_obj->operand[operand_i].type == UD_OP_REG) {
        // special case for special AH/BH etc registers
        switch (ud_obj->operand[operand_i].base) {
        case UD_R_AH :
        case UD_R_CH :
            InstructionOperand fullreg;
            if (ud_obj->operand[operand_i].base == UD_R_AH)
                fullreg = InstructionOperand(OPTYPE_VAR, 64, "UD_R_RAX");
            else if (ud_obj->operand[operand_i].base == UD_R_CH)
                fullreg = InstructionOperand(OPTYPE_VAR, 64, "UD_R_RCX");
            // move value into appropriate place
            InstructionOperand tmpValue (OPTYPE_VAR, 64);
            InstructionOperand eight    (OPTYPE_CONSTANT, 8, 8);
            instructions.push_back(new InstructionAssign(address, size, tmpValue, value));
            instructions.push_back(new InstructionShl(address, size, tmpValue, tmpValue, eight));
            // zero out appropriate spot in destination register
            InstructionOperand ff00    (OPTYPE_CONSTANT, 64, 0xffffffffffff00ffULL);
            instructions.push_back(new InstructionAnd(address, size, fullreg, fullreg, ff00));
            // or value with rax
            instructions.push_back(new InstructionOr(address, size, fullreg, fullreg, tmpValue));
            return;
        }

        // setting an AL/AX type register sets only the lower bits
        if (    (register_bits(ud_obj->operand[operand_i].base) == 8)
             || (register_bits(ud_obj->operand[operand_i].base) == 16)) {
            // this should handle registers larger than 64 bits appropriately
            int value_bits = value.g_bits() < 8 ? 8 : value.g_bits();
            int base       = register_to64(ud_obj->operand[operand_i].base);
            InstructionOperand dst (OPTYPE_VAR, register_bits(base), ud_type_DEBUG[base]);
            InstructionOperand one  (OPTYPE_CONSTANT, dst.g_bits(), 1);
            InstructionOperand bits (OPTYPE_CONSTANT, 64, value_bits);
            InstructionOperand mask (OPTYPE_VAR, register_bits(base));
            InstructionOperand tmp  (OPTYPE_VAR, dst.g_bits());
            instructions.push_back(new InstructionShl(address, size, mask, one, bits));
            instructions.push_back(new InstructionSub(address, size, mask, mask, one));
            instructions.push_back(new InstructionNot(address, size, mask, mask));
            instructions.push_back(new InstructionAnd(address, size, tmp, dst, mask));
            instructions.push_back(new InstructionOr(address, size, dst, tmp, value));
            return;
        }

        InstructionOperand dst (OPTYPE_VAR,
                                register_bits(ud_obj->operand[operand_i].base),
                                ud_type_DEBUG[register_to64(ud_obj->operand[operand_i].base)]);
        instructions.push_back(new InstructionAssign(address, size, dst, value));
    }
}


InstructionOperand Translator :: operand_load (ud_t * ud_obj,    int operand_i,
                                               uint64_t address, int bits)
{
    size_t size = ud_insn_len(ud_obj);

    InstructionOperand addr = operand(ud_obj, operand_i, address);
    InstructionOperand loadResult (OPTYPE_VAR, bits);
    if (bits == 128) {
        // load least-significant 64-bits
        instructions.push_back(new InstructionLoad(address, size, 64, loadResult, addr));
        // load most-significant 64-bits
        InstructionOperand tmp     (OPTYPE_VAR, 128);
        InstructionOperand tmpAddr (OPTYPE_VAR, 64);
        InstructionOperand eight   (OPTYPE_CONSTANT, 8, 8);
        instructions.push_back(new InstructionAdd(address, size, tmpAddr, addr, eight));
        instructions.push_back(new InstructionLoad(address, size, 64, tmp, tmpAddr));
        // shift msbytes into place
        InstructionOperand shift   (OPTYPE_CONSTANT, 8, 64);
        instructions.push_back(new InstructionShl(address, size, tmp, tmp, shift));
        // or LSBytes and MSBytes into dst
        instructions.push_back(new InstructionOr(address, size, loadResult, loadResult, tmp));
    }
    else
        instructions.push_back(new InstructionLoad(address, size, bits, loadResult, addr));
    return loadResult;
}


InstructionOperand Translator :: operand_get (ud_t * ud_obj, int operand_i, uint64_t address)
{
    InstructionOperand result = operand(ud_obj, operand_i, address);

    if (ud_obj->operand[operand_i].type == UD_OP_MEM) {
        return operand_load(ud_obj, operand_i, address, ud_obj->operand[operand_i].size);
    }

    return result;
}


uint64_t Translator :: operand_lval (int bits, struct ud_operand operand)
{
    switch (bits) {
    case 8  : return (uint64_t) operand.lval.ubyte;
    case 16 : return (uint64_t) operand.lval.uword;
    case 32 : return (uint64_t) operand.lval.udword;
    case 64 : return (uint64_t) operand.lval.uqword;
    }
    
    std::stringstream ss;
    ss << "invalid bit value passed to operand_lval: " << bits;
    throw std::runtime_error(ss.str());
    
    return 0;
}


InstructionOperand Translator :: operand (ud_t * ud_obj, int operand_i, uint64_t address)
{
    struct ud_operand operand = ud_obj->operand[operand_i];
    size_t size = ud_insn_len(ud_obj);

    if (operand.type == UD_OP_REG) {
        // special cases for annoying AH, BH... etc registers
        switch (ud_obj->operand[operand_i].base) {
        case UD_R_AH :
        case UD_R_CH :
            InstructionOperand fullreg;
            if (ud_obj->operand[operand_i].base == UD_R_AH)
                fullreg = InstructionOperand(OPTYPE_VAR, 64, "UD_R_RAX");
            else if (ud_obj->operand[operand_i].base == UD_R_CH)
                fullreg = InstructionOperand(OPTYPE_VAR, 64, "UD_R_RCX");
            InstructionOperand rh    (OPTYPE_VAR, 8);
            InstructionOperand eight (OPTYPE_CONSTANT, 8, 8);
            instructions.push_back(new InstructionShr(address, size, rh, fullreg, eight));
            return rh;
        }
        // get the register's name
        std::string name = ud_type_DEBUG[register_to64(operand.base)];
        return InstructionOperand(OPTYPE_VAR, register_bits(operand.base), name);
    }
    
    else if ((operand.type == UD_OP_MEM) && (ud_obj->pfx_seg)) {

        std::string name = ud_type_DEBUG[register_to64(ud_obj->pfx_seg)];
        InstructionOperand seg(OPTYPE_VAR, register_bits(ud_obj->pfx_seg), name);

        if (operand.offset) {
            InstructionOperand offset(OPTYPE_CONSTANT,
                                      operand.offset,
                                      operand_lval(operand.offset, operand));

            InstructionOperand result(OPTYPE_VAR, 64);
            instructions.push_back(new InstructionAdd(address, size, result, seg, offset));
            return result;
        }
        else if (operand.base) {

            std::string name = ud_type_DEBUG[register_to64(operand.base)];
            InstructionOperand base(OPTYPE_VAR, register_bits(operand.base), name);

            InstructionOperand result(OPTYPE_VAR, 64);
            instructions.push_back(new InstructionAdd(address, size, result, seg, base));
            return result;
        }
        return seg;
    }
    else if (operand.type == UD_OP_MEM) {
        InstructionOperand base;
        InstructionOperand index;
        InstructionOperand scale;
        InstructionOperand displ;
        size_t size = ud_insn_len(ud_obj);

        if (operand.base) {
            std::string name = ud_type_DEBUG[register_to64(operand.base)];
            base = InstructionOperand(OPTYPE_VAR, register_bits(operand.base), name);
        }
        if (operand.index) {
            std::string name = ud_type_DEBUG[register_to64(operand.index)];
            index = InstructionOperand(OPTYPE_VAR, register_bits(operand.index), name);
        }
        if (operand.scale) {
            scale = InstructionOperand(OPTYPE_CONSTANT, 8, operand.scale);
        }
        if (operand.offset) {
            displ = InstructionOperand(OPTYPE_CONSTANT, operand.offset,
                                       operand_lval(operand.offset, operand));
        }

        InstructionOperand index_scale = index;
        if (operand.index && operand.scale) {
            index_scale = InstructionOperand(OPTYPE_VAR, 64);
            instructions.push_back(new InstructionMul(address, size, index_scale, index, scale));
        }
        
        InstructionOperand base_displacement = base;
        if (operand.base && operand.offset) {
            base_displacement = InstructionOperand(OPTYPE_VAR, register_bits(operand.base));
            instructions.push_back(new InstructionSignExtend(address,           size,
                                                             base_displacement, displ));
            instructions.push_back(new InstructionAdd(address, size, base_displacement,
                                                      base,    base_displacement));
        }
        else if ((! operand.base) && (operand.offset))
            base_displacement = displ;
        
        if (operand.index) {
            InstructionOperand result = InstructionOperand(OPTYPE_VAR, 64);
            instructions.push_back(new InstructionAdd(address, size, result, 
                                                      index_scale, base_displacement));
            return result;
        }
        
        return base_displacement;
    }
    else if (operand.type == UD_OP_CONST)
        return InstructionOperand(OPTYPE_CONSTANT, operand.size, operand_lval(operand.size, operand));
    else if (operand.type == UD_OP_JIMM)
        return InstructionOperand(OPTYPE_CONSTANT, operand.size, operand_lval(operand.size, operand));
    else if (operand.type == UD_OP_IMM)
        return InstructionOperand(OPTYPE_CONSTANT, operand.size, operand_lval(operand.size, operand));
    else
        throw std::runtime_error("unsupported operand type " + ins_debug_str(ud_obj));
    
    return InstructionOperand();
}


void Translator :: cmovcc (ud_t * ud_obj, uint64_t address, InstructionOperand cond)
{

    size_t size = ud_insn_len(ud_obj);

    InstructionOperand dst = operand_get(ud_obj, 0, address);
    InstructionOperand src = operand_get(ud_obj, 1, address);
    InstructionOperand tmp (OPTYPE_VAR, dst.g_bits());
    InstructionOperand notCond (OPTYPE_VAR, 1);

    instructions.push_back(new InstructionNot(address, size, notCond, cond));
    instructions.push_back(new InstructionMul(address, size, dst, dst, notCond));
    instructions.push_back(new InstructionMul(address, size, tmp, src, cond));
    instructions.push_back(new InstructionOr(address, size, dst, dst, tmp));

    operand_set(ud_obj, 0, address, dst);
}


void Translator :: jcc (ud_t * ud_obj, uint64_t address, InstructionOperand cond)
{
    size_t size = ud_insn_len(ud_obj);
    InstructionOperand dst = operand_get(ud_obj, 0, address);

    if (ud_obj->operand[0].type == UD_OP_JIMM) {
        InstructionOperand rip(OPTYPE_VAR, 64, "UD_R_RIP");
        InstructionOperand tmp(OPTYPE_VAR, 64);
        instructions.push_back(new InstructionSignExtend(address, size, tmp, dst));
        instructions.push_back(new InstructionAdd(address, size, tmp, rip, tmp));
        instructions.push_back(new InstructionBrc(address, size, cond, tmp));
    }
    else instructions.push_back(new InstructionBrc(address, size, cond, dst));
}


void Translator :: adc (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    InstructionOperand lhs = operand_get(ud_obj, 0, address);
    InstructionOperand rhs = operand_get(ud_obj, 1, address);
    InstructionOperand tmp (OPTYPE_VAR, lhs.g_bits());
        
    InstructionOperand OFTmp    (OPTYPE_VAR, lhs.g_bits());
    InstructionOperand OFTmpShl (OPTYPE_CONSTANT, lhs.g_bits(), lhs.g_bits() - 1);
    InstructionOperand OF       (OPTYPE_VAR, 1, "OF"); // signed overflow
    InstructionOperand CF       (OPTYPE_VAR, 1, "CF"); // unsigned overflow
    InstructionOperand ZF       (OPTYPE_VAR, 1, "ZF");
    InstructionOperand SF       (OPTYPE_VAR, 1, "SF"); // "negative" flag
    InstructionOperand zero     (OPTYPE_CONSTANT, tmp.g_bits(), 0);
    
    instructions.push_back(new InstructionAdd(address, size, tmp, lhs, rhs));
    instructions.push_back(new InstructionAdd(address, size, tmp, tmp, CF));
    
    instructions.push_back(new InstructionCmpLtu(address, size, CF, tmp, lhs));
    instructions.push_back(new InstructionCmpEq (address, size, ZF, tmp, zero));
    instructions.push_back(new InstructionCmpLts(address, size, SF, tmp, zero));
    // OF is calculated based on the RREIL paper
    // http://www2.in.tum.de/bib/files/sepp11precise.pdf
    InstructionOperand SFxorOF(OPTYPE_VAR, 1);
    instructions.push_back(new InstructionCmpLts(address, size, SFxorOF, lhs, rhs));
    instructions.push_back(new InstructionXor(address, size, OF, SFxorOF, SF));
    
    operand_set(ud_obj, 0, address, tmp);
}


void Translator :: add (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    InstructionOperand lhs = operand_get(ud_obj, 0, address);
    InstructionOperand rhs = operand_get(ud_obj, 1, address);
    InstructionOperand tmp (OPTYPE_VAR, lhs.g_bits());
        
    InstructionOperand OF       (OPTYPE_VAR, 1, "OF"); // signed overflow
    InstructionOperand CF       (OPTYPE_VAR, 1, "CF"); // unsigned overflow
    InstructionOperand ZF       (OPTYPE_VAR, 1, "ZF");
    InstructionOperand SF       (OPTYPE_VAR, 1, "SF"); // "negative" flag
    InstructionOperand zero     (OPTYPE_CONSTANT, tmp.g_bits(), 0);
    
    instructions.push_back(new InstructionAdd(address, ud_insn_len(ud_obj), tmp, lhs, rhs));
    
    instructions.push_back(new InstructionCmpLtu(address, size, CF, tmp, lhs));
    instructions.push_back(new InstructionCmpEq (address, size, ZF, tmp, zero));
    instructions.push_back(new InstructionCmpLts(address, size, SF, tmp, zero));
    // OF is calculated based on the RREIL paper
    // http://www2.in.tum.de/bib/files/sepp11precise.pdf
    InstructionOperand SFxorOF(OPTYPE_VAR, 1);
    instructions.push_back(new InstructionCmpLts(address, size, SFxorOF, lhs, rhs));
    instructions.push_back(new InstructionXor(address, size, OF, SFxorOF, SF));
    
    operand_set(ud_obj, 0, address, tmp);
}

void Translator :: And (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    InstructionOperand lhs  = operand_get(ud_obj, 0, address);
    InstructionOperand rhs  = operand_get(ud_obj, 1, address);
    InstructionOperand tmp (OPTYPE_VAR, lhs.g_bits());
    
    InstructionOperand OF   (OPTYPE_VAR, 1, "OF"); // signed overflow
    InstructionOperand CF   (OPTYPE_VAR, 1, "CF"); // unsigned overflow
    InstructionOperand ZF   (OPTYPE_VAR, 1, "ZF");
    InstructionOperand SF   (OPTYPE_VAR, 1, "SF"); // "negative" flag
    InstructionOperand zero (OPTYPE_CONSTANT, tmp.g_bits(), 0);

    // 8-bit immediates are always sign extended
    if ((ud_obj->operand[1].type == UD_OP_IMM) && (rhs.g_bits() == 8)) {
        InstructionOperand rhs_old = rhs;
        rhs = InstructionOperand(OPTYPE_VAR, 64);
        instructions.push_back(new InstructionSignExtend(address, size, rhs, rhs_old));
    }
    
    instructions.push_back(new InstructionAnd(address, size, tmp, lhs, rhs));
    
    instructions.push_back(new InstructionAssign(address, size, OF, zero));
    instructions.push_back(new InstructionAssign(address, size, CF, zero));
    instructions.push_back(new InstructionCmpEq (address, size, ZF, tmp, zero));
    instructions.push_back(new InstructionCmpLts(address, size, SF, tmp, zero));
    
    operand_set(ud_obj, 0, address, tmp);
}

// destination must be register according to intel docs
void Translator :: bsf (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    InstructionOperand dst = operand_get(ud_obj, 0, address);
    InstructionOperand src = operand_get(ud_obj, 1, address);

    InstructionOperand result     (OPTYPE_VAR, dst.g_bits());
    InstructionOperand one        (OPTYPE_CONSTANT, src.g_bits(), 1);
    InstructionOperand result_set (OPTYPE_VAR, 1);
    InstructionOperand result_set_inverse (OPTYPE_VAR, 1);
    InstructionOperand zero       (OPTYPE_CONSTANT, src.g_bits(), 0);
    InstructionOperand bit        (OPTYPE_VAR, 1);
    InstructionOperand bitLocMul  (OPTYPE_VAR, 8);

    instructions.push_back(new InstructionAssign(address, size, result, zero));
    instructions.push_back(new InstructionAssign(address, size, result_set, zero));
    for (int i = 0; i < src.g_bits(); i++) {
        // get bit for this location
        InstructionOperand bitLoc (OPTYPE_CONSTANT, 8, i);
        instructions.push_back(new InstructionShr(address, size, bit, src, bitLoc));
        instructions.push_back(new InstructionAnd(address, size, bit, bit, one));
        // multiply location by bit, setting bitLocMul to 0 if bit not set
        instructions.push_back(new InstructionMul(address, size, bitLocMul, bitLoc, bit));
        // if result_set is not set, set result to the multiplied location
        instructions.push_back(new InstructionNot(address, size, result_set_inverse, result_set));
        instructions.push_back(new InstructionMul(address,   size, bitLocMul,
                                                  bitLocMul, result_set_inverse));
        instructions.push_back(new InstructionOr(address, size, result, result, bitLocMul));
        // set result_set if not set yet
        instructions.push_back(new InstructionOr(address, size, result_set, result_set, bit));
    }

    instructions.push_back(new InstructionAssign(address, size, dst, result));
}

void Translator :: bt (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    if (ud_obj->operand[0].type != UD_OP_REG)
        throw std::runtime_error("bt only supported on type reg");

    InstructionOperand haystack = operand_get(ud_obj, 0, address);
    InstructionOperand needle   = operand_get(ud_obj, 1, address);
    InstructionOperand CF         (OPTYPE_VAR, 1, "CF");

    // do it now so we don't have to later when we support mem operands
    if (ud_obj->operand[0].type == UD_OP_REG) {
        InstructionOperand mask (OPTYPE_CONSTANT, 8, register_bits(ud_obj->operand[0].base) - 1);
        InstructionOperand needleTmp (OPTYPE_VAR, 8);
        instructions.push_back(new InstructionAnd(address, size, needleTmp, needle, mask));
        instructions.push_back(new InstructionShr(address, size, CF, haystack, needleTmp));
    }
}

void Translator :: call (ud_t * ud_obj, uint64_t address)
{
    
    InstructionOperand rip     = InstructionOperand(OPTYPE_VAR, 64, "UD_R_RIP");
    InstructionOperand rsp     = InstructionOperand(OPTYPE_VAR, 64, "UD_R_RSP");
    InstructionOperand subsize = InstructionOperand(OPTYPE_CONSTANT, 8, STACK_ELEMENT_SIZE);

    // we must get the dst before messing with rsp for call [rsp+offset]
    InstructionOperand dst = operand_get(ud_obj, 0, address);
    
    // push RIP
    instructions.push_back(new InstructionSub(address, ud_insn_len(ud_obj), rsp, rsp, subsize));
    instructions.push_back(new InstructionStore(address, ud_insn_len(ud_obj), 64, rsp, rip));

    // set RIP += offset
    InstructionOperand off = InstructionOperand(OPTYPE_VAR, 64);
    InstructionOperand one = InstructionOperand(OPTYPE_CONSTANT, 1, 1);

    if (ud_obj->operand[0].type == UD_OP_JIMM) {
        instructions.push_back(new InstructionSignExtend(address, ud_insn_len(ud_obj), off, dst));
        instructions.push_back(new InstructionAdd(address, ud_insn_len(ud_obj), off, rip, off));
        instructions.push_back(new InstructionBrc(address, ud_insn_len(ud_obj), one, off));
    }
    else {
        instructions.push_back(new InstructionBrc(address, ud_insn_len(ud_obj), one, dst));
    }
}

void Translator :: cdqe (ud_t * ud_obj, uint64_t address)
{
    InstructionOperand rax (OPTYPE_VAR, 64, "UD_R_RAX");
    InstructionOperand eax (OPTYPE_VAR, 32, "UD_R_RAX");

    instructions.push_back(new InstructionSignExtend(address, ud_insn_len(ud_obj), rax, eax));
}


void Translator :: cmova (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    InstructionOperand ZF         (OPTYPE_VAR, 1, "ZF");
    InstructionOperand notZF      (OPTYPE_VAR, 1);
    InstructionOperand CF         (OPTYPE_VAR, 1, "CF");
    InstructionOperand notCF      (OPTYPE_VAR, 1);
    InstructionOperand notCFandnotZF (OPTYPE_VAR, 1);

    instructions.push_back(new InstructionNot(address, size, notZF, ZF));
    instructions.push_back(new InstructionNot(address, size, notCF, CF));
    instructions.push_back(new InstructionAnd(address, size, notCFandnotZF, notZF, notCF));

    cmovcc(ud_obj, address, notCFandnotZF);
}


void Translator :: cmovb (ud_t * ud_obj, uint64_t address)
{
    InstructionOperand CF (OPTYPE_VAR, 1, "CF");

    cmovcc(ud_obj, address, CF);
}


void Translator :: cmovbe (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    InstructionOperand ZF        (OPTYPE_VAR, 1, "ZF");
    InstructionOperand CF        (OPTYPE_VAR, 1, "CF");
    InstructionOperand CForZF    (OPTYPE_VAR, 1);

    instructions.push_back(new InstructionOr(address, size, CForZF, CF, ZF));

    cmovcc(ud_obj, address, CForZF);
}


void Translator :: cmovnz (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    InstructionOperand ZF    (OPTYPE_VAR, 1, "ZF");
    InstructionOperand notZF (OPTYPE_VAR, 1);

    instructions.push_back(new InstructionNot(address, size, notZF, ZF));

    cmovcc(ud_obj, address, notZF);
}


void Translator :: cmovs (ud_t * ud_obj, uint64_t address)
{
    InstructionOperand SF (OPTYPE_VAR, 1, "SF");

    cmovcc(ud_obj, address, SF);
}


void Translator :: cmovz (ud_t * ud_obj, uint64_t address)
{
    InstructionOperand ZF (OPTYPE_VAR, 1, "ZF");

    cmovcc(ud_obj, address, ZF);
}


void Translator :: cmp (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);
    InstructionOperand lhs = operand_get(ud_obj, 0, address);
    InstructionOperand rhs = operand_get(ud_obj, 1, address);
    
    InstructionOperand CF          (OPTYPE_VAR, 1, "CF");
    InstructionOperand CForZF      (OPTYPE_VAR, 1, "CForZF");
    InstructionOperand SFxorOF     (OPTYPE_VAR, 1, "SFxorOF");
    InstructionOperand SFxorOForZF (OPTYPE_VAR, 1, "SFxorOForZF");
    InstructionOperand ZF          (OPTYPE_VAR, 1, "ZF");
    InstructionOperand OF          (OPTYPE_VAR, 1, "OF");
    InstructionOperand SF          (OPTYPE_VAR, 1, "SF");
    InstructionOperand tmp0        (OPTYPE_VAR, lhs.g_bits());
    InstructionOperand tmp0s       (OPTYPE_CONSTANT, tmp0.g_bits(), 0);

    InstructionOperand sext (OPTYPE_VAR, lhs.g_bits());
    if (ud_obj->operand[1].type == UD_OP_IMM) {
        instructions.push_back(new InstructionSignExtend(address, size, sext, rhs));
    }
    else
        sext = rhs;
    
    instructions.push_back(new InstructionCmpLtu(address, size, CF,          lhs, sext));
    instructions.push_back(new InstructionCmpLeu(address, size, CForZF,      lhs, sext));
    instructions.push_back(new InstructionCmpLts(address, size, SFxorOF,     lhs, sext));
    instructions.push_back(new InstructionCmpLes(address, size, SFxorOForZF, lhs, sext));
    instructions.push_back(new InstructionCmpEq (address, size, ZF,          lhs, sext));
    instructions.push_back(new InstructionSub   (address, size, tmp0,        lhs, sext));
    instructions.push_back(new InstructionCmpLts(address, size, SF,          tmp0, tmp0s));
    instructions.push_back(new InstructionXor   (address, size, OF,          SFxorOF, SF));
}


void Translator :: cmpxchg (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    InstructionOperand dst = operand_get(ud_obj, 0, address);
    InstructionOperand src = operand_get(ud_obj, 1, address);

    // compare dst with appropriately sized rax
    InstructionOperand rax (OPTYPE_VAR, 64, "UD_R_RAX");
    InstructionOperand raxCmp (OPTYPE_VAR, 1);
    InstructionOperand raxTmp (OPTYPE_VAR, 64);
    if (dst.g_bits() < 64) {
        uint64_t rax_mask_ = (1 << dst.g_bits()) - 1;
        InstructionOperand raxMask (OPTYPE_CONSTANT, 64, rax_mask_);
        instructions.push_back(new InstructionAnd(address, size, raxTmp, rax, raxMask));
        instructions.push_back(new InstructionCmpEq(address, size, raxCmp, raxTmp, dst));
    }
    else
        instructions.push_back(new InstructionCmpEq(address, size, raxCmp, rax, dst));

    InstructionOperand notRaxCmp (OPTYPE_VAR, 1);
    instructions.push_back(new InstructionNot(address, size, notRaxCmp, raxCmp));

    // set future value of dst
    InstructionOperand futureDst (OPTYPE_VAR, dst.g_bits());
    InstructionOperand futureDstTmp (OPTYPE_VAR, dst.g_bits());
    instructions.push_back(new InstructionMul(address, size, futureDstTmp, src, raxCmp));
    instructions.push_back(new InstructionMul(address, size, futureDst, dst, notRaxCmp));
    instructions.push_back(new InstructionOr(address, size, futureDst, futureDst, futureDstTmp));

    // set future value of rax and then set rax
    InstructionOperand futureRax (OPTYPE_VAR, 64);
    InstructionOperand futureRaxTmp (OPTYPE_VAR, 64);
    instructions.push_back(new InstructionMul(address, size, futureRax, rax, raxCmp));
    instructions.push_back(new InstructionMul(address, size, futureRaxTmp, src, notRaxCmp));
    instructions.push_back(new InstructionOr(address, size, rax, futureRax, futureRaxTmp));

    InstructionOperand ZF (OPTYPE_VAR, 1, "ZF");
    instructions.push_back(new InstructionAssign(address, size, ZF, raxCmp));

    // set dst
    operand_set(ud_obj, 0, address, futureDst);
}


void Translator :: dec (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    InstructionOperand dst = operand_get(ud_obj, 0, address);
    InstructionOperand one (OPTYPE_CONSTANT, dst.g_bits(), 1);
    InstructionOperand tmp (OPTYPE_VAR, dst.g_bits());

    instructions.push_back(new InstructionSub(address, size, tmp, dst, one));
        
    InstructionOperand OF (OPTYPE_VAR, 1, "OF");
    InstructionOperand ZF (OPTYPE_VAR, 1, "ZF");
    InstructionOperand SF (OPTYPE_VAR, 1, "SF");
    InstructionOperand SFxorOF (OPTYPE_VAR, 1);
    InstructionOperand zero (OPTYPE_CONSTANT, dst.g_bits(), 0);

    instructions.push_back(new InstructionCmpLts(address, size, SF, tmp, zero));
    instructions.push_back(new InstructionCmpEq(address, size, ZF, tmp, zero));
    instructions.push_back(new InstructionCmpLts(address, size, SFxorOF, tmp, dst));
    instructions.push_back(new InstructionXor(address, size, OF, SFxorOF, SF));
    
    operand_set(ud_obj, 0, address, tmp);
}


void Translator :: div (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);
    InstructionOperand divisor = operand_get(ud_obj, 0, address);

    if (divisor.g_bits() == 64) {
        InstructionOperand rax (OPTYPE_VAR, 64, "UD_R_RAX");
        InstructionOperand rdx (OPTYPE_VAR, 64, "UD_R_RDX");
        InstructionOperand sixfour   (OPTYPE_CONSTANT, 8, 64);
        InstructionOperand dividend  (OPTYPE_VAR, 128);
        InstructionOperand quotient  (OPTYPE_VAR, 64);
        InstructionOperand remainder (OPTYPE_VAR, 64);

        instructions.push_back(new InstructionShl(address, size, dividend, rdx, sixfour));
        instructions.push_back(new InstructionOr (address, size, dividend, dividend, rax));

        instructions.push_back(new InstructionDiv(address, size, quotient, dividend, divisor));
        instructions.push_back(new InstructionMod(address, size, remainder, dividend, divisor));

        instructions.push_back(new InstructionAssign(address, size, rax, quotient));
        instructions.push_back(new InstructionAssign(address, size, rdx, remainder));
    }
    else
        throw std::runtime_error("unsupported div bits");
}


void Translator :: hlt (ud_t * ud_obj, uint64_t address)
{
    instructions.push_back(new InstructionHlt(address, ud_insn_len(ud_obj)));
}


void Translator :: imul (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    InstructionOperand dst;
    InstructionOperand srca;
    InstructionOperand srcb;
    // three different imuls for one/two/three operands
    if (    (ud_obj->operand[0].type != UD_NONE)
         && (ud_obj->operand[1].type != UD_NONE)
         && (ud_obj->operand[2].type != UD_NONE)) {
        dst = operand_get(ud_obj, 0, address);
        srca = operand_get(ud_obj, 1, address);
        srcb = operand_get(ud_obj, 2, address);
    }
    else if ((ud_obj->operand[1].type != UD_NONE) && (ud_obj->operand[2].type == UD_NONE)) {
        // dst must be register
        dst  = operand_get(ud_obj, 0, address);
        srca = operand_get(ud_obj, 0, address);
        srcb = operand_get(ud_obj, 1, address);
    }
    else {
        std::cerr << ins_debug_str(ud_obj) << std::endl;
        throw std::runtime_error("unsupported imul, only 2 operand imul supported");
    }

    InstructionOperand srcaSext (OPTYPE_VAR, dst.g_bits());
    InstructionOperand srcbSext (OPTYPE_VAR, dst.g_bits());

    instructions.push_back(new InstructionSignExtend(address, size, srcaSext, srca));
    instructions.push_back(new InstructionSignExtend(address, size, srcbSext, srcb));

    InstructionOperand tmp (OPTYPE_VAR, 128);
    instructions.push_back(new InstructionMul(address, size, tmp, srca, srcb));
    instructions.push_back(new InstructionMul(address, size, dst, srca, srcb));

    InstructionOperand sixfour(OPTYPE_CONSTANT, 8, 64);
    InstructionOperand highBits(OPTYPE_VAR, 64);
    instructions.push_back(new InstructionShr(address, size, highBits, tmp, sixfour));

    InstructionOperand OF (OPTYPE_VAR, 1, "OF");
    InstructionOperand CF (OPTYPE_VAR, 1, "CF");
    InstructionOperand zero (OPTYPE_CONSTANT, 64, 0);
    instructions.push_back(new InstructionCmpEq(address, size, OF, highBits, zero));
    instructions.push_back(new InstructionNot(address, size, OF, OF));
    instructions.push_back(new InstructionAssign(address, size, CF, OF));

    InstructionOperand SF (OPTYPE_VAR, 1, "SF");
    instructions.push_back(new InstructionCmpLts(address, size, SF, dst, zero));

    InstructionOperand ZF (OPTYPE_VAR, 1, "ZF");
    instructions.push_back(new InstructionCmpEq(address, size, ZF, dst, zero));
}


void Translator :: inc (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    InstructionOperand dst = operand_get(ud_obj, 0, address);
    InstructionOperand tmp (OPTYPE_VAR, dst.g_bits());
    InstructionOperand one (OPTYPE_CONSTANT, dst.g_bits(), 1);

    instructions.push_back(new InstructionAdd(address, size, tmp, dst, one));

    InstructionOperand OF (OPTYPE_VAR, 1, "OF");
    InstructionOperand ZF (OPTYPE_VAR, 1, "ZF");
    InstructionOperand SF (OPTYPE_VAR, 1, "SF");
    InstructionOperand SFxorOF (OPTYPE_VAR, 1);
    InstructionOperand zero (OPTYPE_CONSTANT, dst.g_bits(), 0);

    instructions.push_back(new InstructionCmpLts(address, size, SF, tmp, zero));
    instructions.push_back(new InstructionCmpEq(address, size, ZF, tmp, zero));
    instructions.push_back(new InstructionCmpLts(address, size, SFxorOF, tmp, dst));
    instructions.push_back(new InstructionXor(address, size, OF, SFxorOF, SF));

    operand_set(ud_obj, 0, address, tmp);
}


void Translator :: ja (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);
    
    InstructionOperand CF        (OPTYPE_VAR, 1, "CF");
    InstructionOperand ZF        (OPTYPE_VAR, 1, "ZF");
    InstructionOperand CForZF    (OPTYPE_VAR, 1);
    InstructionOperand notCForZF (OPTYPE_VAR, 1);

    instructions.push_back(new InstructionOr(address, size, CForZF, CF, ZF));
    instructions.push_back(new InstructionNot(address, size, notCForZF, CForZF));

    jcc(ud_obj, address, notCForZF);
}


void Translator :: jae (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);
    
    InstructionOperand CF    (OPTYPE_VAR, 1, "CF");
    InstructionOperand notCF (OPTYPE_VAR, 1);

    instructions.push_back(new InstructionNot(address, size, notCF, CF));

    jcc(ud_obj, address, notCF);
}


void Translator :: jb (ud_t * ud_obj, uint64_t address)
{
    InstructionOperand CF (OPTYPE_VAR, 1, "CF");

    jcc(ud_obj, address, CF);
}


void Translator :: jbe (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);
    
    InstructionOperand CF        (OPTYPE_VAR, 1, "CF");
    InstructionOperand ZF        (OPTYPE_VAR, 1, "ZF");
    InstructionOperand CForZF    (OPTYPE_VAR, 1);

    instructions.push_back(new InstructionOr(address, size, CForZF, CF, ZF));

    jcc(ud_obj, address, CForZF);
}


void Translator :: jg (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    InstructionOperand SF     (OPTYPE_VAR, 1, "SF");
    InstructionOperand OF     (OPTYPE_VAR, 1, "OF");
    InstructionOperand SFeqOF (OPTYPE_VAR, 1);
    InstructionOperand ZF     (OPTYPE_VAR, 1, "ZF");
    InstructionOperand notZF  (OPTYPE_VAR, 1, "notZF");
    InstructionOperand notZFandSFeqOF (OPTYPE_VAR, 1);

    instructions.push_back(new InstructionCmpEq(address, size, SFeqOF, SF, OF));
    instructions.push_back(new InstructionNot  (address, size, notZF, ZF));
    instructions.push_back(new InstructionAnd  (address, size, notZFandSFeqOF, notZF, SFeqOF));

    jcc(ud_obj, address, notZFandSFeqOF);
}


void Translator :: jge (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    InstructionOperand SF     (OPTYPE_VAR, 1, "SF");
    InstructionOperand OF     (OPTYPE_VAR, 1, "OF");
    InstructionOperand SFeqOF (OPTYPE_VAR, 1);

    instructions.push_back(new InstructionCmpEq(address, size, SFeqOF, SF, OF));

    jcc(ud_obj, address, SFeqOF);
}


void Translator :: jl (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);
    InstructionOperand SFxorOF (OPTYPE_VAR, 1);
    InstructionOperand SF (OPTYPE_VAR, 1, "SF");
    InstructionOperand OF (OPTYPE_VAR, 1, "OF");

    instructions.push_back(new InstructionXor(address, size, SFxorOF, SF, OF));

    jcc(ud_obj, address, SFxorOF);
}


void Translator :: jle (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);
    InstructionOperand ZForSFxorOF (OPTYPE_VAR, 1);
    InstructionOperand ZF (OPTYPE_VAR, 1, "ZF");
    InstructionOperand SF (OPTYPE_VAR, 1, "SF");
    InstructionOperand OF (OPTYPE_VAR, 1, "OF");

    instructions.push_back(new InstructionXor(address, size, ZForSFxorOF, SF, OF));
    instructions.push_back(new InstructionOr(address, size, ZForSFxorOF, ZForSFxorOF, ZF));

    jcc(ud_obj, address, ZForSFxorOF);
}


// making this an InstructionBrc greatly simplifies analysis as all branches are
// either InstructionBrc or InstructionCall
void Translator :: jmp (ud_t * ud_obj, uint64_t address)
{
    InstructionOperand one (OPTYPE_CONSTANT, 1, 1);

    jcc(ud_obj, address, one);
}


void Translator :: jns (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);
    
    InstructionOperand SF    (OPTYPE_VAR,   1, "SF");
    InstructionOperand notSF (OPTYPE_VAR,   1, "notSF");

    instructions.push_back(new InstructionNot(address, size, notSF, SF));

    jcc(ud_obj, address, notSF);
}


void Translator :: jnz (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);
    
    InstructionOperand ZF    (OPTYPE_VAR,   1, "ZF");
    InstructionOperand notZF (OPTYPE_VAR,   1, "notZF");

    instructions.push_back(new InstructionNot(address, size, notZF, ZF));

    jcc(ud_obj, address, notZF);
}


void Translator :: js (ud_t * ud_obj, uint64_t address)
{
    InstructionOperand SF  (OPTYPE_VAR, 1, "SF");

    jcc(ud_obj, address, SF);
}


void Translator :: jz (ud_t * ud_obj, uint64_t address)
{
    InstructionOperand ZF  (OPTYPE_VAR, 1, "ZF");

    jcc(ud_obj, address, ZF);
}


void Translator :: lea (ud_t * ud_obj, uint64_t address)
{
    InstructionOperand dst = operand(ud_obj, 0, address);
    InstructionOperand src = operand(ud_obj, 1, address);
    
    instructions.push_back(new InstructionAssign(address, ud_insn_len(ud_obj), dst, src));
}


void Translator :: leave (ud_t * ud_obj, uint64_t address)
{
    InstructionOperand rsp   (OPTYPE_VAR, 64, "UD_R_RSP");
    InstructionOperand rbp   (OPTYPE_VAR, 64, "UD_R_RBP");
    InstructionOperand eight (OPTYPE_CONSTANT, 8, 8);
    
    instructions.push_back(new InstructionAssign(address, ud_insn_len(ud_obj), rsp, rbp));
    instructions.push_back(new InstructionLoad  (address, ud_insn_len(ud_obj), 64, rbp, rsp));
    instructions.push_back(new InstructionAdd   (address, ud_insn_len(ud_obj), rsp, rsp, eight));
}


void Translator :: mov (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    InstructionOperand src = operand_get(ud_obj, 1, address);

    // god help us all
    if (ud_obj->pfx_rex) {
        if (ud_insn_ptr(ud_obj)[1] == 0xc7) {
            InstructionOperand sext (OPTYPE_VAR, 64);
            instructions.push_back(new InstructionSignExtend(address, size, sext, src));
            operand_set(ud_obj, 0, address, sext);
            return;
        }
    }
    //else
        operand_set(ud_obj, 0, address, src);
}


void Translator :: movd (ud_t * ud_obj, uint64_t address)
{
    InstructionOperand src = operand_get(ud_obj, 1, address);
    operand_set(ud_obj, 0, address, src);
}


// memory only supports 64-bit loads and stores
void Translator :: movdqa (ud_t * ud_obj, uint64_t address)
{
    // currently we don't check for alignment and there is no difference
    // between this function and movdqu
    movdqu(ud_obj, address);
}


// memory only supports 64-bit loads and stores
void Translator :: movdqu (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    // 128-bit load
    if (ud_obj->operand[1].type == UD_OP_MEM) {
        InstructionOperand dst = operand(ud_obj, 0, address);
        InstructionOperand src = operand_load(ud_obj, 1, address, 128);
        instructions.push_back(new InstructionAssign(address, size, dst, src));
    }
    else {
        InstructionOperand dst = operand(ud_obj, 0, address);
        InstructionOperand src = operand(ud_obj, 1, address);
        instructions.push_back(new InstructionAssign(address, size, dst, src));
    }
}


void Translator :: movsd (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    if (ud_obj->operand[0].type == UD_NONE)
        throw std::runtime_error("movsd operand[0] == UD_NONE. IMPLEMENT THIS!");

    InstructionOperand dst = operand_get(ud_obj, 0, address);
    InstructionOperand src = operand_get(ud_obj, 1, address);

    instructions.push_back(new InstructionAssign(size, address, dst, src));
}


void Translator :: movsq (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    InstructionOperand rsi (OPTYPE_VAR, 64, "UD_R_RSI");
    InstructionOperand rdi (OPTYPE_VAR, 64, "UD_R_RDI");

    instructions.push_back(new InstructionStore(address, size, 64, rdi, rsi));

    InstructionOperand DF     (OPTYPE_VAR, 1, "DF");
    InstructionOperand neg16  (OPTYPE_CONSTANT, 64, -16);
    InstructionOperand eight  (OPTYPE_CONSTANT, 64, 8);
    InstructionOperand add    (OPTYPE_VAR, 64);
    InstructionOperand addTmp (OPTYPE_VAR, 64);
    instructions.push_back(new InstructionAssign(address, size, add, eight));
    instructions.push_back(new InstructionMul(address, size, addTmp, neg16, DF));
    instructions.push_back(new InstructionAdd(address, size, add, add, addTmp));
    instructions.push_back(new InstructionAdd(address, size, rdi, rdi, add));
    instructions.push_back(new InstructionAdd(address, size, rsi, rsi, add));
}


void Translator :: movsx (ud_t * ud_obj, uint64_t address)
{
    InstructionOperand dst = operand_get(ud_obj, 0, address);
    InstructionOperand src = operand_get(ud_obj, 1, address);

    instructions.push_back(new InstructionSignExtend(address, ud_insn_len(ud_obj), dst, src));

    operand_set(ud_obj, 0, address, dst);
}


// dst must be register
void Translator :: movsxd (ud_t * ud_obj, uint64_t address)
{
    // for this translator, movsxd is identical to movsx
    movsx(ud_obj, address);
}


void Translator :: movq (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);
    InstructionOperand dst = operand(ud_obj, 0, address);
    InstructionOperand src = operand(ud_obj, 1, address);

    if (ud_obj->operand[0].type == UD_OP_MEM)
        instructions.push_back(new InstructionStore(address, size, 64, dst, src));
    else if (ud_obj->operand[1].type == UD_OP_MEM) {
        src = operand_load(ud_obj, 1, address, 128);
        instructions.push_back(new InstructionAssign(address, size, dst, src));
    }
    else
        instructions.push_back(new InstructionAssign(address, size, dst, src));
}


void Translator :: movqa (ud_t * ud_obj, uint64_t address)
{
    // udis86 does not properly detect movdqa
    if (    (ud_insn_len(ud_obj) > 3)
         && (ud_insn_ptr(ud_obj)[0] = 0x66)
         && (ud_insn_ptr(ud_obj)[1] = 0x0f)
         && (ud_insn_ptr(ud_obj)[2] = 0x6f)) {
        movdqa(ud_obj, address);
    }
    else
        throw std::runtime_error(std::string("unsupported movqa: ")
                                 + ud_insn_hex(ud_obj));
}


// dst must be register, zero extend
void Translator :: movzx (ud_t * ud_obj, uint64_t address)
{
    InstructionOperand src = operand_get(ud_obj, 1, address);
 
    operand_set(ud_obj, 0, address, src);
}


void Translator :: mul (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);
    if (ud_obj->operand[0].size == 64) {
        /*      A B // src
         *    * C D // rax
         * ---------
         *    E F G
         *  H I J
         */
        InstructionOperand A (OPTYPE_VAR, 64);
        InstructionOperand B (OPTYPE_VAR, 64);
        InstructionOperand C (OPTYPE_VAR, 64);
        InstructionOperand D (OPTYPE_VAR, 64);
        InstructionOperand E (OPTYPE_VAR, 64);
        InstructionOperand F (OPTYPE_VAR, 64);
        InstructionOperand G (OPTYPE_VAR, 64);
        InstructionOperand H (OPTYPE_VAR, 64);
        InstructionOperand I (OPTYPE_VAR, 64);
        InstructionOperand J (OPTYPE_VAR, 64);
        InstructionOperand S32 (OPTYPE_CONSTANT, 8, 32);
        InstructionOperand A32 (OPTYPE_CONSTANT, 64, 0xffffffff);
        InstructionOperand tmp (OPTYPE_VAR, 64);
        InstructionOperand carry (OPTYPE_VAR, 64);

        InstructionOperand src = operand_get(ud_obj, 0, address);
        InstructionOperand rax(OPTYPE_VAR, 64, "UD_R_RAX");
        InstructionOperand rdx(OPTYPE_VAR, 64, "UD_R_RDX");

        instructions.push_back(new InstructionShr(address, size, A, src, S32));
        instructions.push_back(new InstructionAnd(address, size, B, src, A32));
        instructions.push_back(new InstructionShr(address, size, C, rax, S32));
        instructions.push_back(new InstructionAnd(address, size, D, rax, A32));

        // D * B
        instructions.push_back(new InstructionMul(address, size, tmp, B, D));
        instructions.push_back(new InstructionAnd(address, size, G, tmp, A32));
        instructions.push_back(new InstructionShr(address, size, carry, tmp, S32));

        // D * A
        instructions.push_back(new InstructionMul(address, size, tmp, D, A));
        instructions.push_back(new InstructionAdd(address, size, tmp, tmp, carry));
        instructions.push_back(new InstructionAnd(address, size, F, tmp, A32));
        instructions.push_back(new InstructionShr(address, size, E, tmp, S32));

        // C * B
        instructions.push_back(new InstructionMul(address, size, tmp, C, B));
        instructions.push_back(new InstructionAnd(address, size, J, tmp, A32));
        instructions.push_back(new InstructionShr(address, size, carry, tmp, S32));

        // C * A
        instructions.push_back(new InstructionMul(address, size, tmp, C, A));
        instructions.push_back(new InstructionAdd(address, size, tmp, tmp, carry));
        instructions.push_back(new InstructionAnd(address, size, I, tmp, A32));
        instructions.push_back(new InstructionShr(address, size, H, tmp, S32));

        instructions.push_back(new InstructionAdd(address, size, rax, F, J));
        instructions.push_back(new InstructionShr(address, size, carry, rax, S32));
        instructions.push_back(new InstructionShl(address, size, rax, rax, S32));
        instructions.push_back(new InstructionAdd(address, size, rax, rax, G));

        instructions.push_back(new InstructionShl(address, size, rdx, H, S32));
        instructions.push_back(new InstructionAdd(address, size, rdx, rdx, E));
        instructions.push_back(new InstructionAdd(address, size, rdx, rdx, I));
        instructions.push_back(new InstructionAdd(address, size, rdx, rdx, carry));

        InstructionOperand OF (OPTYPE_VAR, 1, "OF");
        InstructionOperand CF (OPTYPE_VAR, 1, "CF");
        InstructionOperand zero64 (OPTYPE_CONSTANT, 64, 0);
        instructions.push_back(new InstructionCmpEq(address, size, OF, rdx, zero64));
        instructions.push_back(new InstructionNot(address, size, OF, OF));
        instructions.push_back(new InstructionAssign(address, size, CF, OF));
    }
    else {
        throw std::runtime_error("mul on unsupported operand size");
    }
}


void Translator :: neg (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    InstructionOperand src = operand_get(ud_obj, 0, size);
    InstructionOperand zero (OPTYPE_CONSTANT, src.g_bits(), 0);
    InstructionOperand tmp  (OPTYPE_VAR, src.g_bits());

    instructions.push_back(new InstructionSub(address, size, tmp, zero, src));

    InstructionOperand CF (OPTYPE_VAR, 1, "CF");
    InstructionOperand SF (OPTYPE_VAR, 1, "SF");
    InstructionOperand ZF (OPTYPE_VAR, 1, "ZF");

    instructions.push_back(new InstructionCmpEq(address, size, CF, src, zero));
    instructions.push_back(new InstructionCmpEq(address, size, ZF, tmp, zero));
    instructions.push_back(new InstructionCmpLts(address, size, SF, tmp, zero));

    InstructionOperand SFxorOF    (OPTYPE_VAR, 1);
    InstructionOperand OF (OPTYPE_VAR, 1, "OF");
    instructions.push_back(new InstructionCmpLts(address, size, SFxorOF, tmp, src));
    instructions.push_back(new InstructionXor(address, size, OF, SFxorOF, SF));

    operand_set(ud_obj, 0, address, tmp);
}


void Translator :: nop (ud_t * ud_obj, uint64_t address)
{
    InstructionOperand noperand(OPTYPE_VAR, 1, "NOP");
    InstructionOperand zero(OPTYPE_CONSTANT, 1, 0);
    instructions.push_back(new InstructionAssign(address, ud_insn_len(ud_obj), noperand, zero));
}


void Translator :: Not (ud_t * ud_obj, uint64_t address)
{
    InstructionOperand dst = operand_get(ud_obj, 0, address);
    instructions.push_back(new InstructionNot(address, ud_insn_len(ud_obj), dst, dst));
    operand_set(ud_obj, 0, address, dst);
}


void Translator :: Or (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    InstructionOperand dst = operand_get(ud_obj, 0, address);
    InstructionOperand src = operand_get(ud_obj, 1, address);
    InstructionOperand tmp (OPTYPE_VAR, dst.g_bits());

    // src is always sign-extended to length of dst
    instructions.push_back(new InstructionSignExtend(address, size, tmp, src));
    instructions.push_back(new InstructionOr(address, size, tmp, dst, tmp));

    operand_set(ud_obj, 0, address, tmp);
}


void Translator :: pcmpeqb (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);
    InstructionOperand dst = operand(ud_obj, 0, address);
    InstructionOperand src;

    if (    (ud_obj->operand[1].type == UD_OP_MEM)
         && (dst.g_bits() == 128)) {
        src = operand_load(ud_obj, 1, address, 128);
    }
    else
        src = operand_get(ud_obj, 1, address);

    InstructionOperand result (OPTYPE_VAR, src.g_bits());
    InstructionOperand zero   (OPTYPE_CONSTANT, src.g_bits(), 0);
    instructions.push_back(new InstructionAssign(address, size, result, zero));

    InstructionOperand byte_mask (OPTYPE_CONSTANT, src.g_bits(), 0xff);
    InstructionOperand src_byte  (OPTYPE_VAR, src.g_bits());
    InstructionOperand dst_byte  (OPTYPE_VAR, src.g_bits());

    InstructionOperand byte_cmp (OPTYPE_VAR, 1);

    InstructionOperand mask (OPTYPE_VAR, src.g_bits());

    for (int i = 0; i < src.g_bits() / 8; i++) {
        InstructionOperand shift (OPTYPE_CONSTANT, 8, i * 8);
        // isolate bytes
        instructions.push_back(new InstructionShr(address, size, src_byte, src, shift));
        instructions.push_back(new InstructionShr(address, size, dst_byte, dst, shift));
        instructions.push_back(new InstructionAnd(address, size, src_byte, src_byte, byte_mask));
        instructions.push_back(new InstructionAnd(address, size, dst_byte, dst_byte, byte_mask));
        // compare bytes
        instructions.push_back(new InstructionCmpEq(address, size, byte_cmp, src_byte, dst_byte));
        // create mask
        instructions.push_back(new InstructionMul(address, size, mask, byte_mask, byte_cmp));
        // move mask into proper location
        instructions.push_back(new InstructionShl(address, size, mask, mask, shift));
        // set mask in result
        instructions.push_back(new InstructionOr(address, size, result, result, mask));
    }

    operand_set(ud_obj, 0, address, result);
}


void Translator :: pmovmskb (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    InstructionOperand dst = operand_get(ud_obj, 0, address);
    InstructionOperand src = operand_get(ud_obj, 1, address);

    InstructionOperand result (OPTYPE_VAR, dst.g_bits());
    InstructionOperand zero   (OPTYPE_CONSTANT, dst.g_bits(), 0);
    instructions.push_back(new InstructionAssign(address, size, result, zero));

    InstructionOperand tmp (OPTYPE_VAR, src.g_bits());
    for (int i = 0; i < src.g_bits() / 8; i++) {
        InstructionOperand isolateShift(OPTYPE_CONSTANT, 8, 7 + (i * 8));
        InstructionOperand one (OPTYPE_CONSTANT, src.g_bits(), 1);
        // isolate proper bit
        instructions.push_back(new InstructionShr(address, size, tmp, src, isolateShift));
        instructions.push_back(new InstructionAnd(address, size, tmp, tmp, one));
        // move bit to final location
        InstructionOperand finalShift(OPTYPE_CONSTANT, 8, i);
        instructions.push_back(new InstructionShl(address, size, tmp, tmp, finalShift));
        // or with result
        instructions.push_back(new InstructionOr(address, size, result, result, tmp));
    }

    operand_set(ud_obj, 0, address, result);
}


void Translator :: pop (ud_t * ud_obj, uint64_t address)
{
    size_t size;
    
    InstructionOperand dst     = operand(ud_obj, 0, address);
    InstructionOperand rsp     = InstructionOperand(OPTYPE_VAR, 64, "UD_R_RSP");
    InstructionOperand addsize = InstructionOperand(OPTYPE_CONSTANT, 8, STACK_ELEMENT_SIZE);
    
    if (ud_obj->operand[0].type == UD_OP_MEM) throw std::runtime_error("popping into memory location not supported");
    
    if (ud_obj->operand[0].type == UD_OP_REG) size = register_bits(ud_obj->operand[0].base);
    else size = ud_obj->operand[0].size;
    
    instructions.push_back(new InstructionLoad(address, ud_insn_len(ud_obj), size, dst, rsp));
    instructions.push_back(new InstructionAdd(address, ud_insn_len(ud_obj), rsp, rsp, addsize));
}


void Translator :: pshufd (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    InstructionOperand dst = operand_get(ud_obj, 0, address);
    InstructionOperand src = operand_get(ud_obj, 1, address);
    InstructionOperand order = operand_get(ud_obj, 2, address);
    InstructionOperand mask (OPTYPE_CONSTANT, 128, 0xffffffff);

    InstructionOperand tmp    (OPTYPE_VAR, 128);
    InstructionOperand result (OPTYPE_VAR, 128);
    InstructionOperand zero   (OPTYPE_CONSTANT, 128, 0);
    instructions.push_back(new InstructionAssign(address, size, result, zero));

    InstructionOperand selector (OPTYPE_VAR, 32);
    InstructionOperand three    (OPTYPE_CONSTANT, 8, 0x3);
    InstructionOperand threetwo (OPTYPE_CONSTANT, 8, 32);
    for (int i = 0; i < 4; i++) {
        InstructionOperand shiftr (OPTYPE_CONSTANT, 8, i * 2);
        InstructionOperand shiftl (OPTYPE_CONSTANT, 8, i * 32);
        // shift the order and mask it to find 2-bit index of dword in src
        instructions.push_back(new InstructionShr(address, size, selector, order, shiftr));
        instructions.push_back(new InstructionAnd(address, size, selector, selector, three));
        // multiply dword index by 32 and shift src right so target dword is lower 32-bits
        instructions.push_back(new InstructionMul(address, size, selector, selector, threetwo));
        instructions.push_back(new InstructionShr(address, size, tmp, src, selector));
        // mask lower 32-bits
        instructions.push_back(new InstructionAnd(address, size, tmp, tmp, mask));
        // move these bits to their correct location where they will reside in result
        instructions.push_back(new InstructionShl(address, size, tmp, tmp, shiftl));
        // or these bits to result
        instructions.push_back(new InstructionOr (address, size, result, result, tmp));
    }

    operand_set(ud_obj, 0, address, result);
}


void Translator :: punpcklbw (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);
    InstructionOperand dst = operand_get(ud_obj, 0, address);
    InstructionOperand src = operand_get(ud_obj, 1, address);
    InstructionOperand result (OPTYPE_VAR, 128, 0);

    if (dst.g_bits() == 128) {
        InstructionOperand tmpsrc   (OPTYPE_VAR, 128, 0);
        InstructionOperand tmpdst   (OPTYPE_VAR, 128, 0);
        InstructionOperand result_zero (OPTYPE_CONSTANT, 128, 0);
        instructions.push_back(new InstructionAssign(address, size, result, result_zero));
        for (int i = 0; i < 8; i++) {
            InstructionOperand mask_src (OPTYPE_CONSTANT, 64, 0xFFULL << (8 * i));
            InstructionOperand mask_dst (OPTYPE_CONSTANT, 64, 0xFFULL << (8 * i));
            InstructionOperand shift    (OPTYPE_CONSTANT, 8, 8 * i);
            InstructionOperand shift8   (OPTYPE_CONSTANT, 8, 8);

            // mask the appropriate bits and move them so the src bits are one byte
            // left of the dst bits
            instructions.push_back(new InstructionAnd(address, size, tmpsrc, src,    mask_src));
            instructions.push_back(new InstructionAnd(address, size, tmpdst, dst,    mask_dst));
            instructions.push_back(new InstructionShl(address, size, tmpsrc, tmpsrc, shift8));
            // or the tmpsrc and tmpdst together into one variable
            instructions.push_back(new InstructionOr (address, size, tmpdst, tmpdst, tmpsrc));
            // shift left and or with result
            instructions.push_back(new InstructionShl(address, size, tmpdst, tmpdst, shift));
            instructions.push_back(new InstructionOr (address, size, result, result, tmpdst));
        }
    }
    else {
        throw std::runtime_error("punpcklbw not supported on dst operand size");
    }
    operand_set(ud_obj, 0, address, result);
}


void Translator :: push (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);
    
    InstructionOperand src     = operand_get(ud_obj, 0, address);
    InstructionOperand rsp     = InstructionOperand(OPTYPE_VAR, 64, "UD_R_RSP");
    InstructionOperand subsize = InstructionOperand(OPTYPE_CONSTANT, 8, STACK_ELEMENT_SIZE);
    InstructionOperand sext      (OPTYPE_VAR, 64);
    
    instructions.push_back(new InstructionSub(address, size, rsp, rsp, subsize));
    instructions.push_back(new InstructionSignExtend(address, size, sext, src));
    instructions.push_back(new InstructionStore(address, ud_insn_len(ud_obj), 64, rsp, sext));
}


void Translator :: pxor (ud_t * ud_obj, uint64_t address)
{

    InstructionOperand src = operand_get(ud_obj, 0, address);
    InstructionOperand dst = operand_get(ud_obj, 1, address);
    InstructionOperand tmp (OPTYPE_VAR, 128, 0);

    instructions.push_back(new InstructionOr(address, ud_insn_len(ud_obj), tmp, dst, src));

    operand_set(ud_obj, 0, address, tmp);
}


void Translator :: rep (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    // decrement rcx
    InstructionOperand rcx (OPTYPE_VAR, 64, "UD_R_RCX");
    InstructionOperand one (OPTYPE_CONSTANT, 64, 1);
    instructions.push_back(new InstructionSub(address, size, rcx, rcx, one));

    // if rcx != 0, jmp negative this instruction size
    // calculate condition
    InstructionOperand zero (OPTYPE_CONSTANT, 64, 0);
    InstructionOperand cond (OPTYPE_VAR, 1);
    instructions.push_back(new InstructionCmpEq(address, size, cond, rcx, zero));
    instructions.push_back(new InstructionNot(address, size, cond, cond));
    // find rip - instruction_size
    InstructionOperand rip     (OPTYPE_VAR, 64, "UD_R_RIP");
    InstructionOperand jmpDst  (OPTYPE_VAR, 64);
    InstructionOperand insSize (OPTYPE_CONSTANT, 64, size);
    instructions.push_back(new InstructionSub(address, size, jmpDst, rip, insSize));
    // do conditional branch
    instructions.push_back(new InstructionBrc(address, size, cond, jmpDst));
}


void Translator :: repe (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    // decrement rcx
    InstructionOperand rcx (OPTYPE_VAR, 64, "UD_R_RCX");
    InstructionOperand one (OPTYPE_CONSTANT, 64, 1);
    instructions.push_back(new InstructionSub(address, size, rcx, rcx, one));

    // if rcx != 0 AND ZF==0, jmp negative this instruction size
    // calculate condition
    InstructionOperand ZF    (OPTYPE_VAR, 1, "ZF");
    InstructionOperand notZF (OPTYPE_VAR, 1);
    InstructionOperand zero (OPTYPE_CONSTANT, 64, 0);
    InstructionOperand cond (OPTYPE_VAR, 1);
    instructions.push_back(new InstructionCmpEq(address, size, cond, rcx, zero));
    instructions.push_back(new InstructionNot(address, size, notZF, ZF));
    instructions.push_back(new InstructionAnd(address, size, cond, cond, notZF));
    instructions.push_back(new InstructionNot(address, size, cond, cond));
    // find rip - instruction_size
    InstructionOperand rip     (OPTYPE_VAR, 64, "UD_R_RIP");
    InstructionOperand jmpDst  (OPTYPE_VAR, 64);
    InstructionOperand insSize (OPTYPE_CONSTANT, 64, size);
    instructions.push_back(new InstructionSub(address, size, jmpDst, rip, insSize));
    // do conditional branch
    instructions.push_back(new InstructionBrc(address, size, cond, jmpDst));
}


void Translator :: repne (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    // decrement rcx
    InstructionOperand rcx (OPTYPE_VAR, 64, "UD_R_RCX");
    InstructionOperand one (OPTYPE_CONSTANT, 64, 1);
    instructions.push_back(new InstructionSub(address, size, rcx, rcx, one));

    // if rcx != 0 AND ZF==0, jmp negative this instruction size
    // calculate condition
    InstructionOperand ZF    (OPTYPE_VAR, 1, "ZF");
    InstructionOperand notZF (OPTYPE_VAR, 1);
    InstructionOperand zero  (OPTYPE_CONSTANT, 64, 0);
    InstructionOperand cond  (OPTYPE_VAR, 1);
    instructions.push_back(new InstructionCmpEq(address, size, cond, rcx, zero));
    instructions.push_back(new InstructionNot(address, size, cond, cond));
    instructions.push_back(new InstructionNot(address, size, notZF, ZF));
    instructions.push_back(new InstructionAnd(address, size, cond, cond, notZF));
    // find rip - instruction_size
    InstructionOperand rip     (OPTYPE_VAR, 64, "UD_R_RIP");
    InstructionOperand jmpDst  (OPTYPE_VAR, 64);
    InstructionOperand insSize (OPTYPE_CONSTANT, 64, size);
    instructions.push_back(new InstructionSub(address, size, jmpDst, rip, insSize));
    // do conditional branch
    instructions.push_back(new InstructionBrc(address, size, cond, jmpDst));
}


void Translator :: ret (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    InstructionOperand one     = InstructionOperand(OPTYPE_CONSTANT, 1, 1);
    InstructionOperand dst     = InstructionOperand(OPTYPE_VAR, 64);
    InstructionOperand rsp     = InstructionOperand(OPTYPE_VAR, 64, "UD_R_RSP");
    InstructionOperand addsize = InstructionOperand(OPTYPE_CONSTANT, 8, STACK_ELEMENT_SIZE);

    instructions.push_back(new InstructionLoad(address, size, 64, dst, rsp));
    instructions.push_back(new InstructionAdd(address, size, rsp, rsp, addsize));
    instructions.push_back(new InstructionBrc(address, size, one, dst));
}


void Translator :: rol (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);
    InstructionOperand src   = operand_get(ud_obj, 0, address);
    InstructionOperand count = operand_get(ud_obj, 1, address);
    InstructionOperand countr  (OPTYPE_VAR, 8);
    InstructionOperand tmpl    (OPTYPE_VAR, src.g_bits());
    InstructionOperand tmpr    (OPTYPE_VAR, src.g_bits());
    InstructionOperand tmp     (OPTYPE_VAR, src.g_bits());
    InstructionOperand U64     (OPTYPE_CONSTANT, 8, 64);

    instructions.push_back(new InstructionShl(address, size, tmpl,   src,  count));
    instructions.push_back(new InstructionSub(address, size, countr, U64,  count));
    instructions.push_back(new InstructionShr(address, size, tmpr,   src,  countr));
    instructions.push_back(new InstructionOr (address, size, tmp,    tmpl, tmpr));

    // is the LSB of the result
    InstructionOperand CF       (OPTYPE_VAR, 1, "CF");
    InstructionOperand one      (OPTYPE_CONSTANT, 8, 1);
    instructions.push_back(new InstructionAnd(address, size, CF, tmp, one));

    // MSB of the result XOR with CF
    InstructionOperand OF      (OPTYPE_VAR, 1, "OF");
    InstructionOperand OFShift (OPTYPE_CONSTANT, 8, src.g_bits() - 1);
    instructions.push_back(new InstructionShr(address, size, OF, tmp, OFShift));
    instructions.push_back(new InstructionXor(address, size, OF, OF, CF));

    operand_set(ud_obj, 0, address, tmp);
}


void Translator :: ror (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    InstructionOperand src   = operand_get(ud_obj, 0, address);
    InstructionOperand count = operand_get(ud_obj, 1, address);
    InstructionOperand countl  (OPTYPE_VAR, 8);
    InstructionOperand tmpr    (OPTYPE_VAR, src.g_bits());
    InstructionOperand tmpl    (OPTYPE_VAR, src.g_bits());
    InstructionOperand tmp     (OPTYPE_VAR, src.g_bits());
    InstructionOperand U64     (OPTYPE_CONSTANT, 8, 64);

    instructions.push_back(new InstructionShr(address, size, tmpr,   src,  count));
    instructions.push_back(new InstructionSub(address, size, countl, U64,  count));
    instructions.push_back(new InstructionShl(address, size, tmpl,   src,  countl));
    instructions.push_back(new InstructionOr (address, size, tmp,    tmpr, tmpl));

    // is the MSB of the result
    InstructionOperand CF       (OPTYPE_VAR, 1, "CF");
    InstructionOperand CFShift  (OPTYPE_CONSTANT, 8, src.g_bits() - 1);
    instructions.push_back(new InstructionShr(address, size, CF, tmp, CFShift));

    // XOR of two most significant bits
    InstructionOperand OF       (OPTYPE_VAR, 1, "OF");
    InstructionOperand OFShift (OPTYPE_CONSTANT, 8, src.g_bits() - 2);
    instructions.push_back(new InstructionShr(address, size, OF, tmp, OFShift));
    instructions.push_back(new InstructionXor(address, size, OF, OF, CF));

    operand_set(ud_obj, 0, address, tmp);
}


void Translator :: sar (ud_t * ud_obj, uint64_t address)
{
    int size = ud_insn_len(ud_obj);

    /*
     * sar family instructions are weird about operands, as a "sar <dst>, 1" instruction
     * will have an operand size of 0 and cause errors when we call operand_lval
     */
    InstructionOperand bits (OPTYPE_VAR, 8);
    if (ud_obj->operand[1].size == 0) bits = InstructionOperand(OPTYPE_CONSTANT, 1, 1);
    else {
        // a SHR by CL will require us to mask bits appropriately
        if (ud_obj->operand[1].base == UD_R_CL) {
            InstructionOperand CL = operand_get(ud_obj, 1, address);
            InstructionOperand CLMask;
            if (ud_obj->pfx_rex & 0x8)
                CLMask = InstructionOperand(OPTYPE_CONSTANT, 8, 0x3f);
            else
                CLMask = InstructionOperand(OPTYPE_CONSTANT, 8, 0x1f);
            instructions.push_back(new InstructionAnd(address, size, bits, CL, CLMask));
        }
        else
            bits = operand_get(ud_obj, 1, address);
    }
    
    InstructionOperand dst        = operand_get(ud_obj, 0, address);
    InstructionOperand tmp          (OPTYPE_VAR, dst.g_bits());
    InstructionOperand one          (OPTYPE_CONSTANT, dst.g_bits(), 1);
    InstructionOperand zero         (OPTYPE_CONSTANT, dst.g_bits(), 0);
    
    instructions.push_back(new InstructionShr(address, size, tmp, dst, bits));

    // to calculate the sign, we isolate the sign bit, use it to set or zero
    // a variable of all 1s, shift that variable left (64 - bits), and or it
    // with the result (assuming 64-bit operand)
    InstructionOperand signBit  (OPTYPE_VAR, 1);
    InstructionOperand sixThree (OPTYPE_CONSTANT, 8, dst.g_bits() - 1);
    InstructionOperand all1s    (OPTYPE_CONSTANT, dst.g_bits(), 0xffffffffffffffffULL);
    InstructionOperand signMask (OPTYPE_VAR, dst.g_bits());
    InstructionOperand sixFour  (OPTYPE_CONSTANT, 8, dst.g_bits());
    InstructionOperand signShift (OPTYPE_VAR, 8);

    instructions.push_back(new InstructionShr(address, size, signBit, dst, sixThree));
    instructions.push_back(new InstructionMul(address, size, signMask, all1s, signBit));
    instructions.push_back(new InstructionSub(address, size, signShift, sixFour, bits));
    instructions.push_back(new InstructionShl(address, size, signMask, signMask, signShift));
    instructions.push_back(new InstructionOr(address, size, tmp, tmp, signMask));

    
    // CF takes last bit shifted out of dst
    InstructionOperand CF           (OPTYPE_VAR, 1, "CF");
    InstructionOperand bitsMinusOne (OPTYPE_VAR,   dst.g_bits());
    instructions.push_back(new InstructionSub(address, size, bitsMinusOne, bits, one));
    instructions.push_back(new InstructionShr(address, size, CF, dst, bitsMinusOne));
    
    // OF is cleared on 1-bit shifts, otherwise it's undefined
    InstructionOperand OF     (OPTYPE_VAR, 1, "OF");
    InstructionOperand OFMask (OPTYPE_VAR, 1);
    instructions.push_back(new InstructionCmpEq(address, size, OFMask, bits, one));
    instructions.push_back(new InstructionNot  (address, size, OFMask, OFMask));
    instructions.push_back(new InstructionAnd  (address, size, OF, OF, OFMask));
    
    InstructionOperand SF (OPTYPE_VAR, 1, "SF");
    instructions.push_back(new InstructionCmpLts(address, size, SF, tmp, zero));
    
    InstructionOperand ZF (OPTYPE_VAR, 1, "ZF");
    instructions.push_back(new InstructionCmpEq(address, size, ZF, tmp, zero));

    operand_set(ud_obj, 0, address, tmp);
}


void Translator :: sbb (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    InstructionOperand lhs = operand_get(ud_obj, 0, address);
    InstructionOperand rhs = operand_get(ud_obj, 1, address);
    InstructionOperand tmp (OPTYPE_VAR, lhs.g_bits());

    InstructionOperand CF       (OPTYPE_VAR, 1, "CF"); // unsigned overflow
    InstructionOperand ZF       (OPTYPE_VAR, 1, "ZF");
    InstructionOperand SF       (OPTYPE_VAR, 1, "SF"); // "negative" flag
    InstructionOperand zero     (OPTYPE_CONSTANT, tmp.g_bits(), 0);
    
    instructions.push_back(new InstructionAdd(address, size, tmp, rhs, CF));
    instructions.push_back(new InstructionSub(address, size, tmp, lhs, tmp));
    
    instructions.push_back(new InstructionCmpLtu(address, size, CF, lhs, tmp));
    instructions.push_back(new InstructionCmpEq (address, size, ZF, tmp, zero));
    instructions.push_back(new InstructionCmpLts(address, size, SF, tmp, zero));

    InstructionOperand SFxorOF    (OPTYPE_VAR, 1);
    InstructionOperand OF (OPTYPE_VAR, 1, "OF");
    instructions.push_back(new InstructionCmpLts(address, size, SFxorOF, tmp, rhs));
    instructions.push_back(new InstructionXor(address, size, OF, SFxorOF, SF));
    
    operand_set(ud_obj, 0, address, tmp);
}


void Translator :: scasb (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    // isolate lsbyte of rax
    InstructionOperand al (OPTYPE_VAR, 8);
    InstructionOperand rax (OPTYPE_VAR, 64, "UD_R_RAX");
    instructions.push_back(new InstructionAssign(address, size, al, rax));

    // get value to compare against
    InstructionOperand rdi (OPTYPE_VAR, 64, "UD_R_RDI");
    InstructionOperand cmpByte (OPTYPE_VAR, 8);
    instructions.push_back(new InstructionLoad(address, size, 8, cmpByte, rdi));

    // taken from Translator :: cmp
    InstructionOperand CF          (OPTYPE_VAR, 1, "CF");
    InstructionOperand CForZF      (OPTYPE_VAR, 1);
    InstructionOperand SFxorOF     (OPTYPE_VAR, 1);
    InstructionOperand SFxorOForZF (OPTYPE_VAR, 1);
    InstructionOperand ZF          (OPTYPE_VAR, 1, "ZF");
    InstructionOperand OF          (OPTYPE_VAR, 1, "OF");
    InstructionOperand SF          (OPTYPE_VAR, 1, "SF");
    InstructionOperand tmp0        (OPTYPE_VAR, 8);
    InstructionOperand tmp0s       (OPTYPE_CONSTANT, 8, 0);
    instructions.push_back(new InstructionCmpLtu(address, size, CF,          cmpByte, al));
    instructions.push_back(new InstructionCmpLeu(address, size, CForZF,      cmpByte, al));
    instructions.push_back(new InstructionCmpLts(address, size, SFxorOF,     cmpByte, al));
    instructions.push_back(new InstructionCmpLes(address, size, SFxorOForZF, cmpByte, al));
    instructions.push_back(new InstructionCmpEq (address, size, ZF,          cmpByte, al));
    instructions.push_back(new InstructionSub   (address, size, tmp0,        cmpByte, al));
    instructions.push_back(new InstructionCmpLts(address, size, SF,          tmp0, tmp0s));
    instructions.push_back(new InstructionXor   (address, size, OF,          SFxorOF, SF));

    // if DF==0 then RDI += 1; if DF == 1 then RDI -= 1
    InstructionOperand DF     (OPTYPE_VAR, 1, "DF");
    InstructionOperand neg2   (OPTYPE_CONSTANT, 64, -2);
    InstructionOperand one    (OPTYPE_CONSTANT, 64, 1);
    InstructionOperand add    (OPTYPE_VAR, 64);
    InstructionOperand addTmp (OPTYPE_VAR, 64);
    instructions.push_back(new InstructionAssign(address, size, add, one));
    instructions.push_back(new InstructionMul(address, size, addTmp, neg2, DF));
    instructions.push_back(new InstructionAdd(address, size, add, add, addTmp));
    instructions.push_back(new InstructionAdd(address, size, rdi, rdi, add));

    // suck it udis86 (doesn't correctly ret prefix for this instruction)
    if ((ud_insn_ptr(ud_obj)[0] == 0xf2) && (ud_obj->pfx_rep == 0))
        repne(ud_obj, address);
}


void Translator :: seta (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    InstructionOperand ZF    (OPTYPE_VAR, 1, "ZF");
    InstructionOperand CF    (OPTYPE_VAR, 1, "CF");
    InstructionOperand CFandZF    (OPTYPE_VAR, 1);
    InstructionOperand notCFandZF (OPTYPE_VAR, 1);

    instructions.push_back(new InstructionAnd(address, size, CFandZF, CF, ZF));
    instructions.push_back(new InstructionNot(address, size, notCFandZF, CFandZF));

    operand_set(ud_obj, 0, address, notCFandZF);
}


void Translator :: setg (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    InstructionOperand ZF    (OPTYPE_VAR, 1, "ZF");
    InstructionOperand SF    (OPTYPE_VAR, 1, "SF");
    InstructionOperand OF    (OPTYPE_VAR, 1, "OF");
    InstructionOperand SFeqOF (OPTYPE_VAR, 1);
    InstructionOperand notZF  (OPTYPE_VAR, 1);
    InstructionOperand notZFandSFeqOF (OPTYPE_VAR, 1);

    instructions.push_back(new InstructionNot(address, size, notZF, ZF));
    instructions.push_back(new InstructionCmpEq(address, size, SFeqOF, SF, OF));
    instructions.push_back(new InstructionAnd(address, size, notZFandSFeqOF, notZF, SFeqOF));

    operand_set(ud_obj, 0, address, notZFandSFeqOF);
}


void Translator :: setl (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    InstructionOperand SF (OPTYPE_VAR, 1, "SF");
    InstructionOperand OF (OPTYPE_VAR, 1, "OF");
    InstructionOperand SFeqOF (OPTYPE_VAR, 1);
    InstructionOperand notSFeqOF (OPTYPE_VAR, 1);

    instructions.push_back(new InstructionCmpEq(address, size, SFeqOF, SF, OF));
    instructions.push_back(new InstructionNot(address, size, notSFeqOF, SFeqOF));

    operand_set(ud_obj, 0, address, notSFeqOF);
}


void Translator :: setle (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    InstructionOperand ZF (OPTYPE_VAR, 1, "ZF");
    InstructionOperand SF (OPTYPE_VAR, 1, "SF");
    InstructionOperand OF (OPTYPE_VAR, 1, "OF");
    InstructionOperand SFeqOF        (OPTYPE_VAR, 1);
    InstructionOperand notSFeqOF     (OPTYPE_VAR, 1);
    InstructionOperand ZFornotSFeqOF (OPTYPE_VAR, 1);

    instructions.push_back(new InstructionCmpEq(address, size, SFeqOF, SF, OF));
    instructions.push_back(new InstructionNot(address, size, notSFeqOF, SFeqOF));
    instructions.push_back(new InstructionOr(address, size, ZFornotSFeqOF, ZF, notSFeqOF));

    operand_set(ud_obj, 0, address, ZFornotSFeqOF);
}


void Translator :: setnz (ud_t * ud_obj, uint64_t address)
{
    InstructionOperand ZF    (OPTYPE_VAR, 1, "ZF");
    InstructionOperand notZF (OPTYPE_VAR, 1);

    instructions.push_back(new InstructionNot(address, ud_insn_len(ud_obj), notZF, ZF));

    operand_set(ud_obj, 0, address, notZF);
}


void Translator :: setz (ud_t * ud_obj, uint64_t address)
{
    InstructionOperand ZF (OPTYPE_VAR, 1, "ZF");

    operand_set(ud_obj, 0, address, ZF);
}


void Translator :: shl (ud_t * ud_obj, uint64_t address)
{
    /*
     * sar family instructions are weird about operands, as a "sar <dst>, 1" instruction
     * will have an operand size of 0 and cause errors when we call operand_lval
     */
    InstructionOperand bits;
    if (ud_obj->operand[1].size == 0) bits = InstructionOperand(OPTYPE_CONSTANT, 1, 1);
    else                              bits = operand_get(ud_obj, 1, address);
    
    InstructionOperand dst        = operand_get(ud_obj, 0, address);
    InstructionOperand tmp          (OPTYPE_VAR, dst.g_bits());
    InstructionOperand one          (OPTYPE_CONSTANT, dst.g_bits(), 1);
    InstructionOperand zero         (OPTYPE_CONSTANT, dst.g_bits(), 0);
    
    int size = ud_insn_len(ud_obj);
    
    // CF takes last bit shifted out of dst
    InstructionOperand CF           (OPTYPE_VAR, 1, "CF");
    InstructionOperand bitsMinusOne (OPTYPE_VAR,   dst.g_bits());
    InstructionOperand dstBitsMinusOne (OPTYPE_CONSTANT, dst.g_bits(), dst.g_bits() - 1);
    instructions.push_back(new InstructionSub(address, size, bitsMinusOne, bits, one));
    instructions.push_back(new InstructionShl(address, size, tmp, dst, bitsMinusOne));
    instructions.push_back(new InstructionShr(address, size, CF, tmp, dstBitsMinusOne));
    
    // OF is set to 1 if MSB of original dst is same of final dst
    InstructionOperand OF      (OPTYPE_VAR, 1, "OF");
    instructions.push_back(new InstructionShl(address, size, tmp, dst, bitsMinusOne));
    instructions.push_back(new InstructionAnd(address, size, tmp, tmp, dst));
    instructions.push_back(new InstructionShr(address, size, OF, tmp, dstBitsMinusOne));
    
    InstructionOperand SF (OPTYPE_VAR, 1, "SF");
    instructions.push_back(new InstructionCmpLts(address, size, SF, tmp, zero));
    
    InstructionOperand ZF (OPTYPE_VAR, 1, "ZF");
    instructions.push_back(new InstructionCmpEq(address, size, ZF, tmp, zero));
    
    instructions.push_back(new InstructionShl(address, size, tmp, dst, bits));
    operand_set(ud_obj, 0, address, tmp);
}


void Translator :: shld (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    InstructionOperand dst  = operand_get(ud_obj, 0, address);
    InstructionOperand src  = operand_get(ud_obj, 1, address);
    InstructionOperand bits = operand_get(ud_obj, 2, address);

    InstructionOperand shift    (OPTYPE_VAR, 8);
    InstructionOperand srcShift (OPTYPE_VAR, 8);
    InstructionOperand srcBits  (OPTYPE_CONSTANT, src.g_bits());

    InstructionOperand tmp      (OPTYPE_VAR, dst.g_bits());
    InstructionOperand srcTmp   (OPTYPE_VAR, dst.g_bits());
    InstructionOperand sixThree (OPTYPE_CONSTANT, 8, 63);

    instructions.push_back(new InstructionAnd(address, size, shift, bits, sixThree));
    instructions.push_back(new InstructionShl(address, size, tmp, src, shift));
    instructions.push_back(new InstructionSub(address, size, srcShift, srcBits, shift));
    instructions.push_back(new InstructionShr(address, size, srcTmp, src, srcShift));

    InstructionOperand CF (OPTYPE_VAR, 1, "CF");
    InstructionOperand dstBitsMinusOne (OPTYPE_CONSTANT, 8, dst.g_bits() - 1);
    instructions.push_back(new InstructionShr(address, size, CF, dst, dstBitsMinusOne));

    InstructionOperand SF (OPTYPE_VAR, 1, "SF");
    InstructionOperand zero (OPTYPE_CONSTANT, dst.g_bits(), 0);
    instructions.push_back(new InstructionCmpLts(address, size, SF, zero, tmp));

    InstructionOperand ZF (OPTYPE_VAR, 1, "ZF");
    instructions.push_back(new InstructionCmpEq(address, size, ZF, tmp, zero));

    InstructionOperand OF    (OPTYPE_VAR, 1, "OF");
    InstructionOperand one   (OPTYPE_CONSTANT, dst.g_bits(), 1);
    InstructionOperand setOF (OPTYPE_VAR, 1);
    InstructionOperand OFTmp (OPTYPE_VAR, dst.g_bits());
    instructions.push_back(new InstructionCmpEq(address, size, setOF, tmp, one));
    instructions.push_back(new InstructionXor(address, size, OFTmp, tmp, dst));
    instructions.push_back(new InstructionShr(address, size, OFTmp, OFTmp, dstBitsMinusOne));
    instructions.push_back(new InstructionAnd(address, size, OF, OFTmp, setOF));

    operand_set(ud_obj, 0, address, tmp);
}


void Translator :: shr (ud_t * ud_obj, uint64_t address)
{
    int size = ud_insn_len(ud_obj);

    /*
     * sar family instructions are weird about operands, as a "sar <dst>, 1" instruction
     * will have an operand size of 0 and cause errors when we call operand_lval
     */
    InstructionOperand bits (OPTYPE_VAR, 8);
    if (ud_obj->operand[1].size == 0) bits = InstructionOperand(OPTYPE_CONSTANT, 1, 1);
    else {
        // a SHR by CL will require us to mask bits appropriately
        if (ud_obj->operand[1].base == UD_R_CL) {
            InstructionOperand CL = operand_get(ud_obj, 1, address);
            InstructionOperand CLMask;
            if (ud_obj->pfx_rex & 0x8)
                CLMask = InstructionOperand(OPTYPE_CONSTANT, 8, 0x3f);
            else
                CLMask = InstructionOperand(OPTYPE_CONSTANT, 8, 0x1f);
            instructions.push_back(new InstructionAnd(address, size, bits, CL, CLMask));
        }
        else
            bits = operand_get(ud_obj, 1, address);
    }
    
    InstructionOperand dst        = operand_get(ud_obj, 0, address);
    InstructionOperand tmp          (OPTYPE_VAR, dst.g_bits());
    InstructionOperand one          (OPTYPE_CONSTANT, dst.g_bits(), 1);
    InstructionOperand zero         (OPTYPE_CONSTANT, dst.g_bits(), 0);
    
    instructions.push_back(new InstructionShr(address, size, tmp, dst, bits));
    
    // CF takes last bit shifted out of dst
    InstructionOperand CF           (OPTYPE_VAR, 1, "CF");
    InstructionOperand bitsMinusOne (OPTYPE_VAR,   dst.g_bits());
    instructions.push_back(new InstructionSub(address, size, bitsMinusOne, bits, one));
    instructions.push_back(new InstructionShr(address, size, CF, dst, bitsMinusOne));
    
    // OF is set to the MSB of the original operand
    InstructionOperand OF      (OPTYPE_VAR, 1, "OF");
    InstructionOperand OFShift (OPTYPE_CONSTANT, 8, dst.g_bits() - 1);
    instructions.push_back(new InstructionShr  (address, size, OF, dst, OFShift));
    
    InstructionOperand SF (OPTYPE_VAR, 1, "SF");
    instructions.push_back(new InstructionCmpLts(address, size, SF, tmp, zero));
    
    InstructionOperand ZF (OPTYPE_VAR, 1, "ZF");
    instructions.push_back(new InstructionCmpEq(address, size, ZF, tmp, zero));
    
    operand_set(ud_obj, 0, address, tmp);
}


void Translator :: std (ud_t * ud_obj, uint64_t address)
{
    InstructionOperand DF (OPTYPE_VAR, 1, "DF");
    InstructionOperand one (OPTYPE_CONSTANT, 1, 1);

    instructions.push_back(new InstructionAssign(address, ud_insn_len(ud_obj), DF, one));
}


void Translator :: stosd (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    // get eax
    InstructionOperand eax (OPTYPE_VAR, 32, "UD_R_RAX");
    // store at location rdi
    InstructionOperand rdi (OPTYPE_VAR, 64, "UD_R_RDI");
    instructions.push_back(new InstructionStore(address, size, 32, rdi, eax));

    // if DF == 0 then rdi += 4, if DF == 1 then rdi -= 4
    InstructionOperand DF     (OPTYPE_VAR, 1, "DF");
    InstructionOperand neg2   (OPTYPE_CONSTANT, 64, -8);
    InstructionOperand one    (OPTYPE_CONSTANT, 64, 4);
    InstructionOperand add    (OPTYPE_VAR, 64);
    InstructionOperand addTmp (OPTYPE_VAR, 64);
    instructions.push_back(new InstructionAssign(address, size, add, one));
    instructions.push_back(new InstructionMul(address, size, addTmp, neg2, DF));
    instructions.push_back(new InstructionAdd(address, size, add, add, addTmp));
    instructions.push_back(new InstructionAdd(address, size, rdi, rdi, add));
}


void Translator :: sub (ud_t * ud_obj, uint64_t address)
{

    InstructionOperand lhs = operand_get(ud_obj, 0, address);
    InstructionOperand rhs = operand_get(ud_obj, 1, address);
    InstructionOperand tmp (OPTYPE_VAR, lhs.g_bits());
        
    InstructionOperand OFTmp    (OPTYPE_VAR, lhs.g_bits());
    InstructionOperand OFTmpShl (OPTYPE_CONSTANT, lhs.g_bits(), lhs.g_bits() - 1);
    InstructionOperand OF       (OPTYPE_VAR, 1, "OF"); // signed overflow
    InstructionOperand CF       (OPTYPE_VAR, 1, "CF"); // unsigned overflow
    InstructionOperand ZF       (OPTYPE_VAR, 1, "ZF");
    InstructionOperand SF       (OPTYPE_VAR, 1, "SF"); // "negative" flag
    InstructionOperand zero     (OPTYPE_CONSTANT, tmp.g_bits(), 0);
    
    instructions.push_back(new InstructionSub(address, ud_insn_len(ud_obj), tmp, lhs, rhs));
    
    instructions.push_back(new InstructionXor(address, ud_insn_len(ud_obj), OFTmp, tmp,   lhs));
    instructions.push_back(new InstructionShr(address, ud_insn_len(ud_obj), OF,    OFTmp, OFTmpShl));
    
    instructions.push_back(new InstructionCmpLtu(address, ud_insn_len(ud_obj), CF, lhs, tmp));
    instructions.push_back(new InstructionCmpEq (address, ud_insn_len(ud_obj), ZF, tmp, zero));
    instructions.push_back(new InstructionCmpLts(address, ud_insn_len(ud_obj), SF, tmp, zero));
    
    operand_set(ud_obj, 0, address, tmp);
}


void Translator :: syscall (ud_t * ud_obj, uint64_t address)
{
    instructions.push_back(new InstructionSyscall(address, ud_insn_len(ud_obj)));
}


void Translator :: test (ud_t * ud_obj, uint64_t address)
{
    InstructionOperand lhs = operand_get(ud_obj, 0, address);
    InstructionOperand rhs = operand_get(ud_obj, 1, address);
    InstructionOperand tmp   (OPTYPE_VAR, lhs.g_bits());
    
    InstructionOperand OF       (OPTYPE_VAR, 1, "OF"); // signed overflow
    InstructionOperand CF       (OPTYPE_VAR, 1, "CF"); // unsigned overflow
    InstructionOperand ZF       (OPTYPE_VAR, 1, "ZF");
    InstructionOperand SF       (OPTYPE_VAR, 1, "SF"); // "negative" flag
    InstructionOperand zero     (OPTYPE_CONSTANT, tmp.g_bits(), 0);
    
    instructions.push_back(new InstructionAnd(address, ud_insn_len(ud_obj), tmp, lhs, rhs));
    
    instructions.push_back(new InstructionCmpEq(address, ud_insn_len(ud_obj), ZF, tmp, zero));
    instructions.push_back(new InstructionCmpLts(address, ud_insn_len(ud_obj), SF, tmp, zero));
    instructions.push_back(new InstructionAssign(address, ud_insn_len(ud_obj), OF, zero));
    instructions.push_back(new InstructionAssign(address, ud_insn_len(ud_obj), CF, zero));
}


void Translator :: Xor (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    InstructionOperand dst(operand_get(ud_obj, 0, address));
    InstructionOperand src(operand_get(ud_obj, 1, address));

    instructions.push_back(new InstructionXor(address, size, dst, dst, src));

    InstructionOperand OF (OPTYPE_VAR, 1, "OF");
    InstructionOperand CF (OPTYPE_VAR, 1, "CF");
    InstructionOperand zero (OPTYPE_CONSTANT, dst.g_bits(), 0);
    instructions.push_back(new InstructionAssign(address, size, OF, zero));
    instructions.push_back(new InstructionAssign(address, size, CF, zero));

    InstructionOperand SF (OPTYPE_VAR, 1, "SF");
    InstructionOperand ZF (OPTYPE_VAR, 1, "ZF");
    instructions.push_back(new InstructionCmpEq(address, size, ZF, dst, zero));
    instructions.push_back(new InstructionCmpLts(address, size, SF, dst, zero));

    operand_set(ud_obj, 0, address, dst);
}
