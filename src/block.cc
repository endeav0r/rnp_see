#include "block.h"

#include <algorithm>
#include <list>
#include <vector>


/*
 * lower_bound and upper_bound are both inclusive
 * to accept targets for addresses 0x0, 0x1 and 0x2 you would pass (0x0, 0x2)
 */
void Block :: find_successors (uint64_t lower_bound, uint64_t upper_bound)
{
    successors.clear();
    
    InstructionRet * ret = dynamic_cast<InstructionRet *>(instructions.back());
    if (ret) return;
    
    InstructionBrc * brc = dynamic_cast<InstructionBrc *>(instructions.back());
    if (brc) {
        InstructionOperand dst = brc->g_dst();
        // destination is constant
        if (dst.g_type() == OPTYPE_CONSTANT) {
            // add branch target
            dst.sign();
            uint64_t target_address = brc->g_address() + brc->g_size();
            target_address          = (int64_t) target_address + (int64_t) dst.g_value();
            if ((target_address >= lower_bound) && (target_address <= upper_bound))
                successors.push_back(target_address);
        }
    }
    
    Instruction * instruction = dynamic_cast<Instruction *>(instructions.back());
    if (instruction == NULL) return;
    uint64_t target_address = instruction->g_address() + instruction->g_size();
    if ((target_address >= lower_bound) && (target_address <= upper_bound))
        successors.push_back(target_address);
    
}

std::list <uint64_t> Blocks :: find_block_beginnings (std::list <Instruction *> instructions,
                                                      uint64_t lower_bound,
                                                      uint64_t upper_bound)
{
    std::list <uint64_t> start_points;
    
    // locate start of all blocks, starting with first block
    start_points.push_back(instructions.front()->g_address());
    
    std::list <Instruction *> :: iterator it;
    for (it = instructions.begin(); it != instructions.end(); it++) {
        
        InstructionBrc * brc = dynamic_cast<InstructionBrc *>(*it);
        if (brc != NULL) {
        InstructionOperand dst = brc->g_dst();
            if (dst.g_type() & OPTYPE_CONSTANT) {
                dst.sign();
                start_points.push_back((int64_t) (brc->g_address() + brc->g_size()) + (int64_t) dst.g_value());
            }
            start_points.push_back(brc->g_address() + brc->g_size());
        }
        
        InstructionRet * ret = dynamic_cast<InstructionRet *>(*it);
        if (ret != NULL) start_points.push_back(ret->g_address() + ret->g_size());
    }
    
    // remove duplicates
    start_points.sort();
    start_points.unique();
    
    // bounds check
    std::list <uint64_t> :: iterator sit;
    for (sit = start_points.begin(); sit != start_points.end();) {
		std::cout << *sit << std::endl;
        if ((*sit < lower_bound) || (*sit > upper_bound)) {
			std::cout << "eliminating " << *sit << std::endl;
            sit = start_points.erase(sit);
        }
        else sit++;
    }
    
    return start_points;
}


// this is terribly inefficient, but accurate
void Blocks :: make_blocks (std::list <Instruction *> instructions, std::list <uint64_t> start_points_list)
{   
    std::vector <uint64_t> start_points(start_points_list.begin(), start_points_list.end());
    
    std::list <Instruction *> block_instructions;
    std::list <Instruction *> :: iterator iit;
    
    for (size_t sit = 0; sit + 1 < start_points.size(); sit++) {
        // find all instructions in this block
        block_instructions.clear();
        for (iit = instructions.begin(); iit != instructions.end(); iit++) {
            if (((*iit)->g_address() >= start_points[sit]) && ((*iit)->g_address() < start_points[sit + 1]))
                block_instructions.push_back(*iit);
        }
        this->blocks[start_points[sit]] = Block(block_instructions);
    }
    
    // instructions for last block
    block_instructions.clear();
    for (iit = instructions.begin(); iit != instructions.end(); iit++) {
        if ((*iit)->g_address() >= start_points.back())
            block_instructions.push_back(*iit);
    }
    this->blocks[start_points.back()] = Block(block_instructions);
}


void Blocks :: control_flow (uint64_t lower_bound, uint64_t upper_bound)
{
    std::map <uint64_t, Block> :: iterator bit;
    for (bit = blocks.begin(); bit != blocks.end(); bit++) {
        /* EMPTY BLOCKS ARE A SPECIAL CASE */
        bit->second.find_successors(lower_bound, upper_bound);
        std::vector <uint64_t> :: iterator sit;
        for (sit = bit->second.successors.begin(); sit!= bit->second.successors.end(); sit++) {
            blocks[*sit].predecessors.push_back(bit->first);
        }
    }
}


Blocks :: Blocks (std::list <Instruction *> instructions)
{
    uint64_t lower_bound = instructions.front()->g_address();
    uint64_t upper_bound = instructions.back()->g_address();
    
    make_blocks(instructions, find_block_beginnings(instructions, lower_bound, upper_bound));
    control_flow(instructions.front()->g_address(), instructions.back()->g_address());
}
