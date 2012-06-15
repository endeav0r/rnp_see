#include "staticvector.h"

#include <stdexcept>

template <class Type>
StaticVector <Type> :: StaticVector ()
{
    _size = 0;
}

template <class Type>
StaticVector <Type> :: ~StaticVector ()
{
    size_t i;
    
    _size = 0;

    for (i = 0; i < chunks.size(); i++)
        delete chunks[i];
}

template <class Type>
Type & StaticVector <Type> :: push_back (Type & t)
{
    size_t index_div = _size / CHUNK_SIZE;
    size_t index_mod = _size % CHUNK_SIZE;
    
    if (index_mod) {
        this->chunks[index_div] = new unsigned char [sizeof(Type) * CHUNK_SIZE];
    }

    Type & tt = get(_size);
    
    tt = t;
    
    _size++;
    
    return tt;
}

template <class Type>
size_t StaticVector <Type> :: size () { return size; }

template <class Type>
Type & StaticVector <Type> :: get (size_t index)
{
    if (index >= _size)
        throw std::out_of_range();
    
    size_t index_div = index / CHUNK_SIZE;
    size_t index_mod = index % CHUNK_SIZE;

    Type * tt = this->chunks[index_div];
    
    tt *= index_mod;
    
    return *tt;
}

template <class Type>
Type & StaticVector <Type> :: back ()
{
    if (_size == 0)
        throw std::out_of_range();
    
    return get(_size - 1);
}
