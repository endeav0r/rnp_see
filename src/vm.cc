#include "vm.h"

#include <iostream>

#include "elf.h"

// this works like a loader
VM :: VM (const uint8_t * data, size_t data_size)
{
	Elf * elf = Elf :: Get(data, data_size);

	memory = Memory(elf->g_pages());

	std::cout << memory.memmap();
}