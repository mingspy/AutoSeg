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
#include <algorithm>
#include "DictFactory.hpp"
#include "wcharHelper.hpp"
#include "Tokenizer.hpp"

using namespace std;

namespace mingspy
{
class InverseTokenizer: public Tokenizer
{
public:
    virtual void maxSplit(const wstring &str, vector<Token> & result)
    {
        wstring inverseStr(str.rbegin(),str.rend());
        vector<Token> atoms;
        atomSplit(inverseStr,atoms);
        maxMatch(inverseStr, atoms, result);
        reverseResult(result, str.length());
    }

    virtual void fullSplit(const wstring &str, vector<Token> & result)
    {
        wstring inverseStr(str.rbegin(),str.rend());
        vector<Token> atoms;
        atomSplit(inverseStr,atoms);
        fullMatch(inverseStr,atoms,result);
        reverseResult(result, str.length());
    }
protected:
    virtual bool exists(const wstring & word)
    {
        return DictFactory::InverseCoreDict().getWordInfo(word) != NULL;
    }

    virtual bool existPrefix(const wstring & word)
    {
        return DictFactory::InverseCoreDict().existPrefix(word);
    }

    void reverseResult(vector<Token> & result, int strlen)
    {
        int i = 0;
        int j = result.size() - 1;
        int t = 0;
        while(i < j)
        {
            // swap index
            t = strlen - result[i]._off - result[i]._len;
            result[i]._off = strlen - result[j]._off - result[j]._len;
            result[j]._off = t;
            // swap len
            t = result[i]._len;
            result[i]._len = result[j]._len;
            result[j]._len = t;
            //swap attribute
            t = result[i]._attr;
            result[i]._attr = result[j]._attr;
            result[j]._attr = t;

            i++;
            j--;
        }
    }
};
}

