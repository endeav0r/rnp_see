#ifndef memory_HEADER
#define memory_HEADER

#include <map>
#include <inttypes.h>
#include <cstddef>

#include "page.h"

class Memory {
    private :
        std::map <uint64_t, Page *> pages;
        uint64_t page_size;
    public :
        Memory (uint64_t page_size);
        
        void allocate_page (uint64_t address, size_t size);
        
        uint8_t  g_byte  (uint64_t address);
        uint16_t g_word  (uint64_t address);
        uint32_t g_dword (uint64_t address);
        uint64_t g_qword (uint64_t address);
        
        void s_byte  (uint64_t address, uint8_t  value);
        void s_word  (uint64_t address, uint16_t value);
        void s_dword (uint64_t address, uint32_t value);
        void s_qword (uint64_t address, uint64_t value);
};

#endif
