#ifndef lx86_HEADER
#define lx86_HEADER

#include <list>
#include <map>
#include <sys/ptrace.h>

#include "elf.h"
#include "loader.h"
#include "memory.h"
#include "symbolicvalue.h"

class Lx86 : public Loader {
	private :
		// we keep all elfs here so we can call func_symbol on them
		std::list <Elf *> elfs;
		pid_t pid;

	public :
		Lx86  (std::string filename);
		~Lx86 ();

		std::string                        func_symbol (uint64_t address);
		Memory                             g_memory    ();
		std::map <uint64_t, SymbolicValue> g_variables ();
		uint64_t                           g_ip_id     ();

		// these are special methods used for debugging
		void step();
		void g_regs (struct user_regs_struct * regs);
};

#endif