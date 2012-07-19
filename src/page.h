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

#ifndef page_HEADER
#define page_HEADER

#include <inttypes.h>
#include <cstddef>

class Page {
    private :
        uint8_t * data;
        size_t size;
        Page * parent;
        int references;
        
        void check_offset (size_t offset, size_t bytes);
        
    public :
        Page (size_t size);
        Page (size_t size, uint8_t * data);
        
        Page * destroy    ();
        Page * make_child ();
        
        void set_parent (Page * parent);
        void reference  ();
        
        void resize (size_t new_size);

        // copies data into this page
        // this does not affect the size of the page, and will throw an error
        // if size > this->size
        void s_data (const uint8_t * data, size_t size);
        void s_data (size_t offset, const uint8_t * data, size_t size);

        size_t    g_size  ();
        uint8_t * g_data  (size_t offset);
        
        uint8_t   g_byte  (size_t offset);
        uint16_t  g_word  (size_t offset);
        uint32_t  g_dword (size_t offset);
        uint64_t  g_qword (size_t offset);
        
        void s_byte  (size_t offset, uint8_t  value);
        void s_word  (size_t offset, uint16_t value);
        void s_dword (size_t offset, uint32_t value);
        void s_qword (size_t offset, uint64_t value);
};

#endif
