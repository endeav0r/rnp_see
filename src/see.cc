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

    bool RUNVM = true;

    if (RUNVM) {

        VM vm(buf, filesize);

        vm.debug_x86_registers();
        
        std::cout << std::endl;

        for (int i = 0; i < 15; i++)
            vm.step();

        std::cout << std::endl;
        
        vm.debug_x86_registers();
        
    }
    else {

        Translator translator;
        
        std::list <Instruction *> instructions = translator.translate_all(0, buf, filesize);

        std::list <Instruction *> :: iterator iit;
        for (iit = instructions.begin(); iit != instructions.end(); iit++) {
            std::string tmp = (*iit)->str();
            while (tmp.length() < 74) tmp += " ";
            tmp += ud_disassemble_at_address(&(buf[(*iit)->g_address()]));
            std::cout << tmp << std::endl;
        }
        std::cout << std::endl;

    }
    
    return 0;
}
