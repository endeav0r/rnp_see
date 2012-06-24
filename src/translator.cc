#include "translator.h"

#include <udis86.h>

#include <cstdio>
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
        case UD_Iadd     : add     (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Iand     : And     (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Icall    : call    (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Icmp     : cmp     (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ihlt     : hlt     (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ija      : ja      (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ijl      : jl      (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ijle     : jle     (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ijmp     : jmp     (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ijnz     : jnz     (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ijs      : js      (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ijz      : jz      (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ilea     : lea     (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ileave   : leave   (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Imov     : mov     (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Imovzx   : movzx   (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Imovsxd  : movsxd  (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Imul     : mul     (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Inop     : nop     (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ipop     : pop     (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ipush    : push    (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Iret     : ret     (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Irol     : rol     (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Isar     : sar     (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Isetz    : setz    (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ishr     : shr     (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Isub     : sub     (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Isyscall : syscall (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Itest    : test    (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        case UD_Ixor     : Xor     (&ud_obj, address + ud_insn_off(&ud_obj)); break;
        default :
            std::stringstream ss;
            ss << "unhandled instruction ["
            << ud_insn_hex(&ud_obj) << " => "
            <<  ud_insn_asm(&ud_obj) << "]"
            << " " << ins_debug_str(&ud_obj);
            std::cerr << ss.str() << std::endl;
            throw std::runtime_error(ss.str());
        }
    }
    else throw std::runtime_error("unable to disassemble instruction");
    
    return instructions;
}


int Translator :: register_bits (int reg)
{
    if ((reg >= UD_R_AL)  && (reg <= UD_R_R15B)) return 8;
    if ((reg >= UD_R_AX)  && (reg <= UD_R_R15W)) return 16;
    if ((reg >= UD_R_EAX) && (reg <= UD_R_R15D)) return 32;
    if ((reg >= UD_R_RAX) && (reg <= UD_R_R15))  return 64;
    if ((reg == UD_R_RIP) || (reg == UD_R_FS))   return 64;
    
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
    else if ((reg == UD_R_FS) || (reg == UD_R_RIP)) return reg;

    throw std::runtime_error(std::string("")
                             + "register_to64 called on unsupported register: " 
                             + ud_type_DEBUG[reg]);
    return -1;
}


void Translator :: operand_set (ud_t * ud_obj, int operand_i, uint64_t address, InstructionOperand & value)
{
    InstructionOperand lhs = operand(ud_obj, operand_i, address);
    if (ud_obj->operand[operand_i].type == UD_OP_MEM)
        instructions.push_back(new InstructionStore(address,
                                                    ud_insn_len(ud_obj),
                                                    ud_obj->operand[operand_i].size,
                                                    lhs,
                                                    value));
    else
        instructions.push_back(new InstructionAssign(address, ud_insn_len(ud_obj), lhs, value));
}


InstructionOperand Translator :: operand_get (ud_t * ud_obj, int operand_i, uint64_t address)
{
    InstructionOperand result = operand(ud_obj, operand_i, address);
    if (ud_obj->operand[operand_i].type == UD_OP_MEM) {
        InstructionOperand loaded_result = InstructionOperand(OPTYPE_VAR, ud_obj->operand[operand_i].size);
        instructions.push_back(new InstructionLoad(address,
                                                   ud_insn_len(ud_obj),
                                                   ud_obj->operand[operand_i].size,
                                                   loaded_result,
                                                   result));
        return loaded_result;
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
    
    if (operand.type == UD_OP_REG) {
        // get the register's name
        std::string name = ud_type_DEBUG[register_to64(operand.base)];
        return InstructionOperand(OPTYPE_VAR, register_bits(operand.base), name);
    }
    
    else if ((operand.type == UD_OP_MEM) && (ud_obj->pfx_seg)) {
        size_t size = ud_insn_len(ud_obj);

        std::string name = ud_type_DEBUG[register_to64(ud_obj->pfx_seg)];
        InstructionOperand seg(OPTYPE_VAR, register_bits(ud_obj->pfx_seg), name);

        InstructionOperand offset(OPTYPE_CONSTANT,
                                  operand.offset,
                                  operand_lval(operand.offset, operand));

        InstructionOperand result(OPTYPE_VAR, 64);
        instructions.push_back(new InstructionAdd(address, size, result, seg, offset));
        return result;
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


void Translator :: add (ud_t * ud_obj, uint64_t address)
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
    
    instructions.push_back(new InstructionAdd(address, ud_insn_len(ud_obj), tmp, lhs, rhs));
    
    instructions.push_back(new InstructionXor(address, ud_insn_len(ud_obj), OFTmp, tmp,   lhs));
    instructions.push_back(new InstructionShr(address, ud_insn_len(ud_obj), OF,    OFTmp, OFTmpShl));
    
    instructions.push_back(new InstructionCmpLtu(address, ud_insn_len(ud_obj), CF, tmp, lhs));
    instructions.push_back(new InstructionCmpEq (address, ud_insn_len(ud_obj), ZF, tmp, zero));
    instructions.push_back(new InstructionCmpLts(address, ud_insn_len(ud_obj), SF, tmp, zero));
    
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

void Translator :: call (ud_t * ud_obj, uint64_t address)
{
    
    InstructionOperand rip     = InstructionOperand(OPTYPE_VAR, 64, "UD_R_RIP");
    InstructionOperand rsp     = InstructionOperand(OPTYPE_VAR, 64, "UD_R_RSP");
    InstructionOperand subsize = InstructionOperand(OPTYPE_CONSTANT, 8, STACK_ELEMENT_SIZE);
    
    // push RIP
    instructions.push_back(new InstructionSub(address, ud_insn_len(ud_obj), rsp, rsp, subsize));
    instructions.push_back(new InstructionStore(address, ud_insn_len(ud_obj), 64, rsp, rip));

    // set RIP += offset
    InstructionOperand dst = operand_get(ud_obj, 0, address);
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


void Translator :: hlt (ud_t * ud_obj, uint64_t address)
{
    instructions.push_back(new InstructionHlt(address, ud_insn_len(ud_obj)));
}


void Translator :: ja (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);
    InstructionOperand dst = operand_get(ud_obj, 0, address);
    
    InstructionOperand CF        (OPTYPE_VAR, 1, "CF");
    InstructionOperand ZF        (OPTYPE_VAR, 1, "ZF");
    InstructionOperand CForZF    (OPTYPE_VAR, 1);
    InstructionOperand notCForZF (OPTYPE_VAR, 1);

    if (ud_obj->operand[0].type == UD_OP_JIMM) {
        InstructionOperand rip(OPTYPE_VAR, 64, "UD_R_RIP");
        InstructionOperand tmp(OPTYPE_VAR, 64);
        instructions.push_back(new InstructionSignExtend(address, size, tmp, dst));
        instructions.push_back(new InstructionAdd(address, size, tmp, rip, tmp));
        instructions.push_back(new InstructionBrc(address, size, notCForZF, tmp));
    }
    else instructions.push_back(new InstructionBrc(address, size, notCForZF, dst));
}


void Translator :: jl (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);
    InstructionOperand SFxorOF (OPTYPE_VAR, 1);
    InstructionOperand SF (OPTYPE_VAR, 1, "SF");
    InstructionOperand OF (OPTYPE_VAR, 1, "OF");
    InstructionOperand dst = operand_get(ud_obj, 0, address);

    instructions.push_back(new InstructionXor(address, size, SFxorOF, SF, OF));

    if (ud_obj->operand[0].type == UD_OP_JIMM) {
        InstructionOperand rip(OPTYPE_VAR, 64, "UD_R_RIP");
        InstructionOperand tmp(OPTYPE_VAR, 64);
        instructions.push_back(new InstructionSignExtend(address, size, tmp, dst));
        instructions.push_back(new InstructionAdd(address, size, tmp, rip, tmp));
        instructions.push_back(new InstructionBrc(address, size, SFxorOF, tmp));
    }
    else instructions.push_back(new InstructionBrc(address, size, SFxorOF, dst));
}


void Translator :: jle (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);
    InstructionOperand ZForSFxorOF (OPTYPE_VAR, 1);
    InstructionOperand ZF (OPTYPE_VAR, 1, "ZF");
    InstructionOperand SF (OPTYPE_VAR, 1, "SF");
    InstructionOperand OF (OPTYPE_VAR, 1, "OF");
    InstructionOperand dst = operand_get(ud_obj, 0, address);

    instructions.push_back(new InstructionXor(address, size, ZForSFxorOF, SF, OF));
    instructions.push_back(new InstructionOr(address, size, ZForSFxorOF, ZForSFxorOF, ZF));

    if (ud_obj->operand[0].type == UD_OP_JIMM) {
        InstructionOperand rip(OPTYPE_VAR, 64, "UD_R_RIP");
        InstructionOperand tmp(OPTYPE_VAR, 64);
        instructions.push_back(new InstructionSignExtend(address, size, tmp, dst));
        instructions.push_back(new InstructionAdd(address, size, tmp, rip, tmp));
        instructions.push_back(new InstructionBrc(address, size, ZForSFxorOF, tmp));
    }
    else instructions.push_back(new InstructionBrc(address, size, ZForSFxorOF, dst));
}


// making this an InstructionBrc greatly simplifies analysis as all branches are
// either InstructionBrc or InstructionCall
void Translator :: jmp (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);

    InstructionOperand dst = operand_get(ud_obj, 0, address);
    InstructionOperand t (OPTYPE_CONSTANT, 1, 1);

    if (ud_obj->operand[0].type == UD_OP_JIMM) {
        InstructionOperand rip(OPTYPE_VAR, 64, "UD_R_RIP");
        InstructionOperand tmp(OPTYPE_VAR, 64);
        instructions.push_back(new InstructionSignExtend(address, size, tmp, dst));
        instructions.push_back(new InstructionAdd(address, size, tmp, rip, tmp));
        instructions.push_back(new InstructionBrc(address, size, t, tmp));
    }
    else instructions.push_back(new InstructionBrc(address, size, t, dst));
}


void Translator :: jnz (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);
    InstructionOperand dst = operand_get(ud_obj, 0, address);
    
    InstructionOperand ZF    (OPTYPE_VAR,   1, "ZF");
    InstructionOperand notZF (OPTYPE_VAR,   1, "notZF");

    instructions.push_back(new InstructionNot(address, ud_insn_len(ud_obj), notZF, ZF));

    if (ud_obj->operand[0].type == UD_OP_JIMM) {
        InstructionOperand rip(OPTYPE_VAR, 64, "UD_R_RIP");
        InstructionOperand tmp(OPTYPE_VAR, 64);
        instructions.push_back(new InstructionSignExtend(address, size, tmp, dst));
        instructions.push_back(new InstructionAdd(address, size, tmp, rip, tmp));
        instructions.push_back(new InstructionBrc(address, size, notZF, tmp));
    }
    else instructions.push_back(new InstructionBrc(address, size, notZF, dst));
}

void Translator :: js (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);
    InstructionOperand SF  (OPTYPE_VAR, 1, "SF");
    InstructionOperand dst = operand_get(ud_obj, 0, address);

    if (ud_obj->operand[0].type == UD_OP_JIMM) {
        InstructionOperand rip(OPTYPE_VAR, 64, "UD_R_RIP");
        InstructionOperand tmp(OPTYPE_VAR, 64);
        instructions.push_back(new InstructionSignExtend(address, size, tmp, dst));
        instructions.push_back(new InstructionAdd(address, size, tmp, rip, tmp));
        instructions.push_back(new InstructionBrc(address, size, SF, tmp));
    }
    else instructions.push_back(new InstructionBrc(address, size, SF, dst));
}

void Translator :: jz (ud_t * ud_obj, uint64_t address)
{
    size_t size = ud_insn_len(ud_obj);
    InstructionOperand ZF  (OPTYPE_VAR, 1, "ZF");
    InstructionOperand dst = operand_get(ud_obj, 0, address);

    if (ud_obj->operand[0].type == UD_OP_JIMM) {
        InstructionOperand rip(OPTYPE_VAR, 64, "UD_R_RIP");
        InstructionOperand tmp(OPTYPE_VAR, 64);
        instructions.push_back(new InstructionSignExtend(address, size, tmp, dst));
        instructions.push_back(new InstructionAdd(address, size, tmp, rip, tmp));
        instructions.push_back(new InstructionBrc(address, size, ZF, tmp));
    }
    else instructions.push_back(new InstructionBrc(address, size, ZF, dst));
}


void Translator :: lea (ud_t * ud_obj, uint64_t address)
{
    InstructionOperand dst = operand(ud_obj, 0, address);
    InstructionOperand src = operand(ud_obj, 1, address);
    
    instructions.push_back(new InstructionAssign(address, ud_insn_len(ud_obj), dst, src));
}


void Translator :: leave (ud_t * ud_obj, uint64_t address)
{
    InstructionOperand rsp   (OPTYPE_VAR, 64, UD_R_RSP);
    InstructionOperand rbp   (OPTYPE_VAR, 64, UD_R_RBP);
    InstructionOperand eight (OPTYPE_CONSTANT, 8, 8);
    
    instructions.push_back(new InstructionAssign(address, ud_insn_len(ud_obj), rsp, rbp));
    instructions.push_back(new InstructionLoad  (address, ud_insn_len(ud_obj), 64, rbp, rsp));
    instructions.push_back(new InstructionAdd   (address, ud_insn_len(ud_obj), rsp, rsp, eight));
}


void Translator :: mov (ud_t * ud_obj, uint64_t address)
{
    InstructionOperand src = operand_get(ud_obj, 1, address);
    operand_set(ud_obj, 0, address, src);
}


// dst must be register
void Translator :: movsxd (ud_t * ud_obj, uint64_t address)
{
    InstructionOperand dst = operand(ud_obj, 0, address);
    InstructionOperand src = operand_get(ud_obj, 1, address);
    
    instructions.push_back(new InstructionSignExtend(address, ud_insn_len(ud_obj), dst, src));
}


// dst must be register
void Translator :: movzx (ud_t * ud_obj, uint64_t address)
{
    // conduct the move first
    mov(ud_obj, address);
 
    InstructionOperand dst = operand(ud_obj, 0, address);
    InstructionOperand toand(OPTYPE_CONSTANT, 8, 0xff);
    
    // x86 will put the byte in the lowest octet of the register, so AND it with 0xff
    instructions.push_back(new InstructionAnd(address, ud_insn_len(ud_obj), dst, dst, toand));
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


void Translator :: nop (ud_t * ud_obj, uint64_t address)
{
    InstructionOperand noperand(OPTYPE_VAR, 1, "NOP");
    InstructionOperand zero(OPTYPE_CONSTANT, 1, 0);
    instructions.push_back(new InstructionAssign(address, ud_insn_len(ud_obj), noperand, zero));
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


void Translator :: push (ud_t * ud_obj, uint64_t address)
{
    size_t size;
    
    InstructionOperand src     = operand_get(ud_obj, 0, address);
    InstructionOperand rsp     = InstructionOperand(OPTYPE_VAR, 64, "UD_R_RSP");
    InstructionOperand subsize = InstructionOperand(OPTYPE_CONSTANT, 8, STACK_ELEMENT_SIZE);
    
    if (ud_obj->operand[0].type == UD_OP_REG) size = register_bits(ud_obj->operand[0].base);
    else size = ud_obj->operand[0].size;
    
    instructions.push_back(new InstructionSub(address, ud_insn_len(ud_obj), rsp, rsp, subsize));
    instructions.push_back(new InstructionStore(address, ud_insn_len(ud_obj), size, rsp, src));
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
    InstructionOperand tmpl (OPTYPE_VAR, src.g_bits());
    InstructionOperand tmpr (OPTYPE_VAR, src.g_bits());
    InstructionOperand U64  (OPTYPE_CONSTANT, 8, 64);

    instructions.push_back(new InstructionShr(address, size, tmpl, src, count));
    instructions.push_back(new InstructionSub(address, size, count, U64, count));
    instructions.push_back(new InstructionShl(address, size, tmpr, src, count));
    instructions.push_back(new InstructionOr(address, size, tmpl, tmpl, tmpr));

    InstructionOperand CF  (OPTYPE_VAR, 1, "CF");
    InstructionOperand one (OPTYPE_CONSTANT, 1, 1);

    instructions.push_back(new InstructionAnd(address, size, CF, one, tmpl));

    InstructionOperand OF      (OPTYPE_VAR, 1, "OF");
    InstructionOperand OFShift (OPTYPE_CONSTANT, 8, src.g_bits() - 1);

    instructions.push_back(new InstructionShr(address, size, OF, tmpl, OFShift));
    instructions.push_back(new InstructionXor(address, size, OF, OF, CF));

    operand_set(ud_obj, 0, address, tmpl);
}


void Translator :: sar (ud_t * ud_obj, uint64_t address)
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
    InstructionOperand signPreserve (OPTYPE_CONSTANT, dst.g_bits(), 1 << (dst.g_bits() - 1));
    InstructionOperand one          (OPTYPE_CONSTANT, dst.g_bits(), 1);
    InstructionOperand zero         (OPTYPE_CONSTANT, dst.g_bits(), 0);
    
    int size = ud_insn_len(ud_obj);
    
    instructions.push_back(new InstructionShr(address, size, tmp, dst, bits));
    instructions.push_back(new InstructionAnd(address, size, tmp, signPreserve, dst));
    
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


void Translator :: setz (ud_t * ud_obj, uint64_t address)
{
    InstructionOperand ZF (OPTYPE_VAR, 1, "ZF");

    operand_set(ud_obj, 0, address, ZF);
}


void Translator :: shr (ud_t * ud_obj, uint64_t address)
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


void Translator :: syscall (ud_t * ud_obj, uint64_t address)
{
    instructions.push_back(new InstructionSyscall(address, ud_insn_len(ud_obj)));
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
    InstructionOperand dst(operand_get(ud_obj, 0, address));
    InstructionOperand src(operand_get(ud_obj, 1, address));

    instructions.push_back(new InstructionXor(address, ud_insn_len(ud_obj), dst, dst, src));

    operand_set(ud_obj, 0, address, dst);
}
