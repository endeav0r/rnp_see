#ifndef translatex86_HEADER
#define translatex86_HEADER

#include <list>

#include <inttypes.h>

#include <udis86.h>

#include "instruction.h"

#define STACK_ELEMENT_SIZE 8

class Translator {
    private :
        std::list <Instruction *> instructions;
    
    public :
        std::string native_asm (uint8_t * data, int size);
        std::list <Instruction *> translate (uint64_t address, uint8_t * data, size_t size);
        
        int register_bits    (int reg);
        int register_to64    (int reg);
        
        uint64_t           operand_lval (int bits, struct ud_operand operand);
        void               operand_set  (ud_t * ud_obj, int operand_i, uint64_t address, InstructionOperand & value);
        InstructionOperand operand_load (ud_t * ud_obj, int operand_i, uint64_t address, int bits);
        InstructionOperand operand_get  (ud_t * ud_obj, int operand_i, uint64_t address);
        InstructionOperand operand      (ud_t * ud_obj, int operand_i, uint64_t address);

        void cmovcc    (ud_t * ud_obj, uint64_t address, InstructionOperand cond);
        void jcc       (ud_t * ud_obj, uint64_t address, InstructionOperand cond);
        
        void adc       (ud_t * ud_obj, uint64_t address);
        void add       (ud_t * ud_obj, uint64_t address);
        void And       (ud_t * ud_obj, uint64_t address);
        void bsf       (ud_t * ud_obj, uint64_t address);
        void call      (ud_t * ud_obj, uint64_t address);
        void cdqe      (ud_t * ud_obj, uint64_t address);
        void cmova     (ud_t * ud_obj, uint64_t address);
        void cmovbe    (ud_t * ud_obj, uint64_t address);
        void cmovnz    (ud_t * ud_obj, uint64_t address);
        void cmovs     (ud_t * ud_obj, uint64_t address);
        void cmovz     (ud_t * ud_obj, uint64_t address);
        void cmp       (ud_t * ud_obj, uint64_t address);
        void cmpxchg   (ud_t * ud_obj, uint64_t address);
        void dec       (ud_t * ud_obj, uint64_t address);
        void div       (ud_t * ud_obj, uint64_t address);
        void hlt       (ud_t * ud_obj, uint64_t address);
        void imul      (ud_t * ud_obj, uint64_t address);
        void ja        (ud_t * ud_obj, uint64_t address);
        void jae       (ud_t * ud_obj, uint64_t address);
        void jb        (ud_t * ud_obj, uint64_t address);
        void jbe       (ud_t * ud_obj, uint64_t address);
        void jg        (ud_t * ud_obj, uint64_t address);
        void jl        (ud_t * ud_obj, uint64_t address);
        void jle       (ud_t * ud_obj, uint64_t address);
        void jmp       (ud_t * ud_obj, uint64_t address);
        void jns       (ud_t * ud_obj, uint64_t address);
        void jnz       (ud_t * ud_obj, uint64_t address);
        void js        (ud_t * ud_obj, uint64_t address);
        void jz        (ud_t * ud_obj, uint64_t address);
        void lea       (ud_t * ud_obj, uint64_t address);
        void leave     (ud_t * ud_obj, uint64_t address);
        void mov       (ud_t * ud_obj, uint64_t address);
        void movd      (ud_t * ud_obj, uint64_t address);
        void movdqa    (ud_t * ud_obj, uint64_t address);
        void movdqu    (ud_t * ud_obj, uint64_t address);
        void movq      (ud_t * ud_obj, uint64_t address);
        void movqa     (ud_t * ud_obj, uint64_t address);
        void movsd     (ud_t * ud_obj, uint64_t address);
        void movsx     (ud_t * ud_obj, uint64_t address);
        void movsxd    (ud_t * ud_obj, uint64_t address);
        void movzx     (ud_t * ud_obj, uint64_t address);
        void mul       (ud_t * ud_obj, uint64_t address);
        void neg       (ud_t * ud_obj, uint64_t address);
        void nop       (ud_t * ud_obj, uint64_t address);
        void Not       (ud_t * ud_obj, uint64_t address);
        void Or        (ud_t * ud_obj, uint64_t address);
        void pcmpeqb   (ud_t * ud_obj, uint64_t address);
        void pmovmskb  (ud_t * ud_obj, uint64_t address);
        void pop       (ud_t * ud_obj, uint64_t address);
        void pshufd    (ud_t * ud_obj, uint64_t address);
        void punpcklbw (ud_t * ud_obj, uint64_t address);
        void push      (ud_t * ud_obj, uint64_t address);
        void pxor      (ud_t * ud_obj, uint64_t address);
        void rep       (ud_t * ud_obj, uint64_t address);
        void repe      (ud_t * ud_obj, uint64_t address);
        void repne     (ud_t * ud_obj, uint64_t address);
        void ret       (ud_t * ud_obj, uint64_t address);
        void rol       (ud_t * ud_obj, uint64_t address);
        void sar       (ud_t * ud_obj, uint64_t address);
        void sbb       (ud_t * ud_obj, uint64_t address);
        void scasb     (ud_t * ud_obj, uint64_t address);
        void seta      (ud_t * ud_obj, uint64_t address);
        void setg      (ud_t * ud_obj, uint64_t address);
        void setnz     (ud_t * ud_obj, uint64_t address);
        void setz      (ud_t * ud_obj, uint64_t address);
        void shl       (ud_t * ud_obj, uint64_t address);
        void shld      (ud_t * ud_obj, uint64_t address);
        void shr       (ud_t * ud_obj, uint64_t address);
        void std       (ud_t * ud_obj, uint64_t address);
        void stosd     (ud_t * ud_obj, uint64_t address);
        void sub       (ud_t * ud_obj, uint64_t address);
        void syscall   (ud_t * ud_obj, uint64_t address);
        void test      (ud_t * ud_obj, uint64_t address);
        void Xor       (ud_t * ud_obj, uint64_t address);
};

#endif
