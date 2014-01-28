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
#include "DictFactory.hpp"
#include "wcharHelper.hpp"
using namespace std;

namespace mingspy{
    
    struct Token{
        int _attr; // type of this token or attribute index.
        int _off; // start index.
        int _len; // len
        Token(int start = 0, int len = 0, int attr = 0)
            :_off(start),_len(len),_attr(attr)
        {
        }
    };
    
    class Tokenizer{
    public:
        void analysis(const wstring & str){
            vector<wstring> results;
            int len = str.length();
            const WordDictionary &core = DictFactory::CoreDict();
            for(int i = 0; i < len; ){
                // merge number and alpha
                wtype itype = getWcharType(str.at(i));
                int j = i + 1;
                if(itype == DIGIT || itype == ALPHA){
                    while(j < len){
                        wtype jtype = getWcharType(str.at(j++));
                        if(jtype != itype){
                            break;
                        }
                        
                    }
                }

                for(j-=i; j <= len - i; j ++){
                    wstring word = str.substr(i, j);
                    if(!core.existPrefix(word)){
                        break;
                    }
                }

                if(j != 1){
                    j --;
                }

                results.push_back(str.substr(i,j));

                if(itype == DIGIT || itype == ALPHA){
                    i += j;
                }else{
                    i++;
                }

            }

            for(int i = 0; i < results.size(); i++){
                wcout<<results[i]<<L"|";
            }
            wcout<<endl;
        }

        /*
        * Split atoms from the given str:
        * Atoms are in form of:
        *   A Chinese char
        *   An English str
        *   A number
        */
        static void atomSplit(const wstring & str, vector<Token> & atoms){
            int len = str.length();
            for(int i = 0; i < len; ){
                // merge number and alpha
                wtype itype = getWcharType(str.at(i));
                if(itype == DIGIT || itype == ALPHA){
                    int j = i+1;
                    while(j <len && (getWcharType(str.at(j)) == itype)){
                        j++;
                    }
                    atoms.push_back(Token( i, j - i, (int)itype));
                    i = j;
                }else{
                    atoms.push_back(Token(i++, 1, (int)itype));
                }
            }
        }
    };
}
