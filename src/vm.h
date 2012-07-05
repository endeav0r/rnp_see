/*
    Copyright 2012 Alex Eubanks (endeavor[at]rainbowsandpwnies.com)

    This file is part of rnp_see ( http://github.com/endeav0r/rnp_see/ )

    rnp_see is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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

        Loader *   loader;
        Kernel     kernel;
        Memory     memory;
        Translator translator;
        bool       delete_loader;

        std::map <uint64_t, SymbolicValue> variables;

        const SymbolicValue g_value (InstructionOperand operand);

        void init ();

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
        VM (Loader * loader);
        VM (Loader * loader, bool delete_loader);
        VM () : loader(NULL), delete_loader(false) { delete_loader = false; }
        ~VM ();

        void step ();

        SymbolicValue g_variable (uint64_t identifier);

        void debug_x86_registers ();
        void debug_variables     ();
};

#endif