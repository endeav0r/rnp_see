#include <iostream>
#include <list>
#include <map>
#include <sstream>

#include <inttypes.h>
#include <stdio.h>

#include <udis86.h>

#include "block.h"
#include "instruction.h"
#include "translator.h"

#include "vm.h"

std::string ud_disassemble_at_address (unsigned char * data)
{
    ud_t ud_obj;
    
    ud_init(&ud_obj);
    ud_set_mode(&ud_obj, 64);
    ud_set_syntax(&ud_obj, UD_SYN_INTEL);
    ud_set_input_buffer(&ud_obj, data, 16);
    
    ud_disassemble(&ud_obj);
    
    return ud_insn_asm(&ud_obj);
}


int main (int argc, char * argv[])
{   
    uint8_t * buf;
    FILE * fh;
    size_t filesize;
    
    fh = fopen(argv[1], "rb");
    
    fseek(fh, 0, SEEK_END);
    filesize = ftell(fh);
    fseek(fh, 0, SEEK_SET);
    
    buf = new uint8_t[filesize];
    
    fread(buf, 1, filesize, fh);
    
    fclose(fh);

    VM vm(buf, filesize);

    vm.debug_x86_registers();
    
    std::cout << std::endl;

    vm.step();
    vm.step();
    vm.step();

    std::cout << std::endl;
    
    vm.debug_x86_registers();

    /*
    Translator translator;
    
    std::list <Instruction *> instructions = translator.translate(0, buf, filesize);
    std::list <Instruction *> :: iterator iit;
    
    Blocks blocks(instructions);
    
    std::map <uint64_t, Block> b = blocks.g_blocks();
    std::map <uint64_t, Block> :: iterator bit;
    for (bit = b.begin(); bit != b.end(); bit++) {
        std::cout << "BLOCK " << std::hex << (*bit).first << " ";
        // print predecessors and successors
        std::stringstream ps;
        ps << "{ ";
        for (size_t i = 0; i < (*bit).second.predecessors.size(); i++) {
            ps << std::hex << (*bit).second.predecessors[i] << " ";
        }
        ps << "} => { ";
        for (size_t i = 0; i < (*bit).second.successors.size(); i++) {
            ps << std::hex << (*bit).second.successors[i] << " ";
        }
        ps << "}";
        std::cout << ps.str() << std::endl;
        
        // print instructions
        std::list <Instruction *> instructions = (*bit).second.g_instructions();
        std::list <Instruction *> :: iterator iit;
        for (iit = instructions.begin(); iit != instructions.end(); iit++) {
            std::string tmp = (*iit)->to_str();
            while (tmp.length() < 74) tmp += " ";
            tmp += ud_disassemble_at_address(&(buf[(*iit)->g_address()]));
            std::cout << tmp << std::endl;
        }
        std::cout << std::endl;
    }
    */
    
    return 0;
}
