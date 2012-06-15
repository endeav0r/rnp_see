#ifndef vm_HEADER
#define vm_HEADER

#include "translator.h"
#include "memory.h"

class VM {
	private :
		Translator translator;
		Memory     memory;
	public :
		VM (const uint8_t * data, size_t data_size);
};

#endif