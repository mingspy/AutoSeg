/*
 * DATcpp - Double-Array Trie Library
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
#include <cassert>
#include "FileUtils.hpp"
#include "MemoryPool.hpp"
#include "MemLeaksCheck.h"

using namespace std;

namespace mingspy
{
#define min(a,b) ((a) < (b) ? (a):(b))

typedef wchar_t TrieChar;

const int TRIE_INDEX_ERROR = 0;
const void * TRIE_DATA_ERROR = NULL;
const int TRIE_CHILD_MAX = 65536; // how many children a char can have in dat.

/**
* The length of a TrieStr
*/
int TrieStrLen(const TrieChar * str)
{
    if(str != NULL)
    {
        const TrieChar * p = str;
        int len = 0;
        while(*p++ != TrieChar(0))
        {
            len ++;
        }
        return len;
    }
    return 0;
}

/**
* Tail data delete call back functions
*/
typedef void (*TailDataFree)(void * );

void TrieCharFreeFunc(void * ptr)
{
    TrieChar * p = (TrieChar *) ptr;
    delete [] p;
}


/**
* Write data to a given file, which used for tail to serialize.
*/
typedef void (*WriteTailDataToFile)(FILE * file, const void * data);

/*
* Reads data from given file, which used for tail unserialize.
* if the pmem != null, must allocate att the needed datas on pmem.
* And the allocate data will be release by pmem at once.
* MemoryPool only safe when used to save simple objects that not 
* holds other objects which need to free.
*/
typedef void *(*ReadTailDataFromFile)(FILE * file, MemoryPool<> * pmem);

void WriteTrieStrToFile(FILE * file, const void * str)
{
    if(str != NULL)
    {

        int len = TrieStrLen((TrieChar *)str) + 1;
        assert(len < 0xffff);
        if(!file_write_int16(file, len))
        {
            assert(false);
        }
        if(fwrite(str, sizeof(TrieChar), len, file) != len)
        {
            assert(false);
        }
    }
    else
    {
        assert(false);
    }
}

void * ReadTrieStrFromFile(FILE * file, MemoryPool<> * pmem)
{
    unsigned short len;
    if(!file_read_int16(file, (short *)&len))
    {
        assert(false);
        return NULL;
    }
    TrieChar * str = NULL;
    if(pmem){
        str = (TrieChar *)pmem->allocAligned(len * sizeof(TrieChar));
    }else{
        str = new TrieChar[len];
    }

    if(fread(str, sizeof(TrieChar), len, file) != len)
    {
        assert(false);
        if(!pmem){
            delete [] str;
        }
        return NULL;
    }

    return str;
}
}

