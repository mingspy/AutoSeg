
#include<iostream>
#include<fstream>
#include <string>
#include <Windows.h>
#include "SparseInstance.hpp"
#include "Matrix.hpp"
#include "WordDictionary.hpp"
#include "DictFileBuilder.hpp"
#include "DATrie.hpp"
#include "MemLeaksCheck.h"
#include "Tokenizer.hpp"

using namespace std;
using namespace mingspy;

void testDAT(){
    DATrie trie(NULL);
    ifstream inf;
    inf.open("../words.txt"); //reading utf-8 file.
    string line;
    vector<wstring> words;
    int i = 0;
    while(getline(inf, line)){
        wstring word = Utf8ToUnicode(line);
        //wcout<<word<<endl;
        words.push_back(word);
        if(++i == 100) break;
    }
    inf.close();

    // test add
    DWORD start_time = GetTickCount();
    i = 0;
    for(vector<wstring>::iterator it = words.begin(); it != words.end(); it ++){
        i++;
        trie.add(it->c_str(),(void *)it->c_str());
        if(i % 1000 == 0){
            cout<<"\radded "<< i;    
        }
    }
    DWORD end_time = GetTickCount();
    cout<<"\nadd words "<<words.size()<< " used : "<<(end_time - start_time)<<" ms"<<endl;

    // testing write to file.
    trie.setDataWriter(WriteTrieStrToFile);
    start_time = GetTickCount();
    trie.writeToFile("e:/tmp/trie.dat");
    end_time = GetTickCount();
    cout<<"write datrie (words:"<<words.size()<< ") used : "<<(end_time - start_time)<<" ms"<<endl;

    // testing read from file.
    start_time = GetTickCount();
    DATrie trie2;
    trie2.setDataReader(ReadTrieStrFromFile);
    trie2.readFromFile("e:/tmp/trie.dat");
    end_time = GetTickCount();
    cout<<"read datrie (words:"<<words.size()<< ") used : "<<(end_time - start_time)<<" ms"<<endl;

    // test retrieve...
    int notfound = 0;
    TrieChar * p = NULL;
    start_time = GetTickCount();
    for(int j = 0; j < words.size(); j++){
        void * result = trie2.retrieve(words[j].c_str());
        if( result == NULL){
            notfound ++;
        }else {
            p = static_cast<TrieChar *>(result);
            if(p == NULL || words[j] != p){
                notfound ++;
            }else{
                if(j % 10000 == 0){
                    wcout << *p <<endl;
                }
            }
        }

    }
    end_time = GetTickCount();
    cout<<"notfound = "<<notfound<<"used :"<<(end_time - start_time)<<endl;
}

void testSparseInstance(){
    SparseInstance instance;
    instance.setValue(20, 20.01);
    instance.setValue(10,10.33);
    instance.setValue(30,30);
    instance.setValue(60,60);
    instance.setValue(60,600);
    instance.setValue(50,50);
    instance.setValue(1,1);
    cout<<"get(20)="<<instance.getValue(20)<<endl;
    cout<<"get(0)="<<instance.getValue(0)<<endl;
    cout<<"get(200)="<<instance.getValue(200)<<endl;
    instance.removeAttr(5);
    instance.removeAttr(30);
    cout<<instance<<endl;
    FILE * file = fopen("e:/tmp/instance.data","wb+");
    WriteInstanceDataToFile(file, &instance);
    fseek(file, 0, SEEK_SET);
    SparseInstance *i = (SparseInstance *)ReadInstanceDataFromFile(file, NULL);
    cout<<*i<<endl;
    delete i;
}

void testMatrix(){
    Matrix matrix;
    cout<<matrix.val(0,0)<<endl;
}

void testWordDictionary() 
{
    //DictFileBuilder::buildDict("..\\data\\coreWordInfo.txt","..\\data\\core.dic");


    DWORD start_time = GetTickCount();
    WordDictionary * dict = new WordDictionary("..\\data\\core.dic");
    DWORD end_time = GetTickCount();
    cout<<"load dictionary used"<<(end_time - start_time)<<endl;
    cout<<*dict->getWordInfo(L"咨询")<<endl;

    start_time = GetTickCount();
    delete dict;
    end_time = GetTickCount();
    cout<<"unload dictionary used"<<(end_time - start_time)<<endl;
}


int main(void){
    #if _MSC_VER > 1000
    wcout<<"windows!"<<_MSC_VER<<endl;
    #else
    cout<<"not windows, end."<<endl;
    return -1;
    #endif


    CheckMemLeaks();
    {
        //testSparseInstance();
        //testMatrix();
        //testWordDictionary();
        
        vector<Token> atoms;
        vector<wstring> words;
        wstring str = L"in 1998年，something important happened,中华人民共和国正式﹌成立了";
        Tokenizer tokenizer;
        tokenizer.maxSplit(str, atoms);
        tokenizer.output(str,atoms, words);
        atoms.clear();
        words.clear();
        tokenizer.fullSplit(str, atoms);
        tokenizer.output(str,atoms, words);
        
        //tokenizer.analysis(L"他说的确在理");
        //tokenizer.analysis(L"中华人民共和国正式成立了，在今天");   
    } 
    wcout<<L"Press enter to return."<<endl;
    getchar();
    return 0;
}