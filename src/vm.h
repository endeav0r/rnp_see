#ifndef vm_HEADER
#define vm_HEADER

#include "memory.h"
#include "symbolicvalue.h"
#include "translator.h"

#include <map>

class VM {
    private :
        Translator translator;
        Memory     memory;
        uint64_t   ip_id;
        std::map <uint64_t, SymbolicValue> variables;

        const SymbolicValue g_value (InstructionOperand operand);

        void execute (InstructionAdd    *    );
        void execute (InstructionAnd    *    );
        void execute (InstructionAssign *    );
        void execute (InstructionBrc    *    );
        void execute (InstructionCmpEq  *    );
        void execute (InstructionCmpLts *    );
        void execute (InstructionLoad   *    );
        void execute (InstructionSignExtend *);
        void execute (InstructionStore  *    );
        void execute (InstructionSub    *    );
        void execute (InstructionXor    *    );
        void execute (Instruction * instruction);

    public :
        VM (const uint8_t * data, size_t data_size);

        void step ();

        SymbolicValue g_variable (uint64_t identifier);

        void debug_x86_registers ();
        void debug_variables     ();
};

#endif