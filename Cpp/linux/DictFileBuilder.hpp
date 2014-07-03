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
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include "Dictionary.hpp"
#include "FileUtils.hpp"
#include "SparseInstance.hpp"
#include "MemLeaksCheck.h"
#include "MSTimer.hpp"

using namespace std;

namespace mingspy
{
 static const wchar_t wordSeperator = L'\t';
 static const wstring natureSeperator = L",";
 static const wchar_t freqSeperator = L':';

class DictFileBuilder
{
public:
    static bool buildDict(const vector<string> &files, const string & output)
    {

        MSTimer timer;
        Dictionary dict;
        int word_count = 0;
        dict.addNature(NATURE_UNDEF);
        for(int i = 0; i< files.size(); i++) {
            UTF8FileReader reader(files[i]);
            wstring * line;
            line = reader.getLine();
            if(line == NULL) {
                cerr<<"empty file."<<files[i].c_str()<<endl;
                continue;
            }

            wstring natureHead = L"@WordPOS\t";
            wstring::size_type natureindex = line->find(natureHead);
            if(natureindex == wstring::npos) {
                wcerr<<L"can't find the natures, treat as user define dictionary."<<endl;
                wcerr<<L"the line is:"<<line<<endl;
                procALine(line, dict);
            }else{
                wstring natures = line->substr(natureindex+natureHead.length());
                vector<wstring> vec;
                split(natures,L",",vec);
                for(int i = 0; i < vec.size(); i++) {
                    dict.addNature(vec[i]);
                }
                dict.addNature(NATURE_UNDEF);
            }

            // 处理词信息
            while((line = reader.getLine()) != NULL) {
                procALine(line, dict);
                if(++word_count%100 == 0) {
                    cout<<"\radded words -> "<<word_count;
                }	
            }

        }

        double load_word_end_time = timer.elapsed();
        cout<<"\n all words added. now write to file."<<endl;
        bool result = dict.writeToFile(output);
        double end_time = timer.elapsed();
        cout<<"build finished! total used:"<<(end_time)
            <<"s\n   load words:"<<word_count<<" used:"<<(load_word_end_time)
            <<"s\n   serialize used:"<<(end_time - load_word_end_time)<<endl;

        return result;
    }

    static void procALine( wstring * line, Dictionary & dict ) 
    {
        wstring::size_type wordIndex = line->find_first_of(wordSeperator);
        wstring word;
        WordNature * info = new WordNature();
        if(wordIndex == wstring::npos) {
            word = *line;
            int index = dict.getNatureIndex(NATURE_UNDEF);
            info->setAttrValue(index, 1);
        } else {
            word = line->substr(0, wordIndex);
            wstring infostr = line->substr(wordIndex + 1);
            if(!infostr.empty()) {
                vector<wstring> infos;
                split(infostr, natureSeperator, infos);
                for(int i = 0; i < infos.size(); i++) {
                    wstring::size_type freqIndex = infos[i].find_first_of(freqSeperator);
                    wstring nature = infos[i].substr(0,freqIndex);
                    wstring freq = infos[i].substr(freqIndex + 1);
                    int d_freq = wcstol(freq.c_str(), NULL, 10);
                    int index = dict.getNatureIndex(nature);
                    if(index == -1) {
                        wcerr<<L"The nature not exist in nature list of the file header:"
                            <<nature<<" line:"<<*line<<endl;
                        exit(-1);
                    }
                    info->setAttrValue(index, d_freq);
                }
            }
        }

        WordNature * natures = (WordNature *)dict.getWordInfo(word);
        if(natures == NULL) {
            dict.addWordInfo(word, info);
        } else {
            for(int i = 0; i< info->numValues(); i++) {
                int freq = info->valueAt(i) + natures->getAttrValue(info->attrAt(i));
                natures->setAttrValue(info->attrAt(i), freq);
            }
            delete info;
        }
    }

    static bool buildInverseDict(const string & inputpath, const string & output)
    {
        MSTimer timer;
        vector<string> files;
        getFiles(inputpath, files);
        Dictionary dict;
        int word_count = 0;

        for(int i = 0; i< files.size(); i++) {
            UTF8FileReader reader(inputpath);
            wstring * line;
            line = reader.getLine();
            if(line == NULL) {
                cerr<<"empty file."<<endl;
                return false;
            }

            wstring natureHead = L"@WordPOS\t";
            wstring::size_type natureindex = line->find(natureHead);
            if(natureindex == wstring::npos) {
                cerr<<"can't find the natures, maybe not a dictionary words file."<<endl;
                return false;
            }

            wstring natures = line->substr(natureindex+natureHead.length());
            vector<wstring> vec;
            split(natures,L",",vec);

            for(int i = 0; i < vec.size(); i++) {
                dict.addNature(vec[i]);
            }

            wchar_t wordSeperator = L'\t';
            wstring natureSeperator = L",";
            wchar_t freqSeperator = L':';
            // 处理词信息
            while((line = reader.getLine()) != NULL) {
                wstring::size_type wordIndex = line->find_first_of(wordSeperator);
                wstring word;
                WordNature * info = NULL;
                if(wordIndex == wstring::npos) {
                    word = (*line);
                } else {
                    word = line->substr(0, wordIndex);
                    wstring infostr = line->substr(wordIndex + 1);
                    if(!infostr.empty()) {
                        info = new WordNature();
                        vector<wstring> infos;
                        split(infostr, natureSeperator, infos);
                        for(int i = 0; i < infos.size(); i++) {
                            wstring::size_type freqIndex = infos[i].find_first_of(freqSeperator);
                            wstring nature = infos[i].substr(0,freqIndex);
                            wstring freq = infos[i].substr(freqIndex + 1);
                            int d_freq = wcstol(freq.c_str(), NULL,10);
                            int index = dict.getNatureIndex(nature);
                            if(index == -1) {
                                wcerr<<L"The nature not exist in nature list of the file header:"
                                     <<nature<<" line:"<<*line<<endl;
                                exit(-1);
                            }
                            info->setAttrValue(index, d_freq);
                        }
                    }
                }

                reverse(word.begin(), word.end());
                dict.addWordInfo(word, info);

                if(++word_count%100 == 0) {
                    cout<<"\radded words -> "<<word_count;
                }
            }
        }
        double load_word_end_time = timer.elapsed();
        cout<<"\n all words added. now write to file."<<endl;
        bool result = dict.writeToFile(output);
        double end_time = timer.elapsed();
        cout<<"build finished! total used:"<<(end_time)
            <<"ms\n   load words:"<<word_count<<" used:"<<(load_word_end_time)
            <<"ms\n   serialize used:"<<(end_time - load_word_end_time)<<endl;

        return result;
    }
};
}

