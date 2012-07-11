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

#include "memory.h"

#include <iostream>
#include <stdexcept>
#include <sstream>

//#define DEBUG

void Memory :: destroy ()
{
    std::map <uint64_t, Page *> :: iterator it;

    for (it = pages.begin(); it != pages.end(); it++) {
        #ifdef DEBUG
        std::cerr << "Memory deleting page: " << std::hex << it->first << std::endl;
        #endif
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


void Memory :: s_data (uint64_t address, const uint8_t * data, size_t size)
{
    uint64_t page_address = g_page_address(address, size);
    this->pages[page_address]->s_data(address - page_address, data, size);
}


SymbolicValue Memory :: g_sym8 (uint64_t address)
{
    if (symbolic_memory.count(address) > 0)
        return symbolic_memory[address];
    return SymbolicValue(8, g_byte(address));
}

SymbolicValue Memory :: g_sym16 (uint64_t address)
{
    return (g_sym8(address + 1).extend(16) << SymbolicValue(8, 8))
           | g_sym8(address).extend(16);
}

SymbolicValue Memory :: g_sym32 (uint64_t address)
{
    return (g_sym16(address + 2).extend(32) << SymbolicValue(8, 16))
           | g_sym16(address).extend(32);
}

SymbolicValue Memory :: g_sym64 (uint64_t address)
{
    return (g_sym32(address + 4).extend(64) << SymbolicValue(8, 32))
           | g_sym32(address).extend(64);
}

void Memory :: s_sym8 (uint64_t address, SymbolicValue value)
{
    if (value.g_wild())
        symbolic_memory[address] = value & SymbolicValue(8, 0xff);
    else
        s_byte(address, value.g_uint64() & 0xff);
}

void Memory :: s_sym16 (uint64_t address, SymbolicValue value)
{
    s_sym8(address + 1, value >> SymbolicValue(8, 8));
    s_sym8(address, value);
}

void Memory :: s_sym32 (uint64_t address, SymbolicValue value)
{
    s_sym16(address + 2, value >> SymbolicValue(8, 16));
    s_sym16(address, value);
}

void Memory :: s_sym64 (uint64_t address, SymbolicValue value)
{
    s_sym32(address + 4, value >> SymbolicValue(8, 32));
    s_sym32(address, value);
}

void Memory :: s_page (uint64_t address, Page * page)
{
    this->pages[address] = page;
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
    try {
        uint64_t page_address = g_page_address(address, 8);
        return this->pages[page_address]->g_qword(address - page_address);
    } catch (std::exception & e) {}
    // attempt to build uint64_t one byte at a time in case
    // it is split across multiple pages
    uint64_t result = 0;
    for (int i = 0; i < 8; i++) {
        result |= ((uint64_t) g_byte(address + i)) << (i * 8);
    }
    return result;
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