#include "memory.h"

#include <iostream>
#include <stdexcept>
#include <sstream>

//#define DEBUG

Memory :: ~Memory ()
{
    std::map <uint64_t, Page *> :: iterator it;

    for (it = pages.begin(); it != pages.end(); it++) {
        // if this is dirty, destroy it once for everytime in dirty_counter
        if (dirty.count(it->first) > 0) {
            for (int i = 0; i < dirty_counter[it->first]; i++) {
                #ifdef DEBUG
                std::cerr << "~Memory dirty page: " 
                          << std::hex << it->first << std::endl;
                #endif
                pages[it->first] = pages[it->first]->destroy();
            }
        }
        pages[it->first] = pages[it->first]->destroy();
    }
}

Memory Memory :: copy ()
{
    dirty.clear();

    return Memory(pages);
}

uint64_t Memory :: g_page_address (uint64_t address, int bits)
{
    std::map <uint64_t, Page *> :: iterator it;
    it = pages.upper_bound(address);
    if (it != pages.begin()) {
        it--;
        if (it->first + it->second->g_size() >= address + bits)
            return it->first;
    }

    std::stringstream ss;
    ss << "memory address dereferenced but not paged: 0x" << std::hex << address;
    throw std::runtime_error(ss.str());
}

void Memory :: dirty_page (uint64_t address)
{
    if (dirty.count(address) == 0) {
        if (dirty_counter.count(address) == 0)
            dirty_counter[address] = 1;
        else
            dirty_counter[address]++;
        dirty.insert(address);
        pages[address] = pages[address]->make_child();
    }
}

size_t Memory :: g_data_size (uint64_t address)
{
    uint64_t page_address = g_page_address(address, 0);
    return this->pages[page_address]->g_size() - (address - page_address);
}

uint8_t * Memory :: g_data (uint64_t address)
{
    uint64_t page_address = g_page_address(address, g_data_size(address));
    return this->pages[page_address]->g_data(address - page_address);
}

uint8_t Memory :: g_byte (uint64_t address)
{
    uint64_t page_address = g_page_address(address, 1);
    return this->pages[page_address]->g_byte(address - page_address);
}

uint16_t Memory :: g_word (uint64_t address)
{
    uint64_t page_address = g_page_address(address, 2);
    return this->pages[page_address]->g_word(address - page_address);
}

uint32_t Memory :: g_dword (uint64_t address)
{
    uint64_t page_address = g_page_address(address, 4);
    return this->pages[page_address]->g_dword(address - page_address);
}

uint64_t Memory :: g_qword (uint64_t address)
{
    uint64_t page_address = g_page_address(address, 8);
    return this->pages[page_address]->g_qword(address - page_address);
}

void Memory :: s_byte (uint64_t address, uint8_t value)
{
    uint64_t page_address = g_page_address(address, 1);
    dirty_page(page_address);
    this->pages[page_address]->s_byte(address - page_address, value);
}

void Memory :: s_word (uint64_t address, uint16_t value)
{
    uint64_t page_address = g_page_address(address, 2);
    dirty_page(page_address);
    this->pages[page_address]->s_word(address - page_address, value);
}

void Memory :: s_dword (uint64_t address, uint32_t value)
{
    uint64_t page_address = g_page_address(address, 4);
    dirty_page(page_address);
    this->pages[page_address]->s_dword(address - page_address, value);
}

void Memory :: s_qword (uint64_t address, uint64_t value)
{
    uint64_t page_address = g_page_address(address, 8);
    dirty_page(page_address);
    this->pages[page_address]->s_qword(address - page_address, value);
}

std::string Memory :: memmap ()
{
    std::stringstream ss;
    std::map <uint64_t, Page *> :: iterator it;

    for (it = pages.begin(); it != pages.end(); it++) {
        ss << std::hex << it->first
           << "\t" << "size=" << std::hex << it->second->g_size()
           << std::endl;
    }

    return ss.str();
}