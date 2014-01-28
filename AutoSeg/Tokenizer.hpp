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
        virtual void maxSplit(const wstring &str, vector<Token> & result){
            vector<Token> atoms;
            atomSplit(str,atoms);
            maxMatch(str, atoms, result);
        }

        virtual void fullSplit(const wstring &str, vector<Token> & result){
            vector<Token> atoms;
            atomSplit(str,atoms);
            fullMatch(str,atoms,result);
        }

        void output(const wstring &str, const vector<Token> & tokens, vector<wstring>& result){
            for(int i = 0; i < tokens.size(); i++){
                result.push_back(str.substr(tokens[i]._off, tokens[i]._len));
            }
        }

        void analysis(const wstring & str){
        }

    protected:
        virtual bool exists(const wstring & word){
            return DictFactory::CoreDict().getWordInfo(word) != NULL;
        }

        virtual bool existPrefix(const wstring & word){
            return DictFactory::CoreDict().existPrefix(word);
        }

        /*
        * try to find all the possible words.
        * assume str = "ABCDE";
        * and AB ABC BCD E in the word dictionary;
        * the result will be :AB, ABC,BCD,D,E
        */
        void fullMatch(const wstring &str, const vector<Token> & atoms, vector<Token> &result){
            
            const int atome_size = atoms.size();
            int lastj = -1;
            for(int i = 0; i < atome_size; i++){
                
                for(int j = i + 1; j < atome_size; j++){
                    wstring word = str.substr(
                        atoms[i]._off, atoms[j]._off + atoms[j]._len - atoms[i]._off);
                    if(exists(word)){
                        result.push_back(
                            Token(atoms[i]._off, atoms[j]._off + atoms[j]._len - atoms[i]._off));
                        lastj = j;
                    }else if(!existPrefix(word)){
                        break;
                    }
                }
                if(i > lastj){
                    result.push_back(
                            Token(atoms[i]._off, atoms[i]._len));
                }
                
            }
           
        }

        /*
        * try to split the str with huge words.
        * assume str = "ABCDE";
        * and AB ABC BCD E in the word dictionary;
        * the result will be : ABC,D,E
        */
        void maxMatch(const wstring &str, const vector<Token> & atoms, vector<Token> &result){
            for(int i = 0; i < atoms.size(); ){
                int j = i + 1;
                for(; j < atoms.size(); j++){
                    wstring word = str.substr(atoms[i]._off, atoms[j]._off + atoms[j]._len - atoms[i]._off);
                    if(!existPrefix(word)){
                        break;
                    }
                }
                result.push_back(Token(atoms[i]._off, atoms[j-1]._off + atoms[j-1]._len - atoms[i]._off));
                i = j;
            }
        }

        /*
        * Split atoms from given str:
        * Atoms are in form of:
        *   A Chinese char
        *   An English str
        *   A number
        */
        void atomSplit(const wstring & str, vector<Token> & atoms){
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
