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
#include "MemLeaksCheck.h"


using namespace std;

void split(const string& src, const string& separator, vector<string>& dest)
{
    string str = src;
    string substring;
    string::size_type start = 0, index;

    do
    {
        index = str.find_first_of(separator,start);
        if (index != string::npos)
        {    
            substring = str.substr(start,index-start);
            dest.push_back(substring);
            start = str.find_first_not_of(separator,index);
            if (start == string::npos) return;
        }
    }while(index != string::npos);

    //the last token
    substring = str.substr(start);
    dest.push_back(substring);
}

void split(const wstring& src, const wstring& separator, vector<wstring>& dest)
{
    if(src.empty()) return;

    wstring str = src;
    wstring substring;
    wstring::size_type start = 0, index;

    do
    {
        index = str.find_first_of(separator,start);
        if (index != wstring::npos)
        {    
            substring = str.substr(start,index-start);
            dest.push_back(substring);
            start = str.find_first_not_of(separator,index);
            if (start == wstring::npos) return;
        }
    }while(index != wstring::npos);

    //the last token
    substring = str.substr(start);
    dest.push_back(substring);
}


const string trim(const string istring)
{
    std::string::size_type first = istring.find_first_not_of(" \n\t\r\0xb");
    if (first == std::string::npos) {
        return std::string();
    }
    else {
        std::string::size_type last = istring.find_last_not_of(" \n\t\r\0xb");
        return istring.substr( first, last - first + 1);
    }
}

const wstring trim(const wstring istring)
{
    std::wstring::size_type first = istring.find_first_not_of(L" \n\t\r\0xb");
    if (first == std::wstring::npos) {
        return std::wstring();
    }
    else {
        std::wstring::size_type last = istring.find_last_not_of(L" \n\t\r\0xb");
        return istring.substr( first, last - first + 1);
    }
}