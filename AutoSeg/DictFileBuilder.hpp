/*
 * Dictionary Library
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
#include <vector>
#include "WordDictionary.hpp"
#include "FileUtils.hpp"
#include "SparseInstance.hpp"
#include "MemLeaksCheck.h"

using namespace std;

namespace mingspy{
    class DictFileBuilder{
    public:
        static bool buildDict(const string & inputpath, const string & output){
            long start_time = GetTickCount();
            UTF8FileReader reader(inputpath);
            wstring * line;
            line = reader.getLine();
            if(line == NULL){
                cerr<<"empty file."<<endl;
                return false;
            }
            
            wstring natureHead = L"@词性\t";
            wstring::size_type natureindex = line->find(natureHead);
            if(natureindex == wstring::npos){
                cerr<<"can't find the natures, maybe not a dictionary words file."<<endl;
                return false;
            }

            wstring natures = line->substr(natureindex+natureHead.length());
            vector<wstring> vec;
            split(natures,L",",vec);
            WordDictionary dict;
            for(int i = 0; i < vec.size(); i++){
                dict.addNature(vec[i]);
            }

            wchar_t wordSeperator = L'\t';
            wstring natureSeperator = L",";
            wchar_t freqSeperator = L':';
            int word_count = 0;
            // 处理词信息
            while((line = reader.getLine()) != NULL){
                wstring::size_type wordIndex = line->find_first_of(wordSeperator);
                wstring word;
                SparseInstance * info = NULL;
                if(wordIndex == wstring::npos){
                    word = *line;
                }else{
                    word = line->substr(0, wordIndex);
                    wstring infostr = line->substr(wordIndex + 1);
                    if(!infostr.empty()){
                        info = new SparseInstance();
                        vector<wstring> infos;
                        split(infostr, natureSeperator, infos);
                        for(int i = 0; i < infos.size(); i++){
                            wstring::size_type freqIndex = infos[i].find_first_of(freqSeperator);
                            wstring nature = infos[i].substr(0,freqIndex);
                            wstring freq = infos[i].substr(freqIndex + 1);
                            double d_freq = stod(freq);
                            int index = dict.getNatureIndex(nature);
                            if(index == -1){
                                wcerr<<L"The nature not exist in nature list of the file header:"
                                    <<nature<<" line:"<<*line<<endl;
                                exit(-1);
                            }
                            info->setValue(index, d_freq);
                        }
                    }
                }

                dict.addWordInfo(word, info);
                
                if(++word_count%100 == 0){
                    cout<<"\radded words -> "<<word_count;
                }
            }
        
            long load_word_end_time = GetTickCount();
            cout<<"\n all words added. now write to file."<<endl;
            bool result = dict.writeToFile(output);
            long end_time = GetTickCount();
            cout<<"build finished! total used:"<<(end_time - start_time)
                <<"ms\n   load words:"<<word_count<<" used:"<<(load_word_end_time - start_time)
                <<"ms\n   serialize used:"<<(end_time - load_word_end_time)<<endl;

            return result;
        }

    };
}

