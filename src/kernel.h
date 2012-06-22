#ifndef kernel_HEADER
#define kernel_HEADER
/*
 * This is a 64-bit linux kernel
 */

#include <map>

#include "memory.h"
#include "symbolicvalue.h"

#define SYS_FUNC(SYSCALLNAME) void sys_##SYSCALLNAME \
    (std::map <uint64_t, SymbolicValue> & variables, Memory & memory);

class Kernel {
    public :
        void syscall (std::map <uint64_t, SymbolicValue> & variables, Memory & memory);

        SYS_FUNC(getpid)
};

#endif