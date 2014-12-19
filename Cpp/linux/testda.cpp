#include "datrie.hpp"
#include <iostream>
#include <string>

using namespace mingspy;
using namespace std;

int main(){

    string str = "中国人";
    DoubleArrayImpl<char, unsigned char, int, unsigned int> da;

    int s = da.getRoot();
    const char * p = str.c_str();
    for(;*p;p++){
        if (da.walk(&s,*p)){
            cout<<"can walk"<<endl;
        }else{
            s = da.insertBranch(s, *p);
            cout<<"insert"<<endl;
        }
    }


    cout<<"checking"<<str<<endl;
    s = da.getRoot();
    p = str.c_str();
    for(;*p;p++){
        if (da.walk(&s,*p)){
            cout<<"can walk"<<endl;
        }else{
            s = da.insertBranch(s, *p);
            cout<<"insert"<<endl;
        }
    }
    if (!*p){
        cout<<"finished"<<endl;
    }

    cout<<"insert str2"<<endl;
    wstring str2 = L"中国的";
    s = da.getRoot();
    p = (const char *)str2.c_str();
    cout<<"sizeof wchar_t"<<sizeof(wchar_t)<<" "<<str2.length()<<endl;
    for(int i = 0; i < str2.length() * sizeof(wchar_t);i++){
        cout<<0+p[i]<<" ";
    }
    cout<<endl;

    for(;*p;p++){
        if (da.walk(&s,*p)){
            cout<<"can walk"<<endl;
        }else{
            s = da.insertBranch(s, *p);
            cout<<"insert"<<endl;
        }
    }

    cout<<"checking str2"<<endl;
    s = da.getRoot();
    p = (const char *)(str2.c_str());
    for(;*p;p++){
        if (da.walk(&s,*p)){
            cout<<"can walk"<<endl;
        }else{
            s = da.insertBranch(s, *p);
            cout<<"insert"<<endl;
        }
    }
    if (!*p){
        cout<<"finished"<<endl;
    }
}
