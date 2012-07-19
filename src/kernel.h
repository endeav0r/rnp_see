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

        SYS_FUNC(exit)
        SYS_FUNC(exit_group)
        SYS_FUNC(fstat)
        SYS_FUNC(getpid)
        SYS_FUNC(mmap)
        SYS_FUNC(read)
        SYS_FUNC(write)
        SYS_FUNC(writev)
};

#endif