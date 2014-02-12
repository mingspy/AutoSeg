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

/**
* In windows using MultibyeToWideChar.
* In unix using iconv
*/

#include <iostream>
#include <string>
#include <vector>
#include "MemLeaksCheck.h"
#if _MSC_VER > 1000
#include <windows.h>
#else
#include <iconv.h>
#pragma comment(lib,"iconv.lib")
#endif

using namespace std;

#if _MSC_VER > 1000
wstring Utf8ToUnicode( const string& str )
{
    int  len = str.length();
    int  unicodeLen = ::MultiByteToWideChar(CP_UTF8,0,str.c_str(),-1,NULL,0);
    wchar_t *  pUnicode = new  wchar_t[unicodeLen+1];
    memset(pUnicode,0,(unicodeLen+1)*sizeof(wchar_t));

    ::MultiByteToWideChar(CP_UTF8,0,str.c_str(),-1,(LPWSTR)pUnicode,unicodeLen);
    wstring  rt = pUnicode;
    delete [] pUnicode;
    return  rt;
}
#else
int code_convert(const char *from_charset,const char *to_charset,const char *inbuf, size_t inlen,
    char *outbuf, size_t outlen)
{
    const char **pin = &inbuf;
    char **pout = &outbuf;

    iconv_t cd = iconv_open(to_charset,from_charset);
    if (cd==0) return -1;

    memset(outbuf,0,outlen);

    if (iconv(cd, (char **)pin, &inlen, pout, &outlen)==-1) return -1;
    iconv_close(cd);

    return 0;
}

wstring Utf8ToUnicode( const string& str )
{
    int  len = str.length();
    wchar_t * pUnicode = new  wchar_t[len+1];
    
    code_convert("UTF-8", "UTF-16", str.c_str(), len, (char *)pUnicode, (len+1)*sizeof(wchar_t));
    wstring  rt = ( wchar_t* )pUnicode;

    delete []  pUnicode;
    return  rt;
}
#endif
