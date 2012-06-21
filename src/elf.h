#ifndef elf_HEADER
#define elf_HEADER

#include <elf.h>
#include <inttypes.h>

#include <list>
#include <map>

#include "page.h"
#include "symbolicvalue.h"

const uint64_t ELF64_STACK_BEGIN = (1 << 30);
const size_t   ELF64_STACK_SIZE  = 0x1000;

const uint64_t ELF32_STACK_BEGIN = 0xbb000000;
const size_t   ELF32_STACK_SIZE  = 0x1000;


class Elf {
    protected :
        const uint8_t * data;
        size_t data_size;

        std::map <uint64_t, Page *> fix_pages (std::map <uint64_t, Page *> pages);
    public :
        Elf (const uint8_t * data, size_t data_size);
        ~Elf ();
        virtual uint64_t g_entry () = 0;
        virtual std::map <uint64_t, Page *>        g_pages     () = 0;
        virtual std::map <uint64_t, SymbolicValue> g_variables () = 0;
        virtual uint64_t                           g_ip_id     () = 0;

        static Elf * Get (const uint8_t * data, size_t data_size);
};

class Elf32 : public Elf {
    private :
        const Elf32_Ehdr * ehdr;
    public :
        Elf32 (const uint8_t * data, size_t data_size);

        uint64_t g_entry ();
        std::map <uint64_t, Page *>        g_pages     ();
        std::map <uint64_t, SymbolicValue> g_variables ();
        uint64_t                           g_ip_id     ();
};

class Elf64 : public Elf {
    private :
        const Elf64_Ehdr * ehdr;

        const Elf64_Shdr * g_shdr(int ndx);
        std::list <std::string> g_dependencies();
    public :
        Elf64 (const uint8_t * data, size_t data_size);

        uint64_t g_entry ();
        std::map <uint64_t, Page *>        g_pages     ();
        std::map <uint64_t, SymbolicValue> g_variables ();
        uint64_t                           g_ip_id     ();
};

#endif