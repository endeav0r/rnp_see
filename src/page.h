#ifndef page_HEADER
#define page_HEADER

#define PAGE_SIZE 4096

#include <inttypes.h>
#include <cstddef>

class Page {
    private :
        uint8_t * data;
        size_t size;
        Page * parent;
        
        int references;
        
    public :
        Page (size_t size);
        
        void destroy ();
        
        void set_parent (Page * parent);
        
        uint8_t  g_byte  (size_t offset);
        uint16_t g_word  (size_t offset);
        uint32_t g_dword (size_t offset);
        uint64_t g_qword (size_t offset);
        
        void s_byte  (size_t offset, uint8_t  value);
        void s_word  (size_t offset, uint16_t value);
        void s_dword (size_t offset, uint32_t value);
        void s_qword (size_t offset, uint64_t value);
};

#endif
