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

#ifndef elf_HEADER
#define elf_HEADER

#include <elf.h>
#include <inttypes.h>

#include <list>
#include <map>

#include "memory.h"
#include "page.h"
#include "symbolicvalue.h"

const uint64_t ELF64_DEP_ADDR   = 0x7f00000000000000ULL;
const uint64_t ELF64_DEP_ADD    = 0x0010000000000000ULL;

const uint64_t ELF64_TLS_ADDR   = 0x7ffe000000000000ULL;
const uint64_t ELF64_TLS_SIZE   = 0x1000ULL;

const uint64_t ELF64_STACK_ADDR = 0x7fff000000000000ULL;
const uint64_t ELF64_STACK_SIZE = 0x4000ULL;

const uint64_t ELF64_RSP_INIT   = 0x7fff000000003000ULL;
const uint64_t ELF64_FS_INIT    = 0x7ffe000000000000ULL;

class Elf {
    protected :
        const uint8_t * data;
        size_t data_size;

        std::map <uint64_t, Page *> fix_pages (std::multimap <uint64_t, Page *> pages);
    public :
        virtual ~Elf () {};
        virtual std::string                        func_symbol (uint64_t address) = 0;
        virtual uint64_t                           g_entry     () = 0;
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
        ~Elf32 () {}
};

class Elf64Symbol {
    private :
        std::string name;
        uint64_t offset;
        uint64_t address;
        uint8_t  binding;
        uint16_t shndx;
    public :
        Elf64Symbol (const std::string name,
                     uint64_t offset,
                     uint64_t address,
                     uint8_t  binding,
                     uint16_t shndx)
            : name(name), offset(offset), address(address), binding(binding), shndx(shndx) {}
        Elf64Symbol () : name(""), offset(0), address(0), binding(0), shndx(0) {}
        const std::string g_name    () const { return name; }
        const uint64_t    g_offset  () const { return offset; }
        const uint64_t    g_address () const { return address; }
        const uint8_t     g_binding () const { return binding; }
        const uint16_t    g_shndx   () const { return shndx; }

        Elf64Symbol operator = (const Elf64Symbol & rhs) {
            name = rhs.name;
            offset = rhs.offset;
            address = rhs.address;
            binding = rhs.binding;
            shndx = rhs.shndx;
            return *this;
        }
};

class Elf64Relocation {
    private :
        const Elf64Symbol symbol;
        const uint64_t    offset;
        const uint64_t    addend;
        const uint8_t     type;
    public :
        Elf64Relocation (const Elf64Symbol & symbol, const uint64_t offset, const uint8_t type)
            : symbol(symbol), offset(offset), addend(0), type(type) {}
        Elf64Relocation (const Elf64Symbol & symbol,
                         const uint64_t offset,
                         const uint64_t addend,
                         const uint8_t  type)
            : symbol(symbol), offset(offset), addend(addend), type(type) {}
        const Elf64Symbol & g_symbol () const { return symbol; }
        const uint64_t      g_offset () const { return offset; }
        const uint64_t      g_addend () const { return addend; }
        const uint8_t       g_type   () const { return type;   }
};

class Elf64 : public Elf {
    private :
        std::list <Elf64 *> dependencies;
        const Elf64_Ehdr *  ehdr;
        const std::string   filename;
        const uint64_t      offset; // virtual address offset
        bool                dependency;

        std::list <Elf64Symbol> global_symbols;

        void load ();

        const std::string           g_strtab_str   (size_t strtab_index, size_t offset);
        const Elf64_Shdr *          g_shdr         (int ndx);
        const Elf64Symbol           g_symbol       (size_t symtab_index, size_t symbol_index);
        std::list <std::string>     g_dependencies ();
        std::list <Elf64Symbol>     g_symbols      ();
        std::list <Elf64Relocation> g_relocations  ();
        std::map <uint64_t, Page *> g_pages        ();

        std::list <Elf64Symbol> find_symbols      (const std::string name);
        std::list <Elf64Symbol> find_symbols_deps (const std::string name);
        const Elf64Symbol       find_symbol_glob  (const std::string name, Elf64 & elf);

        // finds DT_NEEDED entries and loads creates Elf64 instances of them
        // for the dependencies list
        void load_dependencies ();

        // does this elf's relocations for the passed memory. The passed Elf64
        // should be the "initial" Elf64 in which all the dependencies are
        // stored
        void patch_relocations (Memory & memory, Elf64 & elf);

        // finds the first .tdata section and copies it into the given page.
        // Returns true if found and copied, false otherwise
        bool set_tls_memory    (Page * page);
    public :
        Elf64  (const std::string filename);
        Elf64  (const std::string filename, uint64_t offset);
        ~Elf64 ();

        std::string                        func_symbol (uint64_t address);
        std::string                        g_filename  () { return filename; };
        uint64_t                           g_entry     ();
        Memory                             g_memory    ();
        std::map <uint64_t, SymbolicValue> g_variables ();
        uint64_t                           g_ip_id     ();
};

#endif