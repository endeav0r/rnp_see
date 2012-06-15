#ifndef block_HEADER
#define block_HEADER

#include <inttypes.h>

#include <list>
#include <map>
#include <vector>

#include "instruction.h"

class Block {
    private :
        std::list <Instruction *> instructions;
    
    public :
        std::vector <uint64_t> predecessors;
        std::vector <uint64_t> successors;
        
        Block () {}
        Block (std::list <Instruction *> instructions)
            : instructions(instructions) {}
        std::list <Instruction *> g_instructions () { return instructions; }
        
        void find_successors(uint64_t lower_bound, uint64_t upper_bound);
};

class Blocks {
    private :
        std::map  <uint64_t, Block> blocks;
        std::list <uint64_t> find_block_beginnings (std::list <Instruction *> instructions,
                                                    uint64_t lower_bound, uint64_t upper_bound);
        
        void                 make_blocks           (std::list <Instruction *> instructions,
                                                    std::list <uint64_t>      start_points);
        void                 control_flow          (uint64_t lower_bound, uint64_t upper_bound);
    public :
        Blocks (std::list <Instruction *> instructions);
        
        std::map <uint64_t, Block> g_blocks ()                 { return blocks; }
        Block &                    g_block  (uint64_t address) { return blocks[address]; }
};

#endif
