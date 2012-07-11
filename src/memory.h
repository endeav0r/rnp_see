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

#ifndef memory_HEADER
#define memory_HEADER

#include <inttypes.h>

#include <cstddef>

#include <map>
#include <unordered_set>
#include <string>

#include "page.h"
#include "symbolicvalue.h"

class Memory {
    private :

        // a set is the right data structure to use here, but we keep getting
        // weird errors at runtime
        std::map <uint64_t, int> dirty_counter;
        std::unordered_set <uint64_t> dirty;
        std::map <uint64_t, Page *> pages;

        std::map <uint64_t, SymbolicValue> symbolic_memory;

        uint64_t g_page_address (uint64_t address, int bits);
        void     dirty_page     (uint64_t address);
    public :
        Memory () {};
        Memory (std::map <uint64_t, Page *> pages) : pages(pages) {};

        void destroy ();

        Memory copy ();

        Page *    g_page (uint64_t address) { return pages[address]; }

        size_t    g_data_size (uint64_t address);
        uint8_t * g_data      (uint64_t address);
        void      s_data      (uint64_t address, const uint8_t * data, size_t size);
        void      s_page      (uint64_t address, Page * page);

        SymbolicValue g_sym8  (uint64_t address);
        SymbolicValue g_sym16 (uint64_t address);
        SymbolicValue g_sym32 (uint64_t address);
        SymbolicValue g_sym64 (uint64_t address);

        void s_sym8  (uint64_t address, SymbolicValue value);
        void s_sym16 (uint64_t address, SymbolicValue value);
        void s_sym32 (uint64_t address, SymbolicValue value);
        void s_sym64 (uint64_t address, SymbolicValue value);
        
        uint8_t  g_byte  (uint64_t address);
        uint16_t g_word  (uint64_t address);
        uint32_t g_dword (uint64_t address);
        uint64_t g_qword (uint64_t address);
        
        void s_byte  (uint64_t address, uint8_t  value);
        void s_word  (uint64_t address, uint16_t value);
        void s_dword (uint64_t address, uint32_t value);
        void s_qword (uint64_t address, uint64_t value);

        std::string memmap ();
};

#endif
