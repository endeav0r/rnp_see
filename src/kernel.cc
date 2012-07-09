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

#include "kernel.h"

#include "instruction.h"
#include "page.h"

#include <cstdio>
#include <sstream>
#include <stdexcept>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>

#define DEBUG

// order of arguments by register
// %rdi, %rsi, %rdx, %r10, %r8 and %r9

void Kernel :: syscall (std::map <uint64_t, SymbolicValue> & variables, Memory & memory)
{
    SymbolicValue rax = variables[InstructionOperand::str_to_id("UD_R_RAX")];

    if (rax.g_wild())
        throw std::runtime_error("syscall called with wild rax");

    switch (rax.g_uint64()) {
        case 0x1  : sys_write  (variables, memory); break;
        case 0x5  : sys_fstat  (variables, memory); break;
        case 0x9  : sys_mmap   (variables, memory); break;
        case 0x14 : sys_writev (variables, memory); break;
        case 0x27 : sys_getpid (variables, memory); break;
        case 0x3c : sys_exit   (variables, memory); break;
        case 0xe7 : sys_exit_group(variables, memory); break;
        default :
            std::stringstream ss;
            ss << "unhandled syscall: " << rax.str();
            throw std::runtime_error(ss.str());
    }
}


void Kernel :: sys_exit (std::map <uint64_t, SymbolicValue> & variables, Memory & memory)
{
    SymbolicValue rdi = variables[InstructionOperand::str_to_id("UD_R_RDI")];

    #ifdef DEBUG
    std::cerr << "SYS_EXIT rdi=" << rdi.str()
              << std::endl;
    #endif    
}


void Kernel :: sys_exit_group (std::map <uint64_t, SymbolicValue> & variables, Memory & memory)
{
    SymbolicValue rdi = variables[InstructionOperand::str_to_id("UD_R_RDI")];

    #ifdef DEBUG
    std::cerr << "SYS_EXIT_GROUP rdi=" << rdi.str()
              << std::endl;
    #endif    
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

    #ifdef DEBUG
    std::cerr << "SYS_FSTAT rdi=" << rdi.str() << ", "
              << "rsi=" << rsi.str() << ", "
              << "stat.st_dev=" << std::hex << buf.st_dev << ", "
              << "stat.st_rdev=" << std::hex << buf.st_rdev << ", "
              << "stat.st_ino=" << std::hex << buf.st_ino << ", "
              << "stat.st_mode=" << std::hex << buf.st_mode << ", "
              << "result_rax=" << variables[InstructionOperand::str_to_id("UD_R_RAX")].str()
              << std::endl;
    #endif

    // some registers are not callee saved, and this call will trash it.
    variables[InstructionOperand::str_to_id("UD_R_RCX")] = SymbolicValue(64, -1);
    variables[InstructionOperand::str_to_id("UD_R_R11")] = SymbolicValue(64, 0x346);
}


void Kernel :: sys_getpid (std::map <uint64_t, SymbolicValue> & variables, Memory & memory)
{
    //SymbolicValueCmpLtu pid(SymbolicValue(64), SymbolicValue(64, 1 << 16));
    SymbolicValue pid(64, 5000);
    variables[InstructionOperand::str_to_id("UD_R_RAX")] = pid;

    #ifdef DEBUG
    std::cerr << "SYS_GETPID "
              << "result_rax=" << variables[InstructionOperand::str_to_id("UD_R_RAX")].str()
              << std::endl;
    #endif
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
    variables[InstructionOperand::str_to_id("UD_R_RAX")] = SymbolicValue(64, next_mmap);

    #ifdef DEBUG
    std::cerr << "SYS_MMAP rdi=" << rdi.str() << ", "
              << "rsi=" << rsi.str() << ", "
              << "rdx=" << rdx.str() << ", "
              << "r10=" << r10.str() << ", "
              << "r8="  << r8.str() << ", "
              << "r9="  << r9.str() << ", "
              << "result_rax=" << variables[InstructionOperand::str_to_id("UD_R_RAX")].str()
              << std::endl;
    #endif

    // some registers are not callee saved, and this call will trash it.
    variables[InstructionOperand::str_to_id("UD_R_RCX")] = SymbolicValue(64, -1);

    // increase next_mmap
    next_mmap += (mmap_size + 4095) % 4096;
}


void Kernel :: sys_write (std::map <uint64_t, SymbolicValue> & variables, Memory & memory)
{
    FILE * fh;
    std::stringstream filename;

    SymbolicValue rdi = variables[InstructionOperand::str_to_id("UD_R_RDI")];
    SymbolicValue rsi = variables[InstructionOperand::str_to_id("UD_R_RSI")];
    SymbolicValue rdx = variables[InstructionOperand::str_to_id("UD_R_RDX")];

    if (    (rdi.g_wild())
         || (rsi.g_wild())
         || (rdx.g_wild()))
        throw std::runtime_error("sys_write called with wild register argument");

    filename << "fh_" << rdi.g_uint64();
    fh = fopen(filename.str().c_str(), "ab");
    if (fh == NULL)
        throw std::runtime_error(std::string("error opening ")
                                 + filename.str() + " in Kernel::sys_write");
    if (rdx.g_uint64() > memory.g_data_size(rsi.g_uint64())){
        std::stringstream ss;
        ss << "count beyond memory limits in Kernel::sys_write. "
           << "rdi=" << rdi.str() << ", "
           << "memory.g_data_size()=" << memory.g_data_size(rsi.g_uint64()) << ", "
           << "rsi=" << rsi.str() << ", rdx=" << rdx.str();
        throw std::runtime_error(ss.str());
    }

    const uint8_t * data = memory.g_data(rsi.g_uint64());
    size_t written = fwrite(data, 1, rdx.g_uint64(), fh);

    if (written != rdx.g_uint64()) {
        std::stringstream ss;
        ss << "wrote " << written << " bytes, should have written "
           << rdx.g_uint64() << " bytes.";
        throw std::runtime_error(ss.str());
    }

    fclose(fh);

    variables[InstructionOperand::str_to_id("UD_R_RAX")] = SymbolicValue(64, written);

    #ifdef DEBUG
    std::cerr << "SYS_WRITE rdi=" << rdi.str() << ", "
              << "rsi=" << rsi.str() << ", "
              << "rdx=" << rdx.str() << ", "
              << "result_rax=" << variables[InstructionOperand::str_to_id("UD_R_RAX")].str()
              << std::endl;
    #endif

    // some registers are not callee saved, and this call will trash it.
    variables[InstructionOperand::str_to_id("UD_R_RCX")] = SymbolicValue(64, -1);
}


void Kernel :: sys_writev (std::map <uint64_t, SymbolicValue> & variables, Memory & memory)
{
    FILE * fh;
    std::stringstream filename;
    size_t bytes_written = 0;

    SymbolicValue rdi = variables[InstructionOperand::str_to_id("UD_R_RDI")];
    SymbolicValue rsi = variables[InstructionOperand::str_to_id("UD_R_RSI")];
    SymbolicValue rdx = variables[InstructionOperand::str_to_id("UD_R_RDX")];

    int buf_n = rdx.g_uint64();

    struct iovec * vec = (struct iovec *) memory.g_data(rsi.g_uint64());

    filename << "fh_" << rdi.g_uint64();
    fh = fopen(filename.str().c_str(), "wb");

    for (int i = 0; i < buf_n; i++) {
        bytes_written += vec->iov_len;
        fwrite(memory.g_data((uint64_t) vec->iov_base), 1, vec->iov_len, fh);
    }

    fclose(fh);

    variables[InstructionOperand::str_to_id("UD_R_RAX")] = SymbolicValue(64, bytes_written);
}