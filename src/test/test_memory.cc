#include <cassert>
#include <inttypes.h>
#include <iostream>
#include <map>

#include "../memory.h"
#include "../page.h"

void test_1 ()
{
	std::map <uint64_t, Page *> pages;

	pages[0]   = new Page(128);
	pages[128] = new Page(128);

	Memory memory(pages);

	for (int i = 0; i < 256; i++) {
		memory.s_byte(i, i);
	}
	for (int i = 0; i < 256; i++) {
		assert(memory.g_byte(i) == i);
	}

	memory.destroy();
}

void test_2 ()
{
	std::map <uint64_t, Page *> pages;

	pages[0]   = new Page(128);
	pages[128] = new Page(128);

	Memory memory(pages);

	for (int i = 0; i < 128; i++) {
		memory.s_word(i*2, i);
	}
	for (int i = 0; i < 128; i++) {
		assert(memory.g_word(i*2) == i);
	}

	memory.destroy();
}

void test_3 ()
{
	std::map <uint64_t, Page *> pages;

	pages[0]   = new Page(128);
	pages[128] = new Page(128);

	Memory memory(pages);

	for (uint32_t i = 0; i < 64; i++) {
		memory.s_dword(i*4, i);
	}
	for (uint32_t i = 0; i < 64; i++) {
		assert(memory.g_dword(i*4) == i);
	}

	memory.destroy();
}

void test_4 ()
{
	std::map <uint64_t, Page *> pages;

	pages[0]   = new Page(128);
	pages[128] = new Page(128);

	Memory memory(pages);

	for (uint32_t i = 0; i < 32; i++) {
		memory.s_qword(i*8, i);
	}
	for (uint32_t i = 0; i < 32; i++) {
		assert(memory.g_qword(i*8) == i);
	}

	memory.destroy();
}

int main (int argc, char * argv[])
{
	test_1(); std::cout << "test_1 pass" << std::endl;
	test_2(); std::cout << "test_2 pass" << std::endl;
	test_3(); std::cout << "test_3 pass" << std::endl;
	test_1(); std::cout << "test_4 pass" << std::endl;

	return 0;
}