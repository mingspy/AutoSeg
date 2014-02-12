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
#include <vector>
#include "MemLeaksCheck.h"


using namespace std;

/*
* Split the given @src by @separator, the @separator will not appear in result.
* @param src - the string to be split.
* @param separator - the separator.
* @param result - the split pieces of string.
*/
void split(const string& src, const string& separator, vector<string>& result)
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
            result.push_back(substring);
            start = str.find_first_not_of(separator,index);
            if (start == string::npos) return;
        }
    }
    while(index != string::npos);

    //the last token
    substring = str.substr(start);
    result.push_back(substring);
}

/*
* Split the given @src by @separator, the @separator will not appear in result.
* @param src - the string to be split.
* @param separator - the separator.
* @param result - the split pieces of string.
*/
void split(const wstring& src, const wstring& separator, vector<wstring>& result)
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
            result.push_back(substring);
            start = str.find_first_not_of(separator,index);
            if (start == wstring::npos) return;
        }
    }
    while(index != wstring::npos);

    //the last token
    substring = str.substr(start);
    result.push_back(substring);
}

/*
* Trim the given string, it removes all white space at the left and right side
* of the given string until meets a non-white space character.
*
* @param str - the string to be trim
* @return - the trimmed str.
*/
const string trim(const string& str)
{
    std::string::size_type first = str.find_first_not_of(" \n\t\r\0xb");
    if (first == std::string::npos)
    {
        return std::string();
    }
    else
    {
        std::string::size_type last = str.find_last_not_of(" \n\t\r\0xb");
        return str.substr( first, last - first + 1);
    }
}

/*
* Trim the given string, it removes all white space at the left and right side
* of the given string until meets a non-white space character.
*
* @param str - the string to be trim
* @return - the trimmed str.
*/
const wstring trim(const wstring& str)
{
    std::wstring::size_type first = str.find_first_not_of(L" \n\t\r\0xb");
    if (first == std::wstring::npos)
    {
        return std::wstring();
    }
    else
    {
        std::wstring::size_type last = str.find_last_not_of(L" \n\t\r\0xb");
        return str.substr( first, last - first + 1);
    }
}
