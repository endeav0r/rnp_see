#include <iostream>
#include <list>
#include <map>
#include <sstream>

#include <inttypes.h>
#include <cstdio>

#include <udis86.h>

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
    VM vm(argv[1]);

    vm.debug_x86_registers();
    
    std::cout << std::endl;

    while (true) {
        int c = getc(stdin);
        if (c == 's') vm.step();
        if (c == 'd') {
            for (int i = 0; i < 8; i++) vm.step();
        }
        if (c == 'f') {
            for (int i = 0; i < 16; i++) vm.step();
        }
        if (c == 'r') vm.debug_x86_registers();
        if (c == 'v') vm.debug_variables();
    }

    return 0;
}
