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

static uint64_t NEXT_MMAP_INIT = 0x77ff000000000000ULL;

class Kernel {
	private :
		uint64_t next_mmap; // location of next address to assign to mmap page
    public :
    	Kernel () : next_mmap(NEXT_MMAP_INIT) {}

        void syscall (std::map <uint64_t, SymbolicValue> & variables, Memory & memory);

        SYS_FUNC(fstat)
        SYS_FUNC(getpid)
        SYS_FUNC(mmap)
        SYS_FUNC(write)
};

#endif