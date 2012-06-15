/* This class is used to keep track of what indexes in a system's variables
   refer to which registers, and is used at translation time to correctly
   perform Single Static Assignment for registers in assembly code
   * 
   * This code can be made cross-platform at a later date
*/

#ifndef registers_HEADER
#define registers_HEADER

#include <vector>

#include <udis86.h>

// http://en.wikipedia.org/wiki/FLAGS_register_(computing)
// we start these at an offset so we can index them by their original values
// in udis86, or from their given names here
enum {
    REG_RAX = UD_OP_CONST + 1,
    REG_RBX,
    REG_RCX,
    REG_RDX,
    REG_RSP,
    REG_RBP,
    REG_RSI,
    REG_RDI,
    REG_R8,
    REG_R9,
    REG_R10,
    REG_R11,
    REG_R12,
    REG_R13,
    REG_R14,
    REG_R15,
    REG_CF,
    REG_PF,
    REG_AF,
    REG_ZF,
    REG_SF,
    REG_OF,
};

#define REG_OFFSET REG_RAX
#define REG_SIZE (REG_OF - REG_RAX + 1)

class Registers {
    public :
        std::vector <size_t> registers;
        
        Registers ();
        
        size_t get_register_index (int reg);
        void   set_register_index (int reg, size_t index);
        int    bits               (int reg);
        int    left_shift         (int reg);
};

#endif
