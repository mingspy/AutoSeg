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
#include <fstream>
#include <string>
#include "StrUtils.hpp"
#include "CodeUtils.hpp"
#include <algorithm>

#if _MSC_VER > 1000
#include <io.h>
#else
#include<sys/types.h>
#include <dirent.h>
#endif


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

string changeToInnerPath(const string & path)
{
    string p = path;
    replace(p.begin(),p.end(), '\\', '/');
    return p;
}

string ensureDir(const string & path)
{
    string p = changeToInnerPath(path);
    if(p.find_last_of('/') != p.length() - 1) {
        p.append("/");
    }
    return p;
}

string combinPath(const string & path, const string & path2)
{
    string r1 = ensureDir(path);
    string r2 = path2;
    replace(r2.begin(),r2.end(), '\\', '/');
    if(r2.at(0) == '/') {
        r2.erase(0, 1);
    }
    r1.append(r2);
    return r1;
}

void getFiles( const string & path, vector<string>& files )
{
     string pathInner = changeToInnerPath(path);
     if(pathInner.at(pathInner.length() - 1) == '/'){
         pathInner.erase(pathInner.length() - 1);
     }
#if _MSC_VER > 1000
    long   hFile   =   0;
    struct _finddata_t fileinfo;
    string p;
    if((hFile = _findfirst(p.assign(pathInner).append("/*").c_str(),&fileinfo)) !=  -1) {
        do {
            if((fileinfo.attrib &  _A_SUBDIR)) {
                if(strcmp(fileinfo.name,".") != 0  &&  strcmp(fileinfo.name,"..") != 0)
                    getFiles( p.assign(pathInner).append("/").append(fileinfo.name), files );
            } else {
                files.push_back(p.assign(pathInner).append("/").append(fileinfo.name) );
            }
        } while(_findnext(hFile, &fileinfo)  == 0);
        _findclose(hFile);
    } 
#else
    DIR * pDir = opendir(path.c_str());
    if(pDir == NULL) {
        return;
    }

    dirent * ent = NULL;
    while((ent = readdir(pDir)) != NULL){
        if(ent->d_type & DT_DIR){
            if(strcmp(ent->d_name,".")==0 || strcmp(ent->d_name,"..")==0) 
                continue;
            getFiles(pathInner+"/"+ent->d_name, files);
        }else{
            files.push_back(pathInner+"/"+ent->d_name);
        }
    }

#endif
}

size_t fileSize(const string& filename)
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
        if(inf.good()){
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
        }
        return NULL;
    }

    static size_t size(const string& filename)
    {
        return fileSize(filename);
    }

private:
    UTF8FileReader();
    UTF8FileReader(const UTF8FileReader &);
    UTF8FileReader & operator = (const UTF8FileReader &);
};

class LineFileReader
{
private:
    ifstream inf;
    string lastLine;
public:
    LineFileReader(const string & file)
    {
        inf.open(file.c_str());
    }
    ~LineFileReader()
    {
        inf.close();
    }

    string * getLine()
    {
        if(inf.good()){
            string line;
            do {
                if(getline(inf,line)) {
                    line = trim(line);
                    if(!line.empty()) {
                        lastLine = line;
                        return &lastLine;
                    }
                }
            } while(!inf.eof());
        }
        
        return NULL;
    }

    size_t size(const string& filename){
        return fileSize(filename);
    }

private:
    LineFileReader();
    LineFileReader(const UTF8FileReader &);
    LineFileReader & operator = (const UTF8FileReader &);
};
