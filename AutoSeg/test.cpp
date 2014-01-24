
#include<iostream>
#include<fstream>
#include <string>
#include "DATrie.hpp"
#include <Windows.h>
using namespace mingspy;
using namespace std;

class Data{
public:
    string d;
    ~Data(){
        cout<<"hhh"<<endl;
    }
};

void free_Data(void * data){
    Data * p = (Data *)data;
    delete p;
}

/**
* In windows using MultibyeToWideChar. 
* In unix using iconv_open
*/
wstring Utf8ToUnicode( const string& str )
{
    int  len = 0;
    len = str.length();
    int  unicodeLen = ::MultiByteToWideChar( CP_UTF8,0,str.c_str(),-1,NULL,0); 
    wchar_t *  pUnicode; 
    pUnicode = new  wchar_t[unicodeLen+1]; 
    memset(pUnicode,0,(unicodeLen+1)*sizeof(wchar_t)); 
    ::MultiByteToWideChar( CP_UTF8,0,str.c_str(),-1,(LPWSTR)pUnicode,unicodeLen ); 
    wstring  rt; 
    rt = ( wchar_t* )pUnicode;
    delete  pUnicode;
    return  rt; 
}

int main(void){
    #if _MSC_VER > 1000
    cout<<"windows!"<<_MSC_VER<<endl;
    #else
    cout<<"not windows, end."<<endl;
    return -1;
    #endif

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
   
    return 0;
}