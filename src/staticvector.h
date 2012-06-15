/*
 * This data structure works like a dynamically sized vector that
 * grows in size by chunks where memory addresses of individual
 * members will not change. It is designed for constant-time lookups,
 * mostly constant-time insertions, and allows references to elements to be
 * safely returned.
 * 
 * It does not decrease in size
 */

#ifndef staticvector_HEADER
#define staticvector_HEADER

#define CHUNK_SIZE 1024

#include <cstddef>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <vector>

template <class Type>
class StaticVector {
    private :
        size_t _size;
        std::vector <unsigned char *> chunks;
    
    public :
        StaticVector  ();
        ~StaticVector ();
        
        Type & push_back (Type & t);
        size_t size      ();
        Type & get       (size_t index);
        Type & back      ();
};



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
    if (_size % CHUNK_SIZE == 0) {
        unsigned char * newmem = new unsigned char [sizeof(Type) * CHUNK_SIZE];
        this->chunks.push_back(newmem);
    }
    
    _size++;

    Type * tt = new((void *) &(get(_size - 1))) Type(t);
    
    return *tt;
}


template <class Type>
size_t StaticVector <Type> :: size () { return _size; }


template <class Type>
Type & StaticVector <Type> :: get (size_t index)
{
    if (index >= _size) {
        std::stringstream ss;
        ss << "get out of bounds [" << _size << "," << index << "]";
        throw std::out_of_range(ss.str());
    }
    
    size_t index_div = index / CHUNK_SIZE;
    size_t index_mod = index % CHUNK_SIZE;

    Type * tt = (Type *) this->chunks[index_div];
    
    return tt[index_mod];
}


template <class Type>
Type & StaticVector <Type> :: back ()
{
    if (_size == 0)
        throw std::out_of_range("back called on StaticVector(0)");
    
    return get(_size - 1);
}


#endif
