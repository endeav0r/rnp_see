#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#include <stdexcept>
#include <string>

#include <udis86.h>

#include "../vm.h"
#include "../lx86.h"

void dump_state (VM & vm, struct user_regs_struct * regs)
{
    uint64_t vm_rip = vm.g_variable(InstructionOperand::str_to_id("UD_R_RIP")).g_uint64();
    uint64_t vm_rax = vm.g_variable(InstructionOperand::str_to_id("UD_R_RAX")).g_uint64();
    uint64_t vm_rbx = vm.g_variable(InstructionOperand::str_to_id("UD_R_RBX")).g_uint64();
    uint64_t vm_rcx = vm.g_variable(InstructionOperand::str_to_id("UD_R_RCX")).g_uint64();
    uint64_t vm_rdx = vm.g_variable(InstructionOperand::str_to_id("UD_R_RDX")).g_uint64();
    uint64_t vm_rsi = vm.g_variable(InstructionOperand::str_to_id("UD_R_RSI")).g_uint64();
    uint64_t vm_rdi = vm.g_variable(InstructionOperand::str_to_id("UD_R_RDI")).g_uint64();
    uint64_t vm_rsp = vm.g_variable(InstructionOperand::str_to_id("UD_R_RSP")).g_uint64();
    uint64_t vm_rbp = vm.g_variable(InstructionOperand::str_to_id("UD_R_RBP")).g_uint64();
    uint64_t vm_r8  = vm.g_variable(InstructionOperand::str_to_id("UD_R_R8")).g_uint64();
    uint64_t vm_r9  = vm.g_variable(InstructionOperand::str_to_id("UD_R_R9")).g_uint64();
    uint64_t vm_r10 = vm.g_variable(InstructionOperand::str_to_id("UD_R_R10")).g_uint64();
    uint64_t vm_r11 = vm.g_variable(InstructionOperand::str_to_id("UD_R_R11")).g_uint64();
    uint64_t vm_r12 = vm.g_variable(InstructionOperand::str_to_id("UD_R_R12")).g_uint64();
    uint64_t vm_r13 = vm.g_variable(InstructionOperand::str_to_id("UD_R_R13")).g_uint64();
    uint64_t vm_r14 = vm.g_variable(InstructionOperand::str_to_id("UD_R_R14")).g_uint64();
    uint64_t vm_r15 = vm.g_variable(InstructionOperand::str_to_id("UD_R_R15")).g_uint64();

    std::cerr << std::hex;
    std::cerr << "rip=vm(" << vm_rip << ") proc(" << regs->rip << ")" << std::endl;
    std::cerr << "rax=vm(" << vm_rax << ") proc(" << regs->rax << ")" << std::endl;
    std::cerr << "rbx=vm(" << vm_rbx << ") proc(" << regs->rbx << ")" << std::endl;
    std::cerr << "rcx=vm(" << vm_rcx << ") proc(" << regs->rcx << ")" << std::endl;
    std::cerr << "rdx=vm(" << vm_rdx << ") proc(" << regs->rdx << ")" << std::endl;
    std::cerr << "rsi=vm(" << vm_rsi << ") proc(" << regs->rsi << ")" << std::endl;
    std::cerr << "rdi=vm(" << vm_rdi << ") proc(" << regs->rdi << ")" << std::endl;
    std::cerr << "rsp=vm(" << vm_rsp << ") proc(" << regs->rsp << ")" << std::endl;
    std::cerr << "rbp=vm(" << vm_rbp << ") proc(" << regs->rbp << ")" << std::endl;
    std::cerr << "r8=vm("  << vm_r8  << ") proc(" << regs->r8  << ")" << std::endl;
    std::cerr << "r9=vm("  << vm_r9  << ") proc(" << regs->r9  << ")" << std::endl;
    std::cerr << "r10=vm(" << vm_r10 << ") proc(" << regs->r10 << ")" << std::endl;
    std::cerr << "r11=vm(" << vm_r11 << ") proc(" << regs->r11 << ")" << std::endl;
    std::cerr << "r12=vm(" << vm_r12 << ") proc(" << regs->r12 << ")" << std::endl;
    std::cerr << "r13=vm(" << vm_r13 << ") proc(" << regs->r13 << ")" << std::endl;
    std::cerr << "r14=vm(" << vm_r14 << ") proc(" << regs->r14 << ")" << std::endl;
    std::cerr << "r15=vm(" << vm_r15 << ") proc(" << regs->r15 << ")" << std::endl;
}

int main (int argc, char * argv[])
{
    struct user_regs_struct regs;
    
    Lx86 * lx86 = new Lx86(argv[1]);
    VM vm(lx86, false);

    std::cout << "init complete";

    std::cout << "vm starts at "
              << std::hex << vm.g_variable(InstructionOperand::str_to_id("UD_R_RIP")).g_uint64()
              << std::endl;

    lx86->g_regs(&regs);
    std::cout << "child starts at "
              << std::hex << regs.rip
              << std::endl;

    int step = 0;
    while (true) {
        // step vm and child process
        step++;
        vm.step();
        lx86->step();

        std::cout << std::dec << step << " ";

        lx86->g_regs(&regs);

        uint64_t vm_rip = vm.g_variable(InstructionOperand::str_to_id("UD_R_RIP")).g_uint64();
        uint64_t vm_rax = vm.g_variable(InstructionOperand::str_to_id("UD_R_RAX")).g_uint64();
        uint64_t vm_rbx = vm.g_variable(InstructionOperand::str_to_id("UD_R_RBX")).g_uint64();
        uint64_t vm_rcx = vm.g_variable(InstructionOperand::str_to_id("UD_R_RCX")).g_uint64();
        uint64_t vm_rdx = vm.g_variable(InstructionOperand::str_to_id("UD_R_RDX")).g_uint64();
        uint64_t vm_rsi = vm.g_variable(InstructionOperand::str_to_id("UD_R_RSI")).g_uint64();
        uint64_t vm_rdi = vm.g_variable(InstructionOperand::str_to_id("UD_R_RDI")).g_uint64();
        uint64_t vm_rsp = vm.g_variable(InstructionOperand::str_to_id("UD_R_RSP")).g_uint64();
        uint64_t vm_rbp = vm.g_variable(InstructionOperand::str_to_id("UD_R_RBP")).g_uint64();
        uint64_t vm_r8  = vm.g_variable(InstructionOperand::str_to_id("UD_R_R8")).g_uint64();
        uint64_t vm_r9  = vm.g_variable(InstructionOperand::str_to_id("UD_R_R9")).g_uint64();
        uint64_t vm_r10 = vm.g_variable(InstructionOperand::str_to_id("UD_R_R10")).g_uint64();
        uint64_t vm_r11 = vm.g_variable(InstructionOperand::str_to_id("UD_R_R11")).g_uint64();
        uint64_t vm_r12 = vm.g_variable(InstructionOperand::str_to_id("UD_R_R12")).g_uint64();
        uint64_t vm_r13 = vm.g_variable(InstructionOperand::str_to_id("UD_R_R13")).g_uint64();
        uint64_t vm_r14 = vm.g_variable(InstructionOperand::str_to_id("UD_R_R14")).g_uint64();
        uint64_t vm_r15 = vm.g_variable(InstructionOperand::str_to_id("UD_R_R15")).g_uint64();

        if (vm_rip != regs.rip) {
            std::cerr << std::endl;
            std::cerr << "diverging instruction pointer at step " << std::dec << step << std::endl;
            dump_state(vm, &regs);
            break;
        }
        else if (    (vm_rax != regs.rax)
                  || (vm_rbx != regs.rbx)
                  || (vm_rcx != regs.rcx)
                  || (vm_rdx != regs.rdx)
                  || (vm_rsi != regs.rsi)
                  || (vm_rdi != regs.rdi)
                  || (vm_rsp != regs.rsp)
                  || (vm_rbp != regs.rbp)
                  || (vm_r8  != regs.r8)
                  || (vm_r9  != regs.r9)
                  || (vm_r10 != regs.r10)
                  || (vm_r11 != regs.r11)
                  || (vm_r12 != regs.r12)
                  || (vm_r13 != regs.r13)
                  || (vm_r14 != regs.r14)
                  || (vm_r15 != regs.r15)) {
            std::cerr << std::endl;
            std::cerr << "diverging registers at step " << std::dec << step << std::endl;
            dump_state(vm, &regs);
            break;
        }
    }

    delete lx86;

    return 0;
}
