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
    #define DEBUGOUTREG(RREG) if (vm_##RREG != regs->RREG) std::cerr << "-"; \
    std::cerr << #RREG "=vm(" << vm_##RREG << ") proc(" << regs->RREG << ")" << std::endl;

    DEBUGOUTREG(rip)
    DEBUGOUTREG(rax)
    DEBUGOUTREG(rbx)
    DEBUGOUTREG(rcx)
    DEBUGOUTREG(rdx)
    DEBUGOUTREG(rsi)
    DEBUGOUTREG(rdi)
    DEBUGOUTREG(rsp)
    DEBUGOUTREG(rbp)
    DEBUGOUTREG(r8)
    DEBUGOUTREG(r9)
    DEBUGOUTREG(r10)
    DEBUGOUTREG(r11)
    DEBUGOUTREG(r12)
    DEBUGOUTREG(r13)
    DEBUGOUTREG(r14)
    DEBUGOUTREG(r15)

    //vm.debug_x86_registers();
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

        if (vm_rip != regs.rip) {
            step++;
            vm.step();
            vm_rip = vm.g_variable(InstructionOperand::str_to_id("UD_R_RIP")).g_uint64();
        }

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
        #define REGCOND(REG) \
            (    (vm_##REG != regs.REG)\
              && (    (vm_##REG < 0x77ff000000000000ULL)\
                   || (vm_##REG > 0x77ff100000000000ULL)))
        else if (REGCOND(rax) || REGCOND(rbx) || REGCOND(rcx) ||
                 REGCOND(rdx) || REGCOND(rsi) || REGCOND(rdi) ||
                 REGCOND(rsp) || REGCOND(rbp) || REGCOND(r8)  ||
                 REGCOND(r9)  || REGCOND(r10) || REGCOND(r11) ||
                 REGCOND(r12) || REGCOND(r13) || REGCOND(r14) ||
                 REGCOND(r15)) {
            std::cerr << std::endl;
            std::cerr << "diverging registers at step " << std::dec << step << std::endl;
            dump_state(vm, &regs);
            break;
        }
    }

    delete lx86;

    return 0;
}
