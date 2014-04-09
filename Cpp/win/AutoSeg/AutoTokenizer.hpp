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
#include "Tokenizer.hpp"

#include "DictFactory.hpp"
using namespace std;
namespace mingspy
{
class AutoTokenizer: public ITokenizer
{
public:
    typedef void (splitFn)(const wstring &str, vector<Token> & result);
    /*
    * according dictionary, forward split the str. From the head char
    * of input, this method will lookup the dictionary, then take the
    * longest word from the str step by step.
    * For example: "AB" "ABCD" "BCD" "BCDE" "EF" are
    * words in dictionary; when given str="ABCDEF", the split result
    * will be: ABCD/ EF/
    * @param str : the input str to split.
    * @param result : result words
    */
    void maxSplit(const wstring &str, vector<Token> & result)
    {
        _worker.maxSplit(str, result);
    }

    /*
    * split out all possible words in the given str.
    * @param str : the input str to split.
    * @param result : result words.
    */
    void fullSplit(const wstring &str, vector<Token> & result)
    {
        _worker.fullSplit(str, result);
    }

    /*
    * Split the word, and try to resolve ambiguities by one gram.
    * @param str : the input str to split.
    * @param result : result words.
    */
    void uniGramSplit(const wstring & str, vector<Token> & result)
    {
        DoSplit(str, result, 1);        
    }

    void biGramSplit(const wstring & str, vector<Token> & result)
    {
        DoSplit(str, result, 2);        
    }

    void mixSplit(const wstring & str, vector<Token> & result){
        DoSplit(str, result, 3);
    }

    static void sentanceSplit(const wstring & str, vector<Token> & result)
    {
        const PunctionDictionary & puncs = DictFactory::Puntions();
        int off = 0;
        int cur = 0;
        const wchar_t * p = str.c_str();
        while(p[cur]) {
            if(puncs.exists(p[cur])) {
                if(cur - off > 0) {
                    result.push_back(Token(off, cur - off));
                }
                result.push_back(Token(cur, 1, -1));
                off = cur + 1;
            }
            cur ++;
        }

        if(off < str.length()) {
            result.push_back(Token(off, str.length() - off));
        }
    }

    void setMaxPaths(int maxs)
    {
        _worker.setMaxPaths(maxs);
    }

private:
    void DoSplit(const wstring & str, vector<Token> & result, int fn){
        vector<Token>  sentances;
        sentanceSplit(str, sentances);
        int prevTokes = 0;
        int off = 0;
        for(int i = 0; i < sentances.size(); i++) {
            if(sentances[i]._attr >= 0) {
                switch(fn){
                case 1:
                    _worker.uniGramSplit(str.substr(sentances[i]._off, sentances[i]._len), result);
                    break;
                case 2:
                    _worker.biGramSplit(str.substr(sentances[i]._off, sentances[i]._len), result);
                    break;
                case 3:
                    _worker.mixSplit(str.substr(sentances[i]._off, sentances[i]._len), result);
                    break;
                }
                
                for(int k = prevTokes; k < result.size(); k++) {
                    result[k]._off += off;
                }
            } else {
                result.push_back(sentances[i]);
            }
            off+= sentances[i]._len;
            prevTokes = result.size();
        }
    }

private:
    Tokenizer _worker;
};
}