#ifndef memory_HEADER
#define memory_HEADER

#include <inttypes.h>

#include <cstddef>

#include <map>
#include <unordered_set>
#include <string>

#include "page.h"

class Memory {
    private :

        // a set is the right data structure to use here, but we keep getting
        // weird errors at runtime
        std::map <uint64_t, int> dirty_counter;
        std::unordered_set <uint64_t> dirty;
        std::map <uint64_t, Page *> pages;

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
