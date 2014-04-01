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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include "StrUtils.hpp"
#include "CodeUtils.hpp"


using namespace std;

bool
file_read_int32 (FILE *file, int *o_val)
{
    unsigned char   buff[4];

    if (fread (buff, 4, 1, file) == 1) {
        *o_val = (buff[0] << 24) | (buff[1] << 16) |  (buff[2] << 8) | buff[3];
        return true;
    }

    return false;
}

bool
file_write_int32 (FILE *file, int val)
{
    unsigned char   buff[4];

    buff[0] = (val >> 24) & 0xff;
    buff[1] = (val >> 16) & 0xff;
    buff[2] = (val >> 8) & 0xff;
    buff[3] = val & 0xff;

    return (fwrite (buff, 4, 1, file) == 1);
}

bool
file_read_chars (FILE *file, char *buff, int len)
{
    return (fread (buff, sizeof (char), len, file) == len);
}

bool
file_write_chars (FILE *file, const char *buff, int len)
{
    return (fwrite (buff, sizeof (char), len, file) == len);
}

bool
file_read_int16 (FILE *file, short *o_val)
{
    unsigned char   buff[2];

    if (fread (buff, 2, 1, file) == 1) {
        *o_val = (buff[0] << 8) | buff[1];
        return true;
    }

    return false;
}

bool
file_write_int16 (FILE *file, short val)
{
    unsigned char   buff[2];

    buff[0] = val >> 8;
    buff[1] = val & 0xff;

    return (fwrite (buff, 2, 1, file) == 1);
}

bool
file_read_int8 (FILE *file, char *o_val)
{
    return (fread (o_val, sizeof (char), 1, file) == 1);
}

bool
file_write_int8 (FILE *file, char val)
{
    return (fwrite (&val, sizeof (char), 1, file) == 1);
}


class UTF8FileReader
{
private:
    ifstream inf;
    wstring lastLine;
public:
    UTF8FileReader(const string & file)
    {
        inf.open(file.c_str());
    }
    ~UTF8FileReader()
    {
        inf.close();
    }

    wstring * getLine()
    {
        string line;
        do {
            if(getline(inf,line)) {
                line = trim(line);
                if(!line.empty()) {
                    lastLine = Utf8ToUnicode(line);
                    return &lastLine;
                }
            }
        } while(!inf.eof());

        return NULL;
    }

    static size_t size(const string& filename)
    {
        ifstream file (filename.c_str(),   ios::in|ios::binary|ios::ate);
        if(!file.good()) {
            return 0;
        }

        file.seekg(0, ios::end);
        long length = file.tellg();
        file.close();
        return length;
    }

private:
    UTF8FileReader();
    UTF8FileReader(const UTF8FileReader &);
    UTF8FileReader & operator = (const UTF8FileReader &);
};

