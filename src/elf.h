#ifndef elf_HEADER
#define elf_HEADER

#include <elf.h>
#include <inttypes.h>

#include <map>

#include "page.h"


class Elf {
    protected :
        const uint8_t * data;
        size_t data_size;

        std::map <uint64_t, Page *> discrete_pages (std::map <uint64_t, Page *> pages);
    public :
        Elf (const uint8_t * data, size_t data_size);
        ~Elf ();
        virtual uint64_t g_entry () = 0;
        virtual std::map <uint64_t, Page *> g_pages () = 0;

        static Elf * Get (const uint8_t * data, size_t data_size);
};

class Elf32 : public Elf {
    private :
        const Elf32_Ehdr * ehdr;
    public :
        Elf32 (const uint8_t * data, size_t data_size);

        uint64_t g_entry ();
        std::map <uint64_t, Page *> g_pages ();
};

class Elf64 : public Elf {
    private :
        const Elf64_Ehdr * ehdr;
    public :
        Elf64 (const uint8_t * data, size_t data_size);

        uint64_t g_entry ();
        std::map <uint64_t, Page *> g_pages();
};

#endif