#include "page.h"

#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>

#define DEBUG

Page :: Page (size_t size)
{
    this->size = size;
    this->data = new uint8_t [size];
    this->parent = NULL;
}

Page :: Page (size_t size, uint8_t * data)
{
    this->size = size;
    this->data = new uint8_t [size] ;
    memcpy(this->data, data, size);
    this->parent = NULL;
}

Page * Page :: destroy ()
{
    #ifdef DEBUG
    std::cerr << "Page::destroy()" << std::endl;
    #endif

    delete[] this->data;
    Page * result = this->parent;
    delete this;
    return result;
}

Page * Page :: make_child ()
{
    Page * child = new Page(size, data);
    child->set_parent(this);
    return child;
}

void Page :: check_offset (size_t offset, size_t bytes)
{
	if (offset + bytes > this->size) {
		std::stringstream ss;
		ss << "invalid offset " << offset << " (size is " << size << ")";
		throw std::out_of_range(ss.str());
	}
}
		

void Page :: set_parent (Page * parent)
{
    this->parent = parent;
}

void Page :: resize (size_t new_size)
{
    uint8_t * new_data = new uint8_t[new_size];
    memset(new_data, 0, new_size);
    size_t copy_size = new_size < size ? new_size : size;
    memcpy(new_data, data, copy_size);
    delete[] data;
    data = new_data;
    size = new_size;
}

size_t Page :: g_size () { return this->size; }

uint8_t * Page :: g_data (size_t offset)
{
    check_offset(offset, 0);
    return &(this->data[offset]);
}

uint8_t Page :: g_byte (size_t offset)
{
    check_offset(offset, 1);
    return this->data[offset];
}

uint16_t Page :: g_word (size_t offset)
{
    check_offset(offset, 2);
    return *((uint16_t *) &(this->data[offset]));
}

uint32_t Page :: g_dword (size_t offset)
{
    check_offset(offset, 4);
    return *((uint32_t *) &(this->data[offset]));
}

uint64_t Page :: g_qword (size_t offset)
{
    check_offset(offset, 8);
    return *((uint64_t *) &(this->data[offset]));
}

void Page :: s_byte (size_t offset, uint8_t value)
{
    check_offset(offset, 1);
    this->data[offset] = value;
}

void Page :: s_word (size_t offset, uint16_t value)
{
    check_offset(offset, 2);
    *((uint16_t *) &(this->data[offset])) = value;
}

void Page :: s_dword (size_t offset, uint32_t value)
{
    check_offset(offset, 4);
    *((uint32_t *) &(this->data[offset])) = value;
}

void Page :: s_qword (size_t offset, uint64_t value)
{
    check_offset(offset, 8);
    *((uint64_t *) &(this->data[offset])) = value;
}
