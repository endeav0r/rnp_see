#include "vm.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

#include "elf.h"

#define DEBUG

void VM :: debug_x86_registers ()
{
    std::stringstream ss;

    #define GVALUE(XX) variables[InstructionOperand::str_to_id(XX)].str()
    #define PRINTREG(XX) << XX << "=" \
                         << std::hex << GVALUE(XX) << std::endl
    
    ss PRINTREG("UD_R_RAX") PRINTREG("UD_R_RBX")
       PRINTREG("UD_R_RCX") PRINTREG("UD_R_RDX")
       PRINTREG("UD_R_RDI") PRINTREG("UD_R_RSI")
       PRINTREG("UD_R_RBP") PRINTREG("UD_R_RSP")
       PRINTREG("UD_R_R8")  PRINTREG("UD_R_R9")
       PRINTREG("UD_R_R10") PRINTREG("UD_R_R11") 
       PRINTREG("UD_R_R12") PRINTREG("UD_R_R13")
       PRINTREG("UD_R_R14") PRINTREG("UD_R_R15")
       PRINTREG("UD_R_RIP");
    std::cout << ss.str();
}

void VM :: debug_variables ()
{
    std::stringstream ss;

    std::map <uint64_t, SymbolicValue> :: iterator it;
    for (it = variables.begin(); it != variables.end(); it++) {
        ss << std::hex << it->first << "=" 
           << std::hex << it->second.g_value() << std::endl;
    }

    std::cout << ss.str();
}

const SymbolicValue VM :: g_value (InstructionOperand operand)
{
    if (operand.g_type() == OPTYPE_CONSTANT)
        return SymbolicValue(operand.g_bits(), operand.g_value());

    if (variables.count(operand.g_id()) == 0) {
        std::stringstream ss;
        ss << "operand not found, id: " << operand.g_id();
        throw std::runtime_error(ss.str());
    }
    
    return variables[operand.g_id()];
}

// this works like a loader
VM :: VM (std::string filename)
{
    #ifdef DEBUG
    std::cerr << "loading ELF" << std::endl;
    #endif

    Elf * elf = Elf::Get(filename);


    #ifdef DEBUG
    std::cerr << "loading translator" << std::endl;
    #endif    
    translator = Translator();

    #ifdef DEBUG
    std::cerr << "getting memory" << std::endl;
    #endif
    memory     = elf->g_memory();

    #ifdef DEBUG
    std::cerr << "getting variables" << std::endl;
    #endif
    variables  = elf->g_variables();

    #ifdef DEBUG
    std::cerr << "getting instruction pointer id" << std::endl;
    #endif
    ip_id      = elf->g_ip_id();

    delete elf;

    std::cout << "Memory mmap: " << std::endl << memory.memmap() << std::endl;
}


void VM :: step ()
{
    uint64_t ip_addr = variables[ip_id].g_value();
    std::list <Instruction *> instructions;

    instructions = translator.translate(ip_addr,
                                        memory.g_data(ip_addr),
                                        memory.g_data_size(ip_addr));

    size_t instruction_size = instructions.front()->g_size();

    std::cout << "step IP=" << std::hex << ip_addr
             << " " << translator.native_asm((uint8_t *) memory.g_data(ip_addr), instruction_size);
    for (size_t i = 0; i < instruction_size; i++) {
        std::cout << " " << std::hex << (int) memory.g_byte(ip_addr + i);
    }
    std::cout << std::endl;

    variables[ip_id] = variables[ip_id] + SymbolicValue(64, instruction_size);

    #define EXECUTE(XX) if (dynamic_cast<XX *>(*it)) \
                            execute(dynamic_cast<XX *>(*it));

    std::list <Instruction *> :: iterator it;
    for (it = instructions.begin(); it != instructions.end(); it++) {
        std::cout << (*it)->str() << std::endl;
             EXECUTE(InstructionAdd)
        else EXECUTE(InstructionAnd)
        else EXECUTE(InstructionAssign)
        else EXECUTE(InstructionBrc)
        else EXECUTE(InstructionCmpEq)
        else EXECUTE(InstructionCmpLts)
        else EXECUTE(InstructionCmpLtu)
        else EXECUTE(InstructionLoad)
        else EXECUTE(InstructionNot)
        else EXECUTE(InstructionShr)
        else EXECUTE(InstructionSignExtend)
        else EXECUTE(InstructionStore)
        else EXECUTE(InstructionSub)
        else EXECUTE(InstructionXor)
        else throw std::runtime_error("unimplemented vm instruction: " + (*it)->str());
    }
}


void VM :: execute (InstructionAdd * add)
{
    variables[add->g_dst().g_id()] = g_value(add->g_lhs()) + g_value(add->g_rhs());
}


void VM :: execute (InstructionAnd * And)
{
    variables[And->g_dst().g_id()] = g_value(And->g_lhs()) & g_value(And->g_rhs());
}


void VM :: execute (InstructionAssign * assign)
{
    InstructionOperand dst  = assign->g_dst();
    const SymbolicValue src = g_value(assign->g_src());
    variables[dst.g_id()]   = src;
}


void VM :: execute (InstructionBrc * brc)
{
    const SymbolicValue condition = g_value(brc->g_cond());
    if (condition.g_wild())
        throw std::runtime_error("symbolic brc not yet supported");
    else {
        variables[ip_id] = g_value(brc->g_dst());
    }
}


void VM :: execute (InstructionCmpEq * cmpeq)
{
    variables[cmpeq->g_dst().g_id()] =    g_value(cmpeq->g_lhs())
                                       == g_value(cmpeq->g_rhs());
}


void VM :: execute (InstructionCmpLts * cmplts)
{
    variables[cmplts->g_dst().g_id()] = g_value(cmplts->g_lhs()).cmpLts(g_value(cmplts->g_rhs()));
}


void VM :: execute (InstructionCmpLtu * cmpltu)
{
    variables[cmpltu->g_dst().g_id()] = g_value(cmpltu->g_lhs()).cmpLtu(g_value(cmpltu->g_rhs()));
}


void VM :: execute (InstructionLoad * load)
{
    InstructionOperand dst = load->g_dst();
    const SymbolicValue src = g_value(load->g_src());

    switch (src.g_bits()) {
    case 8 :
        variables[dst.g_id()] = SymbolicValue(dst.g_bits(),
                                              memory.g_byte(src.g_value()));
        break;
    case 16 :
        variables[dst.g_id()] = SymbolicValue(dst.g_bits(),
                                              memory.g_word(src.g_value()));
        break;
    case 32 :
        variables[dst.g_id()] = SymbolicValue(dst.g_bits(),
                                              memory.g_dword(src.g_value()));
        break;
    case 64 :
        variables[dst.g_id()] = SymbolicValue(dst.g_bits(),
                                              memory.g_qword(src.g_value()));
        break;
    default :
        std::stringstream ss;
        ss << "Tried to load invalid bit size: " << src.g_bits();
        throw std::runtime_error(ss.str());
    }
}


void VM :: execute (InstructionNot * Not)
{
    variables[Not->g_dst().g_id()] = ~ g_value(Not->g_src());
}


void VM :: execute (InstructionShr * shr)
{
    variables[shr->g_dst().g_id()] = g_value(shr->g_lhs()) >> g_value(shr->g_rhs());
}


void VM :: execute (InstructionSignExtend * sext)
{
    variables[sext->g_dst().g_id()] = g_value(sext->g_src()).signExtend();
}


void VM :: execute (InstructionStore * store)
{
    const SymbolicValue dst = g_value(store->g_dst());
    const SymbolicValue src = g_value(store->g_src());

    switch (store->g_bits()) {
    case 8  : memory.s_byte(dst.g_value(), src.g_value()); break;
    case 16 : memory.s_word(dst.g_value(), src.g_value()); break;
    case 32 : memory.s_dword(dst.g_value(), src.g_value()); break;
    case 64 : memory.s_qword(dst.g_value(), src.g_value()); break;
    default :
        std::stringstream ss;
        ss << "Tried to store invalid bit size: " << store->g_bits();
        throw std::runtime_error(ss.str());
    }
}


void VM :: execute (InstructionSub * sub)
{
    variables[sub->g_dst().g_id()] = g_value(sub->g_lhs()) - g_value(sub->g_rhs());
}


void VM :: execute (InstructionXor * Xor)
{
    InstructionOperand dst = Xor->g_dst();
    const SymbolicValue lhs = g_value(Xor->g_lhs());
    const SymbolicValue rhs = g_value(Xor->g_rhs());

    variables[dst.g_id()] = SymbolicValue(dst.g_bits(), lhs.g_value() ^ rhs.g_value());
}

void VM :: execute (Instruction * ins) {}