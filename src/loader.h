#ifndef loader_HEADER
#define loader_HEADER

#include <inttypes.h>

#include <map>
#include <string>

#include "memory.h"
#include "symbolicvalue.h"

class Loader {
	public :
		virtual std::string                        func_symbol (uint64_t address) = 0;
		virtual Memory                             g_memory    () = 0;
		virtual std::map <uint64_t, SymbolicValue> g_variables () = 0;
		virtual uint64_t                           g_ip_id     () = 0;
};

#endif