#ifndef elf_HEADER
#define elf_HEADER

#include <elf.h>
#include <inttypes.h>

#include <list>
#include <map>

#include "memory.h"
#include "page.h"
#include "symbolicvalue.h"



class Elf {
    protected :
        const uint8_t * data;
        size_t data_size;

        std::map <uint64_t, Page *> fix_pages (std::multimap <uint64_t, Page *> pages);
    public :
        Elf ();
        ~Elf ();
        virtual uint64_t g_entry () = 0;
        virtual Memory                             g_memory    () = 0;
        virtual std::map <uint64_t, SymbolicValue> g_variables () = 0;
        virtual uint64_t                           g_ip_id     () = 0;

        static Elf * Get (std::string filename);
};

class Elf32 : public Elf {
    private :
        const Elf32_Ehdr * ehdr;
    public :
        Elf32 (std::string filename);
};

class Elf64Symbol {
    private :
        const std::string name;
        const uint64_t offset;
        const uint64_t address;
    public :
        Elf64Symbol (const std::string name, uint64_t offset, uint64_t address)
            : name(name), offset(offset), address(address) {}
        Elf64Symbol () : name(""), offset(0), address(0) {}
        const std::string g_name    () const { return name; }
        const uint64_t    g_offset  () const { return offset; }
        const uint64_t    g_address () const { return address; }
};

class Elf64Relocation {
    private :
        const Elf64Symbol symbol;
        const uint64_t    offset;
        const uint64_t    addend;
    public :
        Elf64Relocation (const Elf64Symbol & symbol, const uint64_t offset)
            : symbol(symbol), offset(offset), addend(0) {}
        Elf64Relocation (const Elf64Symbol & symbol, const uint64_t offset, const uint64_t addend)
            : symbol(symbol), offset(offset), addend(addend) {}
        const Elf64Symbol & g_symbol () const { return symbol; }
        const uint64_t      g_offset () const { return offset; }
        const uint64_t      g_addend () const { return addend; }
};

class Elf64 : public Elf {
    private :
        std::list <Elf64 *> dependencies;
        const Elf64_Ehdr * ehdr;
        const std::string  filename;
        const uint64_t     offset; // virtual address offset
        bool               dependency;

        void load ();

        const std::string           g_strtab_str   (size_t strtab_index, size_t offset);
        const Elf64_Shdr *          g_shdr         (int ndx);
        const Elf64Symbol           g_symbol       (size_t symtab_index, size_t symbol_index);
        std::list <std::string>     g_dependencies ();
        std::list <Elf64Symbol>     g_symbols      ();
        std::list <Elf64Relocation> g_relocations  ();
        std::map <uint64_t, Page *> g_pages        ();

        const Elf64Symbol           find_symbol      (const std::string name);
        const Elf64Symbol           find_symbol_deps (const std::string name);

        void load_dependencies ();
        void patch_relocations (Memory & memory);
    public :
        Elf64 (const std::string filename);
        Elf64 (const std::string filename, uint64_t offset);

        uint64_t g_entry ();
        Memory                             g_memory    ();
        std::map <uint64_t, SymbolicValue> g_variables ();
        uint64_t                           g_ip_id     ();
};

#endif