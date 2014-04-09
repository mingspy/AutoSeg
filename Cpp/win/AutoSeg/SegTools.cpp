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


void estimateTokenizer(const vector<wstring>& test_datas, int test_size,
                       const vector<vector<wstring> >& refer_datas,
                       ITokenizer & tokenizer, int choice)
{
  
    MSTimer timer;
    vector<vector<wstring> > seg_results;
    for(int i = 0; i < test_datas.size(); i++) {
        vector<Token> tokens;
        vector<wstring> words;
        switch(choice){
        case 1:
            tokenizer.maxSplit(test_datas[i], tokens);
            break;
        case 2:
            tokenizer.fullSplit(test_datas[i], tokens);
            break;
        case 3:
            tokenizer.uniGramSplit(test_datas[i], tokens);
            break;
        case 4:
            tokenizer.biGramSplit(test_datas[i], tokens);
            break;
        case 5:
            tokenizer.mixSplit(test_datas[i], tokens);
            break;
        }
        Tokenizer::output(test_datas[i], tokens, words);
        seg_results.push_back(words);
    }

    double elapsed = timer.elapsed();
    cout<<"segment "<<test_size<<" bytes data used "<<elapsed<<"s, speed "
        <<(test_size/1024.0/1024.0/elapsed)<<"m/s"
        <<endl;
    // estimates
    wstring punctuations = L"£ª£¬¡££¿£¨£©¡±¡°£«£«£«£­£­£­£­£®£¥¡¢£¯£½£¾¡À£¥¡Á¡Á¡Á¡ª¡ª¡ª¡ª¡ª¡ª¡ª£­¡®¡¯¡­¡­¡ë¡ù¡ú¡Î¡Ã¡Ù¢Ù¢Ú¢Û¢Ü¢Ý¢Þ¢ß¢à¢Æ¢Ç©¤¡õ¡ø¡÷¡ð¡ñ¡ï¡¢¡£¡±¡´¡µ¡¶¡·¡º¡»¡²¡³";
    int refer_words = 0;
    int total_correct = 0;
    int total_segmented = 0;
    for(int i = 0; i < seg_results.size(); i++) {
        refer_words += refer_datas[i].size();
        total_segmented += seg_results[i].size();
        for(int j = 0; j < refer_datas[i].size(); j++) {
            if(punctuations.find(refer_datas[i][j]) != wstring::npos) {
                refer_words --;
            }
        }
        for(int j = 0; j < seg_results[i].size(); j++) {
            if(punctuations.find(seg_results[i][j]) != wstring::npos) {
                total_segmented --;
                continue;
            }

            int m = max(0, j - 4);
            int n = min(j+4, refer_datas[i].size());
            for(int k = m; k < n; k++) {
                if(seg_results[i][j] == refer_datas[i][k]) {
                    total_correct ++;
                    break;
                }
            }
        }
    }

    cout<<"refer words:"<< refer_words<<" segmented :"<<total_segmented
        <<" corrected:"<<total_correct<<endl;
    double precision = (total_correct + 0.01)/(total_segmented + 0.01);
    double recall = (total_correct + 0.01)/(refer_words + 0.01);
    double f2 = precision * recall * 2 / (precision + recall);
    cout<<"\tprecision = "<<precision<<endl;
    cout<<"\trecall = "<<recall<<endl;
    cout<<"\tf2 = "<<f2<<endl;
}

void estimateSegmetors()
{
    MSTimer timer;
    
    //DictFileBuilder::buildInverseDict("../data/estimate/coreWordInfo.txt","../data/estimate/inverseCore.dic");
    DictFactory::initialize();
    cout<<"load dict "<<timer<<endl;
    timer.restart();
    // load test data.
    vector<wstring> test_datas;
    size_t test_size = UTF8FileReader::size("../data/estimate/test_data.txt");
    {
        UTF8FileReader testDataReader("../data/estimate/test_data.txt");
        wstring * line;
        while((line = testDataReader.getLine()) != NULL) {
            test_datas.push_back(*line);
        }
    }

    cout<<"load test data "<<timer<<endl;
    timer.restart();
    // load refer data.
    vector<vector<wstring> > refer_datas;
    {
        wstring * line;
        UTF8FileReader referDataReader("../data/estimate/test_refer.txt");
        while((line = referDataReader.getLine()) != NULL) {
            vector<wstring> words;
            refer_datas.push_back(words);
            split(*line, L"  ", words);
            vector<wstring> & refer = refer_datas[refer_datas.size() - 1];
            for(int i = 0; i < words.size(); i++) {
                int idx = words[i].find_first_of(L'/');
                wstring word = words[i].substr(0,idx);
                if(word[0] == L'[') {
                    word = word.substr(1);
                }

                refer.push_back(word);
            }
        }
    }
    cout<<"load test refer data "<<timer<<endl;

    Tokenizer tokenizer;
    AutoTokenizer autoSeg;
    int choice = 0;
    string input;
    while(true){
        cout<<"Input the number ahead to run test:"<<endl
            <<"1. maxSplit"<<endl
            <<"2. fullSplit"<<endl
            <<"3. oneGramSplit"<<endl
            <<"4. biGramSplit"<<endl
            <<"5. mixSplit"<<endl
            <<"0. quit."<<endl
            <<">";
        
        cin>>input;
        choice = atoi(input.c_str());
        cin.clear();
        if(choice <= 0 || choice > 5){
            cout<<"quit.";
            break;
        }
        
        cout<<endl
            <<"estimate forward tokenizer."<<endl;
        estimateTokenizer(test_datas,test_size,refer_datas, tokenizer,choice);

        cout<<endl
            <<"estimate AutoSegmentor."<<endl;
        estimateTokenizer(test_datas,test_size,refer_datas, autoSeg,choice);
    }
    

}

void printHelp(const char * name){
    cout<<"Welcome use"<<name<<endl
        <<"to build core.dict using:"<<endl
        <<"\t -b input output "<<endl
        <<"\t -b    :default input is ../data/words/ output is ../data/core.dic"<<endl
        <<"to test split performance using:"<<endl
        <<"\t -t   :default core.dic should place in ../data/"<<endl;
}

void buildCoreDict(int argc, char ** argv){
    string input = "../data/words/core/";
    string output = "../data/core.dic";
    if(argc == 4){
        input = argv[2];
        output = argv[3];
    }
    vector<string> files;
    getFiles(input, files);
    // input is a file.
    if(files.size() == 0){
        files.push_back(input);
    }
    DictFileBuilder::buildDict(files, output);
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
        if(argc < 2){
            printHelp(argv[0]);
            return -1;
        }
        string arg1 = argv[1];
        if(arg1 == "-t"){
            estimateSegmetors();
        }else if(arg1 == "-b"){
            buildCoreDict(argc, argv);
        }

    }
    wcout<<L"Press enter to return."<<endl;
    getchar();
    return 0;
}
