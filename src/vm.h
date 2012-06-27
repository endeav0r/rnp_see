#ifndef vm_HEADER
#define vm_HEADER

#include "elf.h"
#include "kernel.h"
#include "memory.h"
#include "symbolicvalue.h"
#include "translator.h"

#include <map>

class VM {
    private :
        uint64_t   ip_id;

        Elf *      elf;
        Kernel     kernel;
        Memory     memory;
        Translator translator;

        std::map <uint64_t, SymbolicValue> variables;

        const SymbolicValue g_value (InstructionOperand operand);

        void execute (InstructionAdd    *    );
        void execute (InstructionAnd    *    );
        void execute (InstructionAssign *    );
        void execute (InstructionBrc    *    );
        void execute (InstructionCmpEq  *    );
        void execute (InstructionCmpLes *    );
        void execute (InstructionCmpLeu *    );
        void execute (InstructionCmpLts *    );
        void execute (InstructionCmpLtu *    );
        void execute (InstructionDiv    *    );
        void execute (InstructionLoad   *    );
        void execute (InstructionMod    *    );
        void execute (InstructionMul    *    );
        void execute (InstructionNot    *    );
        void execute (InstructionOr     *    );
        void execute (InstructionShl    *    );
        void execute (InstructionShr    *    );
        void execute (InstructionSignExtend *);
        void execute (InstructionStore  *    );
        void execute (InstructionSub    *    );
        void execute (InstructionSyscall *   );
        void execute (InstructionXor    *    );
        void execute (Instruction * instruction);

    public :
        VM (std::string filename);
        ~VM ();

        void step ();

        SymbolicValue g_variable (uint64_t identifier);

        void debug_x86_registers ();
        void debug_variables     ();
};

#endif