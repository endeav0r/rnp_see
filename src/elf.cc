#include "elf.h"

#include <cstdio>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <sstream>

#include "instruction.h"

#define DEBUG
//#define DEBUG_RELO

/*
 *  Performs a lot of checking/cleaning of pages.
 *  Linkers often times overlap pages in the elf binary. This makes our job
 *  much more difficult. To deal with this, we make sure pages don't overlap.
 *  Pages must, by spec, be given in increasing order of virtual address, which
 *  makes our job a lot easier.
 */
std::map <uint64_t, Page *> Elf :: fix_pages (std::multimap <uint64_t, Page *> pages)
{
    std::map <uint64_t, Page *> final_pages;

    // no overlapping pages
    // this just fixes the pages in pages, but does not add them to final_pages
    std::multimap <uint64_t, Page *> :: iterator it = pages.begin();
    std::multimap <uint64_t, Page *> :: iterator next = it; next++;
    while (next != pages.end()) {
        // this page overflows in to the next page
        if (it->first + it->second->g_size() > next->first) {
            // if the next page is in the middle of this page, we split this page around
            // the next page
            if (it->first + it->second->g_size() > next->first + next->second->g_size()) {
                uint64_t nnext = next->first + next->second->g_size();
                // find the first address which is not a page and hold that value
                // in nnext
                while (pages.count(nnext) > 0) {
                    std::pair<std::multimap <uint64_t, Page *> :: iterator,
                              std::multimap <uint64_t, Page *> :: iterator> nnext_range;
                    nnext_range = pages.equal_range(nnext);
                    std::multimap<uint64_t, Page *> :: iterator nnit;
                    uint64_t greatest_size = 0;
                    for (nnit = nnext_range.first; nnit != nnext_range.second; nnit++) {
                        if (nnit == it) continue;
                        if (nnit->second->g_size() > greatest_size) {
                            greatest_size = nnit->second->g_size();
                        }
                    }
                    // account for zero sized next pages
                    if (greatest_size == 0) break;
                    nnext += greatest_size;
                }
                // make sure we still overlap and we need to create a new page
                // after the middle page
                if (it->first + it->second->g_size() > nnext) {
                    size_t new_offset = nnext - it->first;
                    // address of old page + size of old page - start of new page
                    size_t new_size   = it->second->g_size() - new_offset;
                    std::pair<uint64_t, Page *> p(nnext, new Page(new_size,
                                                                  it->second->g_data(new_offset)));
                    pages.insert(p);
                }
            }
            // now resize the page so it doesn't overlap
            it->second->resize(next->first - it->first);
        }
        it++;
        next++;
    }

    // add the pages to final_pages
    for (it = pages.begin(); it != pages.end(); it++) {
        final_pages[it->first] = it->second;
    }

    // no empty pages
    std::map <uint64_t, Page *> :: iterator fpit;
    for (fpit = final_pages.begin(); fpit != final_pages.end(); fpit++) {
        if (fpit->second->g_size() == 0) {
            fpit->second->destroy();
            fpit = final_pages.erase(fpit);
        }
    }

    return final_pages;
}

Elf * Elf :: Get (std::string filename)
{
    FILE * fh;
    size_t filesize;
    uint8_t tmp[1024];

    fh = fopen(filename.c_str(), "rb");
    if (fh == NULL) throw std::runtime_error("could not open file: " + filename);

    fseek(fh, 0, SEEK_END);
    filesize = ftell(fh);
    fseek(fh, 0, SEEK_SET);

    if (filesize < 1024) throw std::runtime_error("elf < 1kb?");

    fread(tmp, 1, 1024, fh);

    fclose(fh);

    if (    (tmp[EI_MAG0] == ELFMAG0)
         && (tmp[EI_MAG1] == ELFMAG1)
         && (tmp[EI_MAG2] == ELFMAG2)
         && (tmp[EI_MAG3] == ELFMAG3)) {
        switch (tmp[EI_CLASS]) {
        case ELFCLASS64 : return new Elf64(filename);
        }
        throw std::runtime_error("invalid elf, EI_CLASS no match");
    }
    else throw std::runtime_error("invalid elf, EI_MAG no match");

    return NULL;
}


/**************
 * Elf32      *
 *************/


Elf32 :: Elf32 (std::string filename)
{
}


/**************
 * Elf64      *
 *************/


void Elf64 :: load ()
{
    FILE * fh;

    fh = fopen(filename.c_str(), "rb");
    if (fh == NULL) throw std::runtime_error("could not open file: " + filename);

    fseek(fh, 0, SEEK_END);
    data_size = ftell(fh);
    fseek(fh, 0, SEEK_SET);

    data = new uint8_t[data_size];
    fread((void *) data, 1, data_size, fh);

    fclose(fh);

    ehdr = (const Elf64_Ehdr *) this->data;

    if (not dependency) {
        load_dependencies();
    }
}

const std::string Elf64 :: g_strtab_str (size_t strtab_index, size_t offset)
{
    const Elf64_Shdr * shdr = g_shdr(strtab_index);

    return (const char *) &(data[shdr->sh_offset + offset]);
}


const Elf64Symbol Elf64 :: g_symbol (size_t symtab_index, size_t symbol_index)
{
    const Elf64_Shdr * shdr = g_shdr(symtab_index);

    if (    (shdr->sh_type != SHT_SYMTAB)
         && (shdr->sh_type != SHT_DYNSYM))
        throw std::runtime_error("requested symbol from non-symbol section");

    size_t offset = shdr->sh_offset + (shdr->sh_entsize * symbol_index);
    const Elf64_Sym * sym = (const Elf64_Sym *) &(data[offset]);
    const std::string name = g_strtab_str(shdr->sh_link, sym->st_name);

    return Elf64Symbol(name, sym->st_value, sym->st_value + this->offset,
                       ELF64_ST_BIND(sym->st_info), sym->st_shndx);
}


const Elf64_Shdr * Elf64 :: g_shdr (int ndx)
{
    size_t offset = ehdr->e_shoff + (ehdr->e_shentsize * ndx);
    return (const Elf64_Shdr *) &(data[offset]);
}


std::list <std::string> Elf64 :: g_dependencies()
{
    std::list <std::string> dependencies;

    // find dynamic sections
    for (int i = 0; i < ehdr->e_shnum; i++) {
        const Elf64_Shdr * shdr = g_shdr(i);
        if (shdr->sh_type != SHT_DYNAMIC) continue;

        // find corresponding string table address
        const Elf64_Shdr * strtab = NULL;
        for (size_t di = 0; di < shdr->sh_size / shdr->sh_entsize; di++) {
            const Elf64_Dyn * dyn = (const Elf64_Dyn *)
                                    &(data[shdr->sh_offset + shdr->sh_entsize * di]);
            // found strtab addr
            if (dyn->d_tag == DT_STRTAB) {
                // find strtab shdr
                for (int si = 0; si < ehdr->e_shnum; si++) {
                    const Elf64_Shdr * tmp = g_shdr(si);
                    if (tmp->sh_addr == dyn->d_un.d_ptr) {
                        strtab = tmp;
                        break;
                    }
                }
            }
        }

        if (strtab == NULL) throw std::runtime_error("unable to locate dynamic strtab");
        // find dynamic NEEDED entries
        for (size_t di = 0; di < shdr->sh_size / shdr->sh_entsize; di++) {
            const Elf64_Dyn * dyn = (const Elf64_Dyn *)
                                    &(data[shdr->sh_offset + shdr->sh_entsize * di]);
            if (dyn->d_tag == DT_NEEDED) {
                dependencies.push_back((const char *) &(data[strtab->sh_offset + dyn->d_un.d_val]));
            }
        }
    }

    return dependencies;
}


std::list <Elf64Symbol> Elf64 :: g_symbols ()
{
    std::list <Elf64Symbol> symbols;

    // find sections with symbols
    for (int seci = 0; seci < ehdr->e_shnum; seci++) {
        const Elf64_Shdr * shdr = g_shdr(seci);
        if (    (shdr->sh_type != SHT_SYMTAB)
             && (shdr->sh_type != SHT_DYNSYM)) continue;

        // symbol section
        for (size_t symi = 0; symi < shdr->sh_size / shdr->sh_entsize; symi++) {
            symbols.push_back(g_symbol(seci, symi));
        }
    }

    return symbols;
}


std::list <Elf64Relocation> Elf64 :: g_relocations ()
{
    std::list <Elf64Relocation> relocations;

    // find relocation tables
    for (int seci = 0; seci < ehdr->e_shnum; seci++) {
        const Elf64_Shdr * shdr = g_shdr(seci);
        if (shdr->sh_type == SHT_RELA) {
            for (size_t reli = 0; reli < shdr->sh_size / shdr->sh_entsize; reli++) {
                size_t rela_offset = shdr->sh_offset + shdr->sh_entsize * reli;
                const Elf64_Rela * rela  = (const Elf64_Rela *) &(data[rela_offset]);
                const Elf64Symbol symbol = g_symbol(shdr->sh_link, ELF64_R_SYM(rela->r_info));
                Elf64Relocation r(symbol,
                                  rela->r_offset,
                                  rela->r_addend,
                                  ELF64_R_TYPE(rela->r_info));
                relocations.push_back(r);
            }
        }
    }

    return relocations;
}


std::list <Elf64Symbol> Elf64 :: find_symbols (const std::string name)
{
    std::list <Elf64Symbol> symbols = g_symbols();
    std::list <Elf64Symbol> result;
    std::list <Elf64Symbol> :: iterator it;

    for (it = symbols.begin(); it != symbols.end(); it++) {
        if ((*it).g_name() == name) {
            result.push_back(*it);
        }
    }

    return result;
}


std::list <Elf64Symbol> Elf64 :: find_symbols_deps (const std::string name)
{
    std::list <Elf64Symbol> symbols;
    std::list <Elf64 *> :: iterator dit;

    for (dit = dependencies.begin(); dit != dependencies.end(); dit++) {
        std::list <Elf64Symbol> dep_symbols = (*dit)->find_symbols(name);
        std::list <Elf64Symbol> :: iterator it;
        for (it = dep_symbols.begin(); it != dep_symbols.end(); it++) {
            if ((*it).g_name() == name)
                symbols.push_back(*it);
        }
    }

    return symbols;
}


const Elf64Symbol Elf64 :: find_symbol_glob (const std::string name, Elf64 & elf)
{
    std::list <Elf64Symbol> symbols = find_symbols(name);
    std::list <Elf64Symbol> :: iterator it;

    Elf64Symbol best_symbol;

    for (it = symbols.begin(); it != symbols.end(); it++) {
        if (it->g_binding() == STB_LOCAL)
            return *it;
        best_symbol = *it;
    }

    symbols = elf.find_symbols_deps(name);
    for (it = symbols.begin(); it != symbols.end(); it++) {
        if ((it->g_binding() == STB_GLOBAL) && (it->g_shndx() != SHN_UNDEF))
            return *it;
        best_symbol = *it;
    }

    if (best_symbol.g_name() == name) return best_symbol;

    throw std::runtime_error("could not find symbol in find_symbol_glob: " + name);
    return Elf64Symbol();
}


void Elf64 :: load_dependencies ()
{
    // load dependency files
    uint64_t offset = 0x7f000000;
    offset <<= 32;
    uint64_t offset_add = 0x00100000;
    offset_add <<= 32;
    std::list <std::string> new_deps = g_dependencies();
    std::list <std::string> loaded;

    while (new_deps.size() > 0) {
        std::list <std::string> :: iterator it;
        for (it = new_deps.begin(); it != new_deps.end();) {

            // if this dep is already loaded
            // this code needs to be rewritten
            std::list <std::string> :: iterator lit;
            bool found = false;
            for (lit = loaded.begin(); lit != loaded.end(); lit++) {
                if (*lit == *it) {
                    it = new_deps.erase(it);
                    found = true;
                    break;
                }
            }
            if (found) continue;

            loaded.push_back(*it);

            offset += offset_add;

            #ifdef DEBUG
                std::cerr << "loading dependency: " << *it 
                          << " with offset " << std::hex << offset << std::endl;
            #endif
            // load this dependency
            dependencies.push_back(new Elf64(*it, offset));

            // add this dependency's dependency to list of dependencies to load
            std::list <std::string> :: iterator dit;
            std::list <std::string> depdeps = dependencies.back()->g_dependencies();
            for (dit = depdeps.begin(); dit != depdeps.end(); dit++) {
                new_deps.push_back(*dit);
            }
        }
    }
}


#define R_X86_64_64        1
#define R_X86_64_GLOB_DAT  6
#define R_X86_64_JUMP_SLOT 7
#define R_X86_64_RELATIVE  8
#define R_X86_64_DTPMOD64  16
#define R_X86_64_TPOFF64   18
#define R_X86_64_IRELATIVE 37

void Elf64 :: patch_relocations (Memory & memory, Elf64 & elf)
{
    std::list <Elf64Relocation> relocations = g_relocations();
    std::list <Elf64Relocation> :: iterator it;

    for (it = relocations.begin(); it != relocations.end(); it++) {
        if (    (it->g_type() == R_X86_64_GLOB_DAT)
             || (it->g_type() == R_X86_64_JUMP_SLOT)) {
            const Elf64Symbol symbol = find_symbol_glob((*it).g_symbol().g_name(), elf);
            memory.s_qword(offset + it->g_offset(), symbol.g_address());   

            #ifdef DEBUG_RELO
                std::cerr << "relocation JMP_SLOT("
                          << it->g_symbol().g_name() << " "
                          << std::hex << it->g_offset() << ") -> ("
                          << symbol.g_name() << " "
                          << std::hex << symbol.g_address() << ")"
                          << std::endl;
            #endif
        }
        else if (it->g_type() == R_X86_64_RELATIVE) {
            memory.s_qword(offset + it->g_offset(), offset + it->g_addend());

            #ifdef DEBUG_RELO
                std::cerr << "relocation RELATIVE("
                          << std::hex << it->g_offset() << ") -> ("
                          << std::hex << offset + it->g_addend() << ")"
                          << std::endl;
            #endif
        }
        else if (it->g_type() == R_X86_64_64) {
            const Elf64Symbol symbol = find_symbol_glob((*it).g_symbol().g_name(), elf);
            memory.s_qword(offset + it->g_offset(),
                           symbol.g_address() + it->g_addend());

            #ifdef DEBUG_RELO
                std::cerr << "relocation 64("
                          << std::hex << it->g_offset() << ") -> ("
                          << symbol.g_name() << " "
                          << std::hex << symbol.g_address() + it->g_addend() << ")"
                          << std::endl;
            #endif
        }
        else if (it->g_type() == R_X86_64_TPOFF64) {}
        else if (it->g_type() == R_X86_64_DTPMOD64) {}
        else if (it->g_type() == R_X86_64_IRELATIVE) {}
        else {
            std::stringstream ss;
            ss << "unknown relocation type: " << (int) it->g_type();
            throw std::runtime_error(ss.str());
        }
    }
}


Elf64 :: Elf64 (const std::string filename)
    : filename(filename), offset(0), dependency(false)
{
    data = NULL;
    load();
}


Elf64 :: Elf64 (const std::string filename, uint64_t offset)
    : filename(filename), offset(offset), dependency(true)
{
    data = NULL;
    load();
}


Elf64 :: ~Elf64 ()
{
    #ifdef DEBUG
    std::cerr << "Elf64 " << filename << " destructor" << std::endl;
    #endif
    std::list <Elf64 *> :: iterator dit;

    for (dit = dependencies.begin(); dit != dependencies.end(); dit++) {
        delete *dit;
    }

    if (data) delete[] data;
}


uint64_t Elf64 :: g_entry () { return ehdr->e_entry; }

std::map <uint64_t, Page *> Elf64 :: g_pages ()
{
    std::multimap <uint64_t, Page *> pages;

    #ifdef DEBUG
    std::cerr << "loading pages for: " << filename << std::endl;
    #endif

    if (dependency) {
        for (int i = 0; i < ehdr->e_phnum; i++) {
            size_t phdr_offset = ehdr->e_phoff + (ehdr->e_phentsize * i);
            const Elf64_Phdr * phdr = (const Elf64_Phdr *) &(data[phdr_offset]);
            if (phdr->p_type == PT_NULL) continue;
            // temporarily allocate space to store the page data
            uint8_t * tmp = new uint8_t[phdr->p_memsz];
            memset(tmp, 0, phdr->p_memsz);
            memcpy(tmp, &(data[phdr->p_offset]), phdr->p_filesz);
            // add offset to vaddr
            uint64_t vaddr = this->offset + phdr->p_vaddr;
            std::pair<uint64_t, Page *> p(vaddr, new Page(phdr->p_memsz, tmp));
            pages.insert(p);
            delete[] tmp;
        }
        return fix_pages(pages);
    }
    else {
        // pages for this binary
        for (int i = 0; i < ehdr->e_phnum; i++) {
            size_t phdr_offset = ehdr->e_phoff + (ehdr->e_phentsize * i);
            const Elf64_Phdr * phdr = (const Elf64_Phdr *) &(data[phdr_offset]);
            //if (phdr->p_type == PT_NULL) continue;
            // temporarily allocate space to store the page data
            // don't allocate empty pages
            uint8_t * tmp = new uint8_t[phdr->p_memsz];
            memset(tmp, 0, phdr->p_memsz);
            memcpy(tmp, &(data[phdr->p_offset]), phdr->p_filesz);
            std::pair<uint64_t, Page *> p(phdr->p_vaddr, new Page(phdr->p_memsz, tmp));
            pages.insert(p);
            delete[] tmp;
        }

        // pages for dependencies
        std::map <uint64_t, Page *> dep_pages;
        std::list <Elf64 *> :: iterator dit;

        for (dit = dependencies.begin(); dit != dependencies.end(); dit++) {
            dep_pages = (*dit)->g_pages();
            std::multimap <uint64_t, Page *> :: iterator pit;
            for (pit = dep_pages.begin(); pit != dep_pages.end(); pit++) {
                pages.insert(std::pair<uint64_t, Page *>(pit->first, pit->second));
            }
        }

        // make a page for the stack
        uint64_t ELF64_STACK_BEGIN = 0x7fff0000;
        ELF64_STACK_BEGIN <<= 32;

        pages.insert(std::pair<uint64_t, Page *>(ELF64_STACK_BEGIN, new Page(0x10000)));

        #ifdef DEBUG
        std::cerr << "fixing pages" << std::endl;
        #endif
        return fix_pages(pages);
    }
}


Memory Elf64 :: g_memory ()
{
    Memory memory(g_pages());

    #ifdef DEBUG
    std::cerr << "patching relocations for: " << filename << std::endl;
    #endif
    patch_relocations(memory, *this);

    std::list <Elf64 *> :: iterator dit;

    for (dit = dependencies.begin(); dit != dependencies.end(); dit++) {
        #ifdef DEBUG
        std::cerr << "patching relocations for: " << (*dit)->g_filename() << std::endl;
        #endif
        (*dit)->patch_relocations(memory, *this);
    }

    return memory;
}


std::map <uint64_t, SymbolicValue> Elf64 :: g_variables ()
{
    std::map <uint64_t, SymbolicValue> variables;

    variables[InstructionOperand::str_to_id("UD_R_RAX")] = SymbolicValue(64, 0);
    variables[InstructionOperand::str_to_id("UD_R_RBX")] = SymbolicValue(64, 0);
    variables[InstructionOperand::str_to_id("UD_R_RCX")] = SymbolicValue(64, 0);
    variables[InstructionOperand::str_to_id("UD_R_RDX")] = SymbolicValue(64, 0);
    variables[InstructionOperand::str_to_id("UD_R_RSI")] = SymbolicValue(64, 0);
    variables[InstructionOperand::str_to_id("UD_R_RDI")] = SymbolicValue(64, 0);
    variables[InstructionOperand::str_to_id("UD_R_R8")]  = SymbolicValue(64, 0);
    variables[InstructionOperand::str_to_id("UD_R_R9")]  = SymbolicValue(64, 0);
    variables[InstructionOperand::str_to_id("UD_R_R10")] = SymbolicValue(64, 0);
    variables[InstructionOperand::str_to_id("UD_R_R11")] = SymbolicValue(64, 0);
    variables[InstructionOperand::str_to_id("UD_R_R12")] = SymbolicValue(64, 0);
    variables[InstructionOperand::str_to_id("UD_R_R13")] = SymbolicValue(64, 0);
    variables[InstructionOperand::str_to_id("UD_R_R14")] = SymbolicValue(64, 0);
    variables[InstructionOperand::str_to_id("UD_R_R15")] = SymbolicValue(64, 0);

    uint64_t RSP = 0x7fff0000;
    RSP <<= 32;
    RSP += 0x10000 - 0x100;
    variables[InstructionOperand::str_to_id("UD_R_RSP")] = SymbolicValue(64, RSP);
    variables[InstructionOperand::str_to_id("UD_R_RBP")] = SymbolicValue(64, RSP + 20);
    variables[InstructionOperand::str_to_id("UD_R_RIP")] = SymbolicValue(64, g_entry());
    return variables;
}

uint64_t Elf64 :: g_ip_id () { return InstructionOperand::str_to_id("UD_R_RIP"); }