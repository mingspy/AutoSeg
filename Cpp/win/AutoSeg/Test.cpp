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
#include <fstream>
#include <string>
#include <vector>
#include "SparseInstance.hpp"
#include "Matrix.hpp"
#include "Dictionary.hpp"
#include "DictFileBuilder.hpp"
#include "DATrie.hpp"
#include "MemLeaksCheck.h"
#include "Tokenizer.hpp"
#include "MSTimer.hpp"
#include "InverseTokenizer.hpp"
#include "AutoTokenizer.hpp"

using namespace std;
using namespace mingspy;

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

void testDAT()
{
    DATrie trie(NULL);
    ifstream inf;
    inf.open("../words.txt"); //reading utf-8 file.
    string line;
    vector<wstring> words;
    int i = 0;
    while(getline(inf, line)) {
        wstring word = Utf8ToUnicode(line);
        //wcout<<word<<endl;
        words.push_back(word);
        if(++i == 100) break;
    }
    inf.close();

    // test add
    MSTimer timer;
    i = 0;
    for(vector<wstring>::iterator it = words.begin(); it != words.end(); it ++) {
        i++;
        trie.add(it->c_str(),(void *)it->c_str());
        if(i % 1000 == 0) {
            cout<<"\radded "<< i;
        }
    }
    cout<<"\nadd words "<<words.size()<< timer<<endl;

    timer.restart();
    // testing write to file.
    trie.setDataWriter(WriteTrieStrToFile);
    trie.writeToFile("e:/tmp/trie.dat");
    cout<<"write datrie (words:"<<words.size()<< ") "<<timer<<" ms"<<endl;

    // testing read from file.
    timer.restart();
    DATrie trie2;
    trie2.setDataReader(ReadTrieStrFromFile);
    trie2.readFromFile("e:/tmp/trie.dat");
    cout<<"read datrie (words:"<<words.size()<< ") "<<timer<<endl;

    // test retrieve...
    int notfound = 0;
    wchar_t * p = NULL;
    timer.restart();
    for(int j = 0; j < words.size(); j++) {
        void * result = trie2.retrieve(words[j].c_str());
        if( result == NULL) {
            notfound ++;
        } else {
            p = static_cast<wchar_t *>(result);
            if(p == NULL || words[j] != p) {
                notfound ++;
            } else {
                if(j % 10000 == 0) {
                    wcout << *p <<endl;
                }
            }
        }

    }

    cout<<"notfound = "<<notfound<<timer<<endl;
    assert(notfound == 0);
}

void testSparseInstance()
{
    SparseInstance<double> instance;
    instance.setAttrValue(20, 20.01);
    instance.setAttrValue(10,10.33);
    instance.setAttrValue(30,30);
    instance.setAttrValue(60,60);
    instance.setAttrValue(60,600);
    instance.setAttrValue(50,50);
    instance.setAttrValue(1,1);
    cout<<"get(20)="<<instance.getAttrValue(20)<<endl;
    cout<<"get(0)="<<instance.getAttrValue(0)<<endl;
    cout<<"get(200)="<<instance.getAttrValue(200)<<endl;
    instance.removeAttr(5);
    instance.removeAttr(30);
    cout<<instance<<endl;
    FILE * file = fopen("e:/tmp/instance.data","wb+");
    SparseInstance<double>::DoWriteSIToFile(file, &instance);
    fseek(file, 0, SEEK_SET);
    SparseInstance<double> *i = SparseInstance<double>::DoReadSIFromFile(file, NULL);
    cout<<*i<<endl;
    assert(instance.getAttrValue(20) == i->getAttrValue(20));
    assert(instance.getAttrValue(200) == i->getAttrValue(200));
    assert(instance.getAttrValue(0) == i->getAttrValue(0));

    delete i;
}

void testMatrix()
{
    Matrix<int> matrix;
    cout<<matrix.val(0,0)<<endl;
    assert(matrix.val(3,3) == 0);
}

void testWordDictionary()
{
    //DictFileBuilder::buildDict("..\\data\\coreWordInfo.txt","..\\data\\core.dic");

    MSTimer timer;
    Dictionary * dict = new Dictionary("..\\data\\core.dic");
    cout<<"load dictionary"<<timer<<endl;
    cout<<*dict->getWordInfo(L"咨询")<<endl;

    timer.restart();
    delete dict;
    cout<<"unload dictionary"<<timer<<endl;
}

void testSegment()
{
    vector<Token> atoms;
    wstring str = L"in 1998年，something important happened,中华人民共和国正式﹌成立了";
    Tokenizer tokenizer;
    tokenizer.maxSplit(str, atoms);
    Tokenizer::printTokens(atoms);
    atoms.clear();
    tokenizer.fullSplit(str, atoms);
    Tokenizer::printTokens(atoms);
    vector<Token> result;

    tokenizer.uniGramSplit(L"他说的确实在理", result);
    Tokenizer::printTokens(result);
    result.clear();
    tokenizer.maxSplit(L"他说的确实在理", result);
    Tokenizer::printTokens(result);
    result.clear();
    tokenizer.fullSplit(L"他说的确实在理", result);
    Tokenizer::printTokens(result);
}


void printHelp(const char * name){
    cout<<"Welcome use"<<name<<endl
        <<"to build core.dict using:"<<endl
        <<"\t -b input output "<<endl
        <<"\t -b    :default input is ../data/words/ output is ../data/core.dic"<<endl
        <<"to test split performance using:"<<endl
        <<"\t -t   :default core.dic should place in ../data/"<<endl;
}


int main(int argc, char ** argv)
{
#if _MSC_VER > 1000
    wcout<<"runing on windows!"<<_MSC_VER<<endl;
#else
    cout<<"not runing on windows!"<<endl;
#endif
   
    //CheckMemLeaks();
    {
        cout<<"sizeof(wchar_t):"<<sizeof(wchar_t)<<endl;
    }
    wcout<<L"Press enter to return."<<endl;
    getchar();
    return 0;
}
