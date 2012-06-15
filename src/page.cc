#include "page.h"

#include <stdexcept>

Page :: Page (size_t size)
{
    this->size = size;
    this->data = new uint8_t [size] ;
    this->parent = NULL;
    this->references = 1;
}

void Page :: destroy ()
{
    this->parent->destroy();
    this->references--;
    if (this->references == 0) {
        delete this->data;
        delete this;
    }
}

void Page :: set_parent (Page * parent)
{
    this->parent = parent;
}

uint8_t Page :: g_byte (size_t offset)
{
    if (offset + 1 > this->size) throw std::out_of_range("");
    return this->data[offset];
}

uint16_t Page :: g_word (size_t offset)
{
    if (offset + 2 > this->size) throw std::out_of_range("");
    return *((uint16_t *) &(this->data[offset]));
}

uint32_t Page :: g_dword (size_t offset)
{
    if (offset + 4 > this->size) throw std::out_of_range("");
    return *((uint32_t *) &(this->data[offset]));
}

uint64_t Page :: g_qword (size_t offset)
{
    if (offset + 8 > this->size) throw std::out_of_range("");
    return *((uint64_t *) &(this->data[offset]));
}

void Page :: s_byte (size_t offset, uint8_t value)
{
    if (offset + 1 > this->size) throw std::out_of_range("");
    this->data[offset] = value;
}

void Page :: s_word (size_t offset, uint16_t value)
{
    if (offset + 2 > this->size) throw std::out_of_range("");
    *((uint16_t *) &(this->data[offset])) = value;
}

void Page :: s_dword (size_t offset, uint32_t value)
{
    if (offset + 4 > this->size) throw std::out_of_range("");
    *((uint32_t *) &(this->data[offset])) = value;
}

void Page :: s_qword (size_t offset, uint64_t value)
{
    if (offset + 8 > this->size) throw std::out_of_range("");
    *((uint64_t *) &(this->data[offset])) = value;
}
