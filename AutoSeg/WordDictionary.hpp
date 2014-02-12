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
#include "TrieDef.hpp"
#include "DATrie.hpp"
#include "SparseInstance.hpp"
#include "MemoryPool.hpp"
#include "MemLeaksCheck.h"

#include "HashMapDef.hpp"

namespace mingspy
{

//const wstring const NATURE_UNDEF = L"UNDEF";

class WordDictionary
{
private:
    DATrie datrie;
    vector<wstring> natures;
    hash_map<wstring, int> nature_index;
    MemoryPool<> mem_pool;
private:
    static const int DICT_SIGNATURE = 0x112fd0ac;
public:
    WordDictionary()
    {
        datrie.setDataFreer(InstanceFreer);
        datrie.setDataReader(ReadInstanceDataFromFile);
        datrie.setDataWriter(WriteInstanceDataToFile);
    }

    // read binary data from a file.
    WordDictionary(const string & file)
    {
        datrie.setMemPool(&mem_pool);
        datrie.setDataReader(ReadInstanceDataFromFile);
        datrie.setDataWriter(WriteInstanceDataToFile);
        FILE * pfile = fopen(file.c_str(),"rb");
        assert(pfile != NULL);
        if(!pfile)
        {
            return;
        }

        readFromFile(pfile);
        fclose(pfile);
    }

    bool addNature(const wstring& nature)
    {
        if(nature_index.find(nature) != nature_index.end())
        {
            return false;
        }
        natures.push_back(nature);
        nature_index[nature] = natures.size() - 1;
        return true;
    }

    int getNatureIndex(const wstring &nature)
    {
        if(nature_index.find(nature) != nature_index.end())
        {
            return nature_index[nature];
        }
        return -1;
    }

    const wstring & getNature(int index)
    {
        if(index < natures.size())
        {
            return natures[index];
        }
        //return NATURE_UNDEF;
        return L"UNDEF";
    }

    bool addWordInfo(const wstring & word, SparseInstance * info)
    {
        return datrie.add(word.c_str(), info);
    }

    const SparseInstance * getWordInfo(const wstring & word) const
    {
        return (const SparseInstance *)datrie.retrieve(word.c_str());
    }

    bool existPrefix(const wstring & prefix) const
    {
        return datrie.containsPrefix(prefix.c_str());
    }

    bool writeToFile(const string & file)
    {
        FILE * pfile = fopen(file.c_str(),"wb");
        assert(pfile != NULL);
        bool result = false;
        int nature_size = natures.size();
        if(!file_write_int32(pfile, DICT_SIGNATURE)
                || !file_write_int32(pfile, nature_size))
        {
            goto end_write;
        }

        for(int i = 0; i < nature_size; i++)
        {
            WriteTrieStrToFile(pfile, natures[i].c_str());
        }

        if(!datrie.writeToFile(pfile))
        {
            goto end_write;
        }

        result = true;
end_write:
        fclose(pfile);
        return result;
    }

private:

    void readFromFile(FILE * pfile)
    {
        // read header
        int signature;
        if(!file_read_int32(pfile, &signature) || signature != DICT_SIGNATURE)
        {
            assert(false);
            return;
        }

        // read Natures.
        int nature_size;
        if(!file_read_int32(pfile, &nature_size)
                || nature_size > 0x0fffffff
                || nature_size < 0)
        {
            assert(false);
            return;
        }

        for(int i = 0; i < nature_size; i++)
        {
            wstring nature = (wchar_t *)ReadTrieStrFromFile(pfile, &mem_pool);
            addNature(nature);
        }
        // read dat
        datrie.readFromFile(pfile);
    }

};


}
