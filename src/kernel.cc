#include "kernel.h"

#include "instruction.h"
#include "page.h"

#include <sstream>
#include <stdexcept>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// order of arguments by register
// %rdi, %rsi, %rdx, %r10, %r8 and %r9

void Kernel :: syscall (std::map <uint64_t, SymbolicValue> & variables, Memory & memory)
{
    SymbolicValue rax = variables[InstructionOperand::str_to_id("UD_R_RAX")];

    if (rax.g_wild())
        throw std::runtime_error("syscall called with wild rax");

    switch (rax.g_uint64()) {
        case 0x5  : sys_fstat (variables, memory); break;
        case 0x9  : sys_mmap  (variables, memory); break;
        case 0x27 : sys_getpid(variables, memory); break;
        default :
            std::stringstream ss;
            ss << "unhandled syscall: " << rax.str();
            throw std::runtime_error(ss.str());
    }
}


void Kernel :: sys_fstat (std::map <uint64_t, SymbolicValue> & variables, Memory & memory)
{
    struct stat buf;
    
    SymbolicValue rdi = variables[InstructionOperand::str_to_id("UD_R_RDI")];
    SymbolicValue rsi = variables[InstructionOperand::str_to_id("UD_R_RSI")];

    if ((rdi.g_wild()) || (rsi.g_wild())) {
        throw std::runtime_error("sys_fstat called with wild variable");
    }

    int result = fstat(rdi.g_uint64(), &buf);

    memory.s_data(rsi.g_uint64(), (const uint8_t *) &buf, sizeof(struct stat));

    variables[InstructionOperand::str_to_id("UD_R_RAX")] = SymbolicValue(64, result);
}


void Kernel :: sys_getpid (std::map <uint64_t, SymbolicValue> & variables, Memory & memory)
{
    //SymbolicValueCmpLtu pid(SymbolicValue(64), SymbolicValue(64, 1 << 16));
    SymbolicValue pid(64, 5000);
    variables[InstructionOperand::str_to_id("UD_R_RAX")] = pid;
}


void Kernel :: sys_mmap (std::map <uint64_t, SymbolicValue> & variables, Memory & memory)
{
    SymbolicValue rdi = variables[InstructionOperand::str_to_id("UD_R_RDI")];
    SymbolicValue rsi = variables[InstructionOperand::str_to_id("UD_R_RSI")];
    SymbolicValue rdx = variables[InstructionOperand::str_to_id("UD_R_RDX")];
    SymbolicValue r10 = variables[InstructionOperand::str_to_id("UD_R_R10")];
    SymbolicValue r8  = variables[InstructionOperand::str_to_id("UD_R_R8")];
    SymbolicValue r9  = variables[InstructionOperand::str_to_id("UD_R_R9")];

    // no wild symbolic values
    if (    (rdi.g_wild())
         || (rsi.g_wild())
         || (rdx.g_wild())
         || (r10.g_wild())
         || (r8.g_wild())
         || (r9.g_wild()))
        throw std::runtime_error("sys_mmap called with wild variables. unsupported.");

    // rdi must == 0 (rdi == addr requested)
    if (rdi.g_uint64() != 0)
        throw std::runtime_error("sys_mmap rdi != 0");
    // r8 must == -1 (r8 == filehandle)
    if (r8.signExtend(64).g_value() != SymbolicValue(64, -1).g_value())
        throw std::runtime_error("sys_mmap r8 != -1, " + r8.signExtend(64).str());

    size_t mmap_size = rsi.g_uint64();

    // create new page for this mmap area
    Page * page = new Page (mmap_size);
    // insert this page at the next available mmap address and set result to its address
    memory.s_page(next_mmap, page);
    variables[InstructionOperand::str_to_id("UD_R_RAX")] == SymbolicValue(64, next_mmap);

    // increase next_mmap
    next_mmap += (mmap_size + 4095) % 4096;
}