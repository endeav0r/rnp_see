#ifndef uint_HEADER
#define uint_HEADER

#include <inttypes.h>
#include <iostream>
#include <string>

class UInt {
    private :
        int bits;
        __uint128_t value;
        __uint128_t g_value128  () const;
        __int128_t  g_svalue128 () const;
    public :
        UInt ();
        UInt (int bits);
        UInt (int bits, uint64_t value);

        UInt sign_extend (int bits) const;
        UInt extend      (int bits) const;

        uint64_t g_value64   () const { return g_value128(); }

        std::string str      () const;
        int         g_bits   () const { return bits; }

        UInt operator +  (const UInt &) const;
        UInt operator -  (const UInt &) const;
        UInt operator *  (const UInt &) const;
        UInt operator /  (const UInt &) const;
        UInt operator %  (const UInt &) const;
        UInt operator &  (const UInt &) const;
        UInt operator ^  (const UInt &) const;
        UInt operator |  (const UInt &) const;
        UInt operator << (const UInt &) const;
        UInt operator >> (const UInt &) const;

        UInt operator ~  () const;

        bool operator <  (const UInt &) const;
        bool operator >  (const UInt &) const;
        bool operator <= (const UInt &) const;
        bool operator >= (const UInt &) const;
        bool operator == (const UInt &) const;
        bool operator != (const UInt &) const;
};

#endif