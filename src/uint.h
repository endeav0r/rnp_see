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

        bool cmpLts (const UInt &) const;
        bool cmpLes (const UInt &) const;
};

#endif