#include "lx86.h"

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#include <set>

#include <sstream>
#include <stdexcept>

#include "instruction.h"
#include "page.h"

#define DEBUG

Lx86 :: Lx86 (std::string filename)
{
    pid_t pid = fork();
    if (pid == -1)
        throw std::runtime_error("fork failure");

    if (pid == 0) {
        // start the child process, prepare for tracing
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl(filename.c_str(), "lx86_child_proc", NULL);
        throw std::runtime_error("ptrace failure");
    }

    this->pid = pid;
    std::cout << "child pid: " << pid << std::endl;

    // load the ELF for this process
    Elf * elf = Elf :: Get(filename);
    uint64_t entry_address = elf->g_entry();
    delete elf;

    int status;
    while (true) {
        wait(&status);
        if (WIFSTOPPED(status))
            break;
    }

    // set a breakpoint at the entry point
    long saved_entry_bytes = ptrace(PTRACE_PEEKTEXT, pid, entry_address, NULL);
    long break_bytes = (saved_entry_bytes & (~0xff)) | 0xcc;
    ptrace(PTRACE_POKETEXT, pid, entry_address, break_bytes);

    // execute to breakpoint
    ptrace(PTRACE_CONT, pid, NULL, NULL);

    while (true) {
        wait(&status);
        if (WIFSTOPPED(status))
            break;
    }

    // restore memory at breakpoint
    ptrace(PTRACE_POKETEXT, pid, entry_address, saved_entry_bytes);

    // correct rip
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid, NULL, &regs);
    regs.rip = entry_address;
    ptrace(PTRACE_SETREGS, pid, NULL, &regs);

    // load all elfs
    std::stringstream ss;
    ss << "/proc/" << (int) pid << "/maps";
    FILE * maps_fh = fopen(ss.str().c_str(), "r");
    if (maps_fh == NULL)
        throw std::runtime_error("could not open file " + ss.str());

    std::unordered_set <std::string> loaded_filenames;

    std::string absolute_filename = realpath(filename.c_str(), NULL);

    char line[512];
    while (fgets(line, 512, maps_fh) != NULL) {
        char str[128];
        char elfname[128];
        uint64_t offset;

        sscanf(line, "%lx-%*s %*s %*s %*s %s %s", &offset, str, elfname);
        if (loaded_filenames.count(elfname) == 0) {
            loaded_filenames.insert(elfname);
            if (absolute_filename == elfname) {
                try {
                    elfs.push_back(new Elf64(elfname, 0));
                    std::cerr << elfname << " loaded " << std::endl;
                }
                catch (std::exception & e) {}
            }
            else {
                try {
                    elfs.push_back(new Elf64(elfname, offset));
                    std::cerr << elfname << " loaded at " << std::hex << offset << std::endl;
                }
                catch (std::exception & e) {}
            }
        }
    }
}

Lx86 :: ~Lx86 ()
{
    std::cerr << "~Lx86" << std::endl;

    std::list <Elf *> :: iterator it;
    for (it = elfs.begin(); it != elfs.end(); it++) {
        delete *it;
    }
}

// props http://www.linuxjournal.com/article/6210?page=0,1
Memory Lx86 :: g_memory ()
{
    std::map <uint64_t, Page *> pages;

    // read /proc/#/maps file
    std::stringstream ss;
    ss << "/proc/" << (int) pid << "/maps";
    FILE * maps_fh = fopen(ss.str().c_str(), "r");
    if (maps_fh == NULL)
        throw std::runtime_error("could not open file " + ss.str());

    ss.str("");
    ss << "/proc/" << (int) pid << "/mem";
    int mem_fd = open(ss.str().c_str(), O_RDONLY);
    if (mem_fd == -1)
        throw std::runtime_error("could not open file " + ss.str());

    char line[512];
    while (fgets(line, 512, maps_fh) != NULL) {
        char str[128];
        char perms[8];
        off64_t start;
        off64_t end;
        // find memory locations
        sscanf(line, "%lx-%lx %s %*s %*s %s", &start, &end, perms, str);

        if (perms[0] != 'r')
            continue;

        uint8_t * buf = new uint8_t[end - start];
        
        // read those bytes!
        if (lseek64(mem_fd, start, SEEK_SET) != start) {
            throw std::runtime_error("error on Lx86::g_memory lseek64");
        }
        off64_t bytes_read = read(mem_fd, buf, end - start);

        if (bytes_read != end - start) {
            throw std::runtime_error("error on Lx86::g_memory read");
        }

        // create new page from buf
        pages[start] = new Page(end - start, buf);
        delete[] buf;
    }

    close(mem_fd);
    fclose(maps_fh);

    return Memory(pages);
}

std::map <uint64_t, SymbolicValue> Lx86 :: g_variables ()
{
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid, NULL, &regs);

    std::map <uint64_t, SymbolicValue> variables;
    variables[InstructionOperand::str_to_id("UD_R_RAX")] = SymbolicValue(64, regs.rax);
    variables[InstructionOperand::str_to_id("UD_R_RBX")] = SymbolicValue(64, regs.rbx);
    variables[InstructionOperand::str_to_id("UD_R_RCX")] = SymbolicValue(64, regs.rcx);
    variables[InstructionOperand::str_to_id("UD_R_RDX")] = SymbolicValue(64, regs.rdx);
    variables[InstructionOperand::str_to_id("UD_R_RSI")] = SymbolicValue(64, regs.rsi);
    variables[InstructionOperand::str_to_id("UD_R_RDI")] = SymbolicValue(64, regs.rdi);
    variables[InstructionOperand::str_to_id("UD_R_R8")]  = SymbolicValue(64, regs.r8);
    variables[InstructionOperand::str_to_id("UD_R_R9")]  = SymbolicValue(64, regs.r9);
    variables[InstructionOperand::str_to_id("UD_R_R10")] = SymbolicValue(64, regs.r10);
    variables[InstructionOperand::str_to_id("UD_R_R11")] = SymbolicValue(64, regs.r11);
    variables[InstructionOperand::str_to_id("UD_R_R12")] = SymbolicValue(64, regs.r12);
    variables[InstructionOperand::str_to_id("UD_R_R13")] = SymbolicValue(64, regs.r13);
    variables[InstructionOperand::str_to_id("UD_R_R14")] = SymbolicValue(64, regs.r14);
    variables[InstructionOperand::str_to_id("UD_R_R15")] = SymbolicValue(64, regs.r15);
    variables[InstructionOperand::str_to_id("UD_R_RBP")] = SymbolicValue(64, regs.rbp);
    variables[InstructionOperand::str_to_id("UD_R_XMM0")] = SymbolicValue(128, 0);
    variables[InstructionOperand::str_to_id("UD_R_XMM1")] = SymbolicValue(128, 0);
    variables[InstructionOperand::str_to_id("UD_R_XMM2")] = SymbolicValue(128, 0);
    variables[InstructionOperand::str_to_id("UD_R_XMM3")] = SymbolicValue(128, 0);

    variables[InstructionOperand::str_to_id("DF")] = SymbolicValue(1, 0);
    
    variables[InstructionOperand::str_to_id("UD_R_FS")]  = SymbolicValue(64, regs.fs_base);
    variables[InstructionOperand::str_to_id("UD_R_RSP")] = SymbolicValue(64, regs.rsp);
    variables[InstructionOperand::str_to_id("UD_R_RIP")] = SymbolicValue(64, regs.rip);

    return variables;
}

uint64_t Lx86 :: g_ip_id ()
{
    return InstructionOperand::str_to_id("UD_R_RIP");
}

std::string Lx86 :: func_symbol (uint64_t address)
{
    std::list <Elf *> :: iterator it;

    for (it = elfs.begin(); it != elfs.end(); it++) {
        std::string symbol = (*it)->func_symbol(address);
        if (symbol != "") {
            return symbol;
        }
    }
    return "";
}

void Lx86 :: step ()
{
    ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL);
    int status;
    while (true) {
        waitpid(pid, &status, 0);
        if (WIFSTOPPED(status))
            break;
    }
}

void Lx86 :: g_regs (struct user_regs_struct * regs)
{
    ptrace(PTRACE_GETREGS, pid, NULL, regs);
}