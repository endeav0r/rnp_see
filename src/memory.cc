#include "memory.h"

#include <stdexcept>

Memory :: Memory (uint64_t page_size)
{
    this->page_size = page_size;
}

void Memory :: allocate_page (uint64_t address, size_t size)
{
    this->pages[address % this->page_size] = new Page(this->page_size);
}

uint8_t Memory :: g_byte (uint64_t address)
{
    uint64_t page_address = address % this->page_size;
    return this->pages[page_address]->g_byte(address - page_address);
}

uint16_t Memory :: g_word (uint64_t address)
{
    uint64_t page_address = address % this->page_size;
    return this->pages[page_address]->g_word(address - page_address);
}

uint32_t Memory :: g_dword (uint64_t address)
{
    uint64_t page_address = address % this->page_size;
    return this->pages[page_address]->g_dword(address - page_address);
}

uint64_t Memory :: g_qword (uint64_t address)
{
    uint64_t page_address = address % this->page_size;
    return this->pages[page_address]->g_qword(address - page_address);
}

void Memory :: s_byte (uint64_t address, uint8_t value)
{
    uint64_t page_address = address % this->page_size;
    this->pages[page_address]->s_byte(address - page_address, value);
}

void Memory :: s_word (uint64_t address, uint16_t value)
{
    uint64_t page_address = address % this->page_size;
    this->pages[page_address]->s_word(address - page_address, value);
}

void Memory :: s_dword (uint64_t address, uint32_t value)
{
    uint64_t page_address = address % this->page_size;
    this->pages[page_address]->s_dword(address - page_address, value);
}

void Memory :: s_qword (uint64_t address, uint64_t value)
{
    uint64_t page_address = address % this->page_size;
    this->pages[page_address]->s_qword(address - page_address, value);
}
