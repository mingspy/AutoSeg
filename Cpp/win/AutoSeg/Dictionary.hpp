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
#include "Configuration.hpp"
#include <wchar.h>

using namespace std;

namespace mingspy
{

const wstring  NATURE_UNDEF = L"UDF";
const wstring NATURE_FREQTOTAL=L"FREQTOL";
const int MAX_WORD_LEN = 2000;
class Dictionary
{
private:
    static const int DICT_SIGNATURE = 0x112fd0ac;
public:
    Dictionary()
    {
        init();
    }

    // read binary data from a file.
    Dictionary(const string & file)
    {
        init();
        cout<<"loading file: "<<file.c_str()<<endl;
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
        return NATURE_UNDEF;
    }

    bool addWordInfo(const wstring & word, WordNature * info)
    {
        return datrie.add(word, info);
    }

    virtual const WordNature * getWordInfo(const wstring & word) const
    {
        return (const WordNature *)datrie.retrieve(word);
    }

    int getNatureFreq(const wstring & word, const wstring & nature) const
    {
        const WordNature * pnatures = getWordInfo(word);
        if(pnatures != NULL) {
            int idx = getNatureIndex(nature);
            if(idx >= 0) {
                return pnatures->getAttrValue(idx);
            }
        }

        return 0;
    }

    int getTotalFreq(const wstring & word) const
    {
        const WordNature * pnatures = getWordInfo(word);
        if(pnatures != NULL) {
            return pnatures->sumOfValues();
        }

        return 0;
    }

    double getProb(const wstring & word) const
    {
        return (getTotalFreq(word) + 1.0) / TOTAL_FREQ;
    }
    virtual bool existPrefix(const wstring & prefix) const
    {
        return datrie.containsPrefix(prefix);
    }

    bool writeToFile(const string & file)
    {
        FILE * pfile = fopen(file.c_str(),"wb");
        assert(pfile != NULL);
        Serializer serializer(pfile);
        bool result = false;
        int nature_size = natures.size();
        if(!serializer.writeInt32(DICT_SIGNATURE)
                || !serializer.writeInt32(nature_size)) {
            goto end_write;
        }

        for(int i = 0; i < nature_size; i++) {
            serializer.writeWstr(natures[i]);
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
    Dictionary(const Dictionary &);

    void init()
    {
        datrie.setMemPool(&mem_pool);
        datrie.getTail().setDataFreer(WordNatureFreer);
        datrie.getTail().setDataReader(WordNatureReader);
        datrie.getTail().setDataWriter(WordNatureWriter);
        TOTAL_FREQ = Configuration::instance().getInt("TOTAL_FREQ", 10000000);
    }

    void readFromFile(FILE * pfile)
    {
        // read header
        Serializer serializer(pfile);
        int signature;
        if((signature = serializer.readInt32()) != DICT_SIGNATURE) {
            assert(false);
            return;
        }

        // read Natures.
        int nature_size = serializer.readInt32();
        if( nature_size > 0x0fffffff
                || nature_size < 0) {
            assert(false);
            return;
        }
        wchar_t buf[512];
        for(int i = 0; i < nature_size; i++) {
            addNature(serializer.readWstr(buf));
        }
        // read dat
        datrie.readFromFile(pfile);
    }

protected:
    DATrie datrie;
    vector<wstring> natures;
    hash_map<wstring, int> nature_index;
    MemoryPool<> mem_pool;
    double TOTAL_FREQ;

};

class NatureProbTable:public Dictionary
{
public:
    NatureProbTable():Dictionary()
    {
        genUnknownNauture();
    }

    NatureProbTable(const string & file):Dictionary(file)
    {
        genUnknownNauture();
    }

    ~NatureProbTable()
    {
        if(_unknownNature) {
            delete _unknownNature;
        }
    }

    int getNatureTotal(int natureIndex) const
    {
        if(natureIndex < natures.size()) {
            return getTotalFreq(natures[natureIndex]);
        }
        return 0;
    }

    double getCoProb(int from, int to) const
    {
        if(from < natures.size() && to < natures.size()) {
            const WordNature * fromInfo = getWordInfo(natures[from]);
            if(fromInfo) {
                double toFreq = fromInfo->getAttrValue(to) + 1.0;
                double FromTotal = fromInfo->sumOfValues() + 44.0;
                return toFreq / FromTotal;
            }
        }

        return 3.0/TOTAL_FREQ;
    }

    const WordNature * getUnknownNature() const
    {
        return _unknownNature;
    }

private:
    NatureProbTable(const NatureProbTable &);
    void genUnknownNauture()
    {
        _unknownNature = new WordNature();
        for(int i = 0; i < natures.size(); i++ ) {
            _unknownNature->setAttrValue(i, 1);
        }
    }

private:
    WordNature * _unknownNature;
};

class UserDict:public Dictionary
{
public:
    UserDict(const string & file):Dictionary(file)
    {
    }

    virtual const WordNature * getWordInfo(const wstring & word) const
    {
        const WordNature * pinfo =  (const WordNature *)datrie.retrieve(word.c_str());
        if(pinfo == NULL) {
            pinfo = (const WordNature *)user_datrie.retrieve(word.c_str());
        }

        return pinfo;
    }

    virtual bool existPrefix(const wstring & prefix) const
    {
        if(datrie.containsPrefix(prefix)) {
            return true;
        }

        return user_datrie.containsPrefix(prefix);
    }

    void loadUserDict(const vector<string> & files)
    {
        int udf_idx = getNatureIndex(NATURE_UNDEF);
        if(udf_idx < 0) {
            addNature(NATURE_UNDEF);
            udf_idx = getNatureIndex(NATURE_UNDEF);
        }

        int count = 0;
        for(int i = 0; i < files.size(); i++) {

            cout<<"\rloading user dictionary:"<<files[i].c_str();
            UTF8FileReader reader(files[i]);
            wstring * line;
            while((line = reader.getLine())) {
                wstring::size_type wordIndex = line->find_first_of(wordSeperator);
                wstring word;
                wstring wordinfo;
                if(wordIndex == wstring::npos) {
                    word = *line;
                } else {
                    word = line->substr(0, wordIndex);
                    if(wordIndex < line->length()) {
                        wordinfo = line->substr(wordIndex + 1);
                    }
                }
                if(wordinfo.empty()) {
                    if(!getWordInfo(word)) {
                        WordNature *nature = new WordNature();
                        nature->setAttrValue(udf_idx, 1);
                        user_datrie.add(word, nature);
                        count ++;
                    }
                } else {
                    WordNature * info = new WordNature();
                    vector<wstring> infos;
                    split(wordinfo, natureSeperator, infos);
                    for(int i = 0; i < infos.size(); i++) {
                        wstring::size_type freqIndex = infos[i].find_first_of(freqSeperator);
                        wstring nature;
                        int d_freq = 1;
                        if(freqIndex == wstring::npos) {
                            nature = infos[i];
                        } else {
                            nature = infos[i].substr(0,freqIndex);
                            wstring freq = infos[i].substr(freqIndex + 1);
                            d_freq = wcstol(freq.c_str(), NULL, 10);
                        }

                        int index = getNatureIndex(nature);
                        if(index == -1) {
                            wcerr<<L"The nature not exist in nature list of the file header:"
                                 <<nature<<" line:"<<*line<<endl;
                            addNature(nature);
                            index = getNatureIndex(nature);
                        }
                        info->setAttrValue(index, d_freq);
                    }

                    WordNature * natures = (WordNature *)getWordInfo(word);
                    if(natures == NULL) {
                        user_datrie.add(word, info);
                    } else {
                        for(int i = 0; i< info->numValues(); i++) {
                            int freq = info->valueAt(i) + natures->getAttrValue(info->attrAt(i));
                            natures->setAttrValue(info->attrAt(i), freq);
                        }
                        delete info;
                    }
                    count++;
                }

                if(count % 1000 == 0) {
                    cout<<"\r added -> "<<count;
                }
            }

        }
        cout<<endl;
    }
private:
    UserDict(const UserDict &);
protected:
    DATrie user_datrie;
};

}
