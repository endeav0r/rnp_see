#ifndef memory_HEADER
#define memory_HEADER

#include <inttypes.h>

#include <cstddef>

#include <map>
#include <string>

#include "page.h"

class Memory {
    private :
        std::map <uint64_t, Page *> pages;

        uint64_t g_page_address(uint64_t address);
    public :
        Memory () {};
        Memory (std::map <uint64_t, Page *> pages) : pages(pages) {};
        
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
