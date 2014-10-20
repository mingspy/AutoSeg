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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <cassert>
#include "IntStrArray.hpp"
#include <iostream>
#include <string>
using namespace std;

namespace mingspy
{

class Serializer
{
public :
    Serializer(FILE * pfile)
    {
        assert(pfile);
        _pfile = pfile;
    }


    inline char readChar ()
    {
        char ch;
        if(fread (&ch, sizeof (char), 1, _pfile) == 1) {
            return ch;
        }
        assert(false);
        return 0x7f;
    }

    inline bool writeChar (char val)
    {
        return (fwrite (&val, sizeof (char), 1, _pfile) == 1);
    }

    inline bool readChars (FILE *file, char *buff, int len)
    {
        return (fread (buff, sizeof (char), len, file) == len);
    }

    inline bool writeChars (FILE *file, const char *buff, int len)
    {
        return (fwrite (buff, sizeof (char), len, file) == len);
    }

    inline short readInt16 ()
    {
        unsigned char   buff[2];
        if (fread (buff, 2, 1, _pfile) == 1) {
            short val = (buff[0] << 8) | buff[1];
            return val;
        }
        assert(false);
        return 0x7fff;
    }

    inline bool writeInt16 (short val)
    {
        unsigned char   buff[2];
        buff[0] = (val >> 8)&0xff;
        buff[1] = val & 0xff;
        return (fwrite (buff, 2, 1, _pfile) == 1);
    }

    inline int readInt32()
    {
        unsigned char   buff[4];
        if (fread (buff, 4, 1, _pfile) == 1) {
            int val = (buff[0] << 24) | (buff[1] << 16)
                      |  (buff[2] << 8) | buff[3];
            return val;
        }
        assert(false);
        return 0x7fffffff;
    }

    inline bool writeInt32(int val)
    {
        unsigned char   buff[4];
        buff[0] = (val >> 24) & 0xff;
        buff[1] = (val >> 16) & 0xff;
        buff[2] = (val >> 8) & 0xff;
        buff[3] = val & 0xff;
        return (fwrite (buff, 4, 1, _pfile) == 1);
    }

    inline bool readIntArray(int * arr, int len)
    {
        for(int i = 0; i < len; i++ ) {
            arr[i] = readInt32();
        }
        return true;
    }

    inline bool writeIntArray(const int * arr, int len)
    {
        for(int i = 0; i < len; i++ ) {
            writeInt32(arr[i]);
        }
        return true;
    }

    inline wchar_t * readWstr(wchar_t * buf = NULL)
    {
        short len = readInt16();
        return readWstrData(len,buf);
    }

    inline wchar_t * readWstrData(int len, wchar_t * buf = NULL)
    {
        bool selfAllocated = false;
        if(buf == NULL) {
            buf = new wchar_t[len + 1];
            selfAllocated = true;
        }
        buf[len] = 0;

        if(len > 0) {
            int *tmp = new int[len+1];
            if(readIntArray(tmp, len)) {
                tmp[len] = 0;
                IntStrArray::arrayToWstr(tmp, buf);
            } else {
                assert(false);
                if(selfAllocated) {
                    delete [] buf;
                    buf = NULL;
                }
            }
            delete [] tmp;
        }
        return buf;
    }

    inline bool writeWstr(const wstring & wstr)
    {
        int len = wstr.length();
        assert(len < 0x7fff);
        if(!writeInt16(len)) {
            assert(false);
        }
        return writeWstrData(wstr, len);
    }

    inline bool writeWstrData(const wstring & wstr, int len)
    {

        bool res = false;
        if(len > 0) {
            int * buf = new int[len+1];
            IntStrArray::wstrToArray(wstr.c_str(),buf);
            res = writeIntArray(buf, len);
            delete [] buf;
        }
        return res;
    }

    inline bool write(const void * buf, int size, int count)
    {
        return fwrite(buf,size, count, _pfile) == count;
    }

    inline bool read(void * buf, int size, int count)
    {
        return fread(buf, size, count, _pfile) == count;
    }
private:


private:
    FILE * _pfile;
};
}