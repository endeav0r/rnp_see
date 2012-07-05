#ifndef lx86_HEADER
#define lx86_HEADER

#include <map>
#include <sys/ptrace.h>

#include "elf.h"
#include "loader.h"
#include "memory.h"
#include "symbolicvalue.h"

class Lx86 : public Loader {
	private :
		Elf * elf;
		pid_t pid;

	public :
		Lx86  (std::string filename);
		~Lx86 ();

		std::string                        func_symbol (uint64_t address);
		uint64_t                           g_entry     ();
		Memory                             g_memory    ();
		std::map <uint64_t, SymbolicValue> g_variables ();
		uint64_t                           g_ip_id     ();
};

#endif