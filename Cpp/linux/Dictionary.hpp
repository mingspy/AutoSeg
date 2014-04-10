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
#include <string>

namespace mingspy
{

//const wstring const NATURE_UNDEF = L"UNDEF";
const wstring NATURE_FREQTOTAL=L"FREQTOL";
class Dictionary
{
private:
    static const int DICT_SIGNATURE = 0x112fd0ac;
public:
    Dictionary()
    {
        datrie.setDataFreer(WordNatureFreer);
        datrie.setDataReader(ReadWordNatureFromFile);
        datrie.setDataWriter(WriteWordNatureToFile);
    }

    // read binary data from a file.
    Dictionary(const string & file)
    {
        datrie.setMemPool(&mem_pool);
        datrie.setDataReader(ReadWordNatureFromFile);
        datrie.setDataWriter(WriteWordNatureToFile);
        FILE * pfile = fopen(file.c_str(),"rb");
        //assert(pfile != NULL);
        if(!pfile) {
            cerr<<"File not exist:"<<file.c_str()<<endl;
            return;
        }

        readFromFile(pfile);
        fclose(pfile);
    }

    bool addNature(const wstring& nature)
    {
        if(nature_index.find(nature) != nature_index.end()) {
            return false;
        }
        natures.push_back(nature);
        nature_index[nature] = natures.size() - 1;
        return true;
    }

    int getNatureIndex(const wstring &nature) const
    {
        if(nature_index.find(nature) != nature_index.end()) {
            return nature_index.find(nature)->second;
        }
        return -1;
    }

    wstring getNature(int index) const
    {
        if(index < natures.size()) {
            return natures[index];
        }
        return L"UNDEF";
    }

    bool addWordInfo(const wstring & word, WordNature * info)
    {
        return datrie.add(word, info);
    }

    const WordNature * getWordInfo(const wstring & word) const
    {
        return (const WordNature *)datrie.retrieve(word.c_str());
    }

    int getNatureFreq(const wstring & word, const wstring & nature) const
    {
        const WordNature * pnatures = getWordInfo(word);
        if(pnatures != NULL){
            int idx = getNatureIndex(nature);
            if(idx >= 0){
                return pnatures->getAttrValue(idx);
            }
        }

        return 0;
    }

    int getTotalFreq(const wstring & word) const
    {
        const WordNature * pnatures = getWordInfo(word);
        if(pnatures != NULL){
            return pnatures->sumOfValues();
        }

        return 0;
    }

    bool existPrefix(const wstring & prefix) const
    {
        return datrie.containsPrefix(prefix);
    }

    bool writeToFile(const string & file)
    {
        FILE * pfile = fopen(file.c_str(),"wb");
        assert(pfile != NULL);
        bool result = false;
        int nature_size = natures.size();
        if(!file_write_int32(pfile, DICT_SIGNATURE)
                || !file_write_int32(pfile, nature_size)) {
            goto end_write;
        }

        for(int i = 0; i < nature_size; i++) {
            WriteTrieStrToFile(pfile, natures[i].c_str());
        }

        if(!datrie.writeToFile(pfile)) {
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
        if(!file_read_int32(pfile, &signature) || signature != DICT_SIGNATURE) {
            assert(false);
            return;
        }

        // read Natures.
        int nature_size;
        if(!file_read_int32(pfile, &nature_size)
                || nature_size > 0x0fffffff
                || nature_size < 0) {
            assert(false);
            return;
        }

        for(int i = 0; i < nature_size; i++) {
            wstring nature = (wchar_t *)ReadTrieStrFromFile(pfile, &mem_pool);
            addNature(nature);
        }
        // read dat
        datrie.readFromFile(pfile);
    }

    protected:
        DATrie datrie;
        vector<wstring> natures;
        hash_map<wstring, int> nature_index;
        MemoryPool<> mem_pool;

};

class ShiftContext:public Dictionary{
public:
    ShiftContext():Dictionary(){
        genUnknownNauture();
    }

    ShiftContext(const string & file):Dictionary(file){
        genUnknownNauture();
    }

    ~ShiftContext(){
        if(_unknownNature){
            delete _unknownNature;
        }
    }

    int getNatureTotal(int natureIndex) const
    {
        if(natureIndex < natures.size()){
            getTotalFreq(natures[natureIndex]);
        }
        return 0;
    }

    double getCoProb(int from, int to) const {
        if(from < natures.size() || to < natures.size()){
            const WordNature * fromInfo = getWordInfo(natures[from]);
            double toFreq = fromInfo->getAttrValue(to) + 1.0;
            double FromTotal = fromInfo->sumOfValues() + 44.0;
            return toFreq / FromTotal;
        }

        return 0.000001;
    }

    const WordNature * getUnknownNature() const{
        return _unknownNature;
    }

private:
    void genUnknownNauture(){
        _unknownNature = new WordNature();
        for(int i = 0; i < natures.size(); i++ ){
             _unknownNature->setAttrValue(i, 1);
        }
    }

private:
    WordNature * _unknownNature;
};

}
