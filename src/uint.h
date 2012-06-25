#ifndef uint_HEADER
#define uint_HEADER

#include <inttypes.h>
#include <iostream>
#include <string>

class UInt {
    private :
        int bits;
        __uint128_t value;
    public :
        UInt ();
        UInt (int bits);
        UInt (int bits, __uint128_t value);

        UInt sign_extend (int bits) const;

        std::string str      () const;
        __uint128_t g_value  () const;
        __int128_t  g_svalue () const;
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
};

#endif