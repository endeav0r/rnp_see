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
        std::list <Instruction *> translate_all (uint64_t address, uint8_t * data, size_t size);
        
        int register_bits (int reg);
        int register_to64 (int reg);
        
        uint64_t           operand_lval (int bits, struct ud_operand operand);
        void               operand_set (ud_t * ud_obj, int operand_i, uint64_t address, InstructionOperand & value);
        InstructionOperand operand_get (ud_t * ud_obj, int operand_i, uint64_t address);
        InstructionOperand operand     (ud_t * ud_obj, int operand_i, uint64_t address);
        
        void add_flags (ud_t * ud_obj, uint64_t address);
        
        void add     (ud_t * ud_obj, uint64_t address);
        void And     (ud_t * ud_obj, uint64_t address);
        void call    (ud_t * ud_obj, uint64_t address);
        void cmp     (ud_t * ud_obj, uint64_t address);
        void hlt     (ud_t * ud_obj, uint64_t address);
        void ja      (ud_t * ud_obj, uint64_t address);
        void jl      (ud_t * ud_obj, uint64_t address);
        void jmp     (ud_t * ud_obj, uint64_t address);
        void jnz     (ud_t * ud_obj, uint64_t address);
        void jz      (ud_t * ud_obj, uint64_t address);
        void lea     (ud_t * ud_obj, uint64_t address);
        void leave   (ud_t * ud_obj, uint64_t address);
        void mov     (ud_t * ud_obj, uint64_t address);
        void movsxd  (ud_t * ud_obj, uint64_t address);
        void movzx   (ud_t * ud_obj, uint64_t address);
        void nop     (ud_t * ud_obj, uint64_t address);
        void pop     (ud_t * ud_obj, uint64_t address);
        void push    (ud_t * ud_obj, uint64_t address);
        void ret     (ud_t * ud_obj, uint64_t address);
        void sar     (ud_t * ud_obj, uint64_t address);
        void shr     (ud_t * ud_obj, uint64_t address);
        void sub     (ud_t * ud_obj, uint64_t address);
        void syscall (ud_t * ud_obj, uint64_t address);
        void test    (ud_t * ud_obj, uint64_t address);
        void Xor     (ud_t * ud_obj, uint64_t address);
};

#endif
