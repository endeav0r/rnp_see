#include "kernel.h"

#include "instruction.h"

#include <sstream>
#include <stdexcept>

void Kernel :: syscall (std::map <uint64_t, SymbolicValue> & variables, Memory & memory)
{
    SymbolicValue rax = variables[InstructionOperand::str_to_id("UD_R_RAX")];

    if (rax.g_wild())
        throw std::runtime_error("syscall called with wild rax");

    switch (rax.g_value()) {
        case 0x14 : sys_getpid(variables, memory); break;
        default :
            std::stringstream ss;
            ss << "unhandled syscall: " << rax.g_value();
            throw std::runtime_error(ss.str());
    }
}


void Kernel :: sys_getpid (std::map <uint64_t, SymbolicValue> & variables, Memory & memory)
{
    SymbolicValueCmpLtu pid(SymbolicValue(64), SymbolicValue(64, 1 << 16));
    variables[InstructionOperand::str_to_id("UD_R_RAX")] = pid;
}