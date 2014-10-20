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
#include <cassert>
#include "FileUtils.hpp"
#include "MemoryPool.hpp"
#include "MemLeaksCheck.h"
#include "IntStrArray.hpp"
#include "Serializer.hpp"

using namespace std;

namespace mingspy
{

//typedef wchar_t TrieChar;

const int TRIE_INDEX_ERROR = 0;
const void * TRIE_DATA_ERROR = NULL;
const int TRIE_CHILD_MAX = 65536; // how many children a char can have in dat.

const wchar_t wordSeperator = L'\t';
const wstring natureSeperator = L",";
const wchar_t freqSeperator = L':';

/**
* Tail data delete call back functions
*/
typedef void (*TailDataFreer)(void * );

inline void TrieStrFreer(void * ptr)
{
    wchar_t * p = (wchar_t *) ptr;
    delete [] p;
}


/**
* Write data to a given file, which used for tail to serialize.
*/
typedef void (*TailDataWriter)(FILE * file, const void * data);

/*
* Reads data from given file, which used for tail unserialize.
* if the pmem != null, must allocate att the needed datas on pmem.
* And the allocate data will be release by pmem at once.
* MemoryPool only safe when used to save simple objects that not
* holds other objects which need to free.
*/
typedef void *(*TailDataReader)(FILE * file);

void TrieStrWriter(FILE * file, const void * str)
{
    if(str != NULL) {
        Serializer serializer(file);
        serializer.writeWstr(static_cast<const wchar_t *>(str));
    } else {
        assert(false);
    }
}

/*
* read wstring from file, and return
* a wchar_t * allocated in heap, caller
* need delete the result when not use.
*/
void * TrieStrReader(FILE * file)
{
    Serializer serializer(file);
    return serializer.readWstr();
}
}

