#include "elf.h"

#include <cstring>
#include <iostream>
#include <stdexcept>

#include "instruction.h"

/*
 *  Performs a lot of checking/cleaning of pages.
 *  Linkers often times overlap pages in the elf binary. This makes our job
 *  much more difficult. To deal with this, we make sure pages don't overlap.
 *  Pages must, by spec, be given in increasing order of virtual address, which
 *  makes our job a lot easier.
 */
std::map <uint64_t, Page *> Elf :: fix_pages (std::map <uint64_t, Page *> pages)
{
	// no overlapping pages
	std::map <uint64_t, Page *> :: iterator it = pages.begin();
	std::map <uint64_t, Page *> :: iterator next = it; next++;
	while (next != pages.end()) {
		// this page overflows in to the next page
		if (it->first + it->second->g_size() > next->first) {
			// if the next page is in the middle of this page, we split this page around
			// the next page
			if (it->first + it->second->g_size() > next->first + next->second->g_size()) {
				uint64_t nnext = next->first + next->second->g_size();
				while (pages.count(nnext) > 0) {
					nnext += pages[nnext]->g_size();
				}
				// make sure we still overlap
				if (it->first + it->second->g_size() > nnext) {
					size_t new_offset = nnext - it->first;
					size_t new_size   = it->second->g_size() - new_offset;
					pages[nnext] = new Page(new_size, it->second->g_data(new_offset));
				}
			}
			// now resize the page so it doesn't overlap
			it->second->resize(next->first - it->first);
		}
		it++;
		next++;
	}

	// no empty pages
	for (it = pages.begin(); it != pages.end(); it++) {
		if (it->second->g_size() == 0) it = pages.erase(it);
	}

	return pages;
}

Elf :: Elf (const uint8_t * data, size_t data_size)
{
	this->data = new uint8_t[data_size];
	memcpy((void *) this->data, (void *) data, data_size);
	data_size = data_size;
}

Elf :: ~Elf()
{
	delete data;
}

Elf * Elf :: Get (const uint8_t * data, size_t data_size)
{
	if (    (data[EI_MAG0] == ELFMAG0)
		 && (data[EI_MAG1] == ELFMAG1)
		 && (data[EI_MAG2] == ELFMAG2)
		 && (data[EI_MAG3] == ELFMAG3)) {
	    switch (data[EI_CLASS]) {
	    case ELFCLASS32 : return new Elf32(data, data_size);
	    case ELFCLASS64 : return new Elf64(data, data_size);
	    }
	    throw std::runtime_error("invalid elf, EI_CLASS no match");
    }
    else throw std::runtime_error("invalid elf, EI_MAG no match");

    return NULL;
}


/**************
 * Elf32      *
 *************/


Elf32 :: Elf32 (const uint8_t * data, size_t data_size) : Elf(data, data_size)
{
	ehdr = (const Elf32_Ehdr *) this->data;
}

uint64_t Elf32 :: g_entry () { return ehdr->e_entry; }

std::map <uint64_t, Page *> Elf32 :: g_pages ()
{
	std::map <uint64_t, Page *> pages;

	for (int i = 0; i < ehdr->e_phnum; i++) {
		const Elf32_Phdr * phdr = (const Elf32_Phdr *) &(data[ehdr->e_phoff + (ehdr->e_phentsize * i)]);
		if (phdr->p_type == PT_NULL) continue;
		// temporarily allocate space to store the page data
		uint8_t * tmp = new uint8_t[phdr->p_memsz];
		memset(tmp, 0, phdr->p_memsz);
		memcpy(tmp, &(data[phdr->p_offset]), phdr->p_filesz);
		pages[phdr->p_vaddr] = new Page(phdr->p_memsz, tmp);
		delete tmp;
	}

	// make a page for the stack
	pages[ELF32_STACK_BEGIN] = new Page(ELF32_STACK_SIZE);

	return fix_pages(pages);
}

std::map <uint64_t, SymbolicValue> Elf32 :: g_variables ()
{
	std::map <uint64_t, SymbolicValue> variables;

	return variables;
}

uint64_t Elf32 :: g_ip_id () { return 0; }


/**************
 * Elf64      *
 *************/

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


Elf64 :: Elf64 (const uint8_t * data, size_t data_size) : Elf(data, data_size)
{
	ehdr = (const Elf64_Ehdr *) this->data;

	std::list <std::string> dependencies = g_dependencies();
	std::list <std::string> :: iterator it;
	for (it = dependencies.begin(); it != dependencies.end(); it++) {
		std::cerr << *it << std::endl;
	}
}

uint64_t Elf64 :: g_entry () { return ehdr->e_entry; }

std::map <uint64_t, Page *> Elf64 :: g_pages ()
{
	std::map <uint64_t, Page *> pages;

	for (int i = 0; i < ehdr->e_phnum; i++) {
		const Elf64_Phdr * phdr = (const Elf64_Phdr *) &(data[ehdr->e_phoff + (ehdr->e_phentsize * i)]);
		if (phdr->p_type == PT_NULL) continue;
		// temporarily allocate space to store the page data
		uint8_t * tmp = new uint8_t[phdr->p_memsz];
		memset(tmp, 0, phdr->p_memsz);
		memcpy(tmp, &(data[phdr->p_offset]), phdr->p_filesz);
		pages[phdr->p_vaddr] = new Page(phdr->p_memsz, tmp);
		delete tmp;
	}

	// make a page for the stack
	pages[ELF64_STACK_BEGIN] = new Page(ELF64_STACK_SIZE);

	return fix_pages(pages);
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

	uint64_t RSP = ELF64_STACK_BEGIN + ELF64_STACK_SIZE - 0x100;
	variables[InstructionOperand::str_to_id("UD_R_RSP")] = SymbolicValue(64, RSP);
	variables[InstructionOperand::str_to_id("UD_R_RBP")] = SymbolicValue(64, RSP + 20);
	variables[InstructionOperand::str_to_id("UD_R_RIP")] = SymbolicValue(64, g_entry());
	return variables;
}

uint64_t Elf64 :: g_ip_id () { return InstructionOperand::str_to_id("UD_R_RIP"); }