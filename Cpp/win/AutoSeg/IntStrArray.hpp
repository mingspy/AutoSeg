/*
 * Copyright (C) 2014  mingspy@163.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#pragma once
#include <iostream>
#include <string>

namespace mingspy
{
class IntStrArray
{
private:
    const int * _array;
public:
    IntStrArray(const int * suffix)
    {
        _array = suffix;
    }

    // change wchar_t to int array in order to save as a unique file
    // between linux and windows
    // notice: the int buf[len] == 0
    static void wstrToArray(const wchar_t * suffix,  int * buf)
    {
        int len = 0;
        while(*suffix) {
            buf[len++] = *suffix++;
        }

        buf[len] = 0;
    }
    static void arrayToWstr(const int * suffix,  wchar_t * buf )
    {
        int len = 0;
        while(*suffix) {
            buf[len++] = (wchar_t)(*suffix++);
        }
        buf[len] = 0;
    }
    static int arraySize(const int * suffix)
    {
        int len = 0;
        while(*suffix++) {
            len++;
        }
        return len+1;
    }

    // the length of suffix int array
    // as the last one be zero, so the
    // suffixSize = length(suffix) + 1;
    inline int size() const
    {
        return arraySize(_array);
    }

    const int * getArray() const
    {
        return _array;
    }

};

}
