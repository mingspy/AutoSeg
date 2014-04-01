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
    DictFileBuilder::buildDict("..\\data\\coreWordInfo.txt","..\\data\\core.dic");

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

    tokenizer.oneGramSplit(L"他说的确实在理", result);
    Tokenizer::printTokens(result);
    result.clear();
    tokenizer.maxSplit(L"他说的确实在理", result);
    Tokenizer::printTokens(result);
    result.clear();
    tokenizer.fullSplit(L"他说的确实在理", result);
    Tokenizer::printTokens(result);
}

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
            tokenizer.oneGramSplit(test_datas[i], tokens);
            break;
        }
        Tokenizer::output(test_datas[i], tokens, words);
        seg_results.push_back(words);
    }

    double elapsed = timer.elapsed();
    cout<<"segment "<<test_size<<" bytes data used "<<elapsed<<"ms, speed "
        <<(test_size/1024.0/1024.0/elapsed)<<"m/s"
        <<endl;
    // estimates
    wstring punctuations = L"＊，。？（）”“＋＋＋－－－－．％、／＝＞±％×××———————－‘’……‰※→∥∶≠①②③④⑤⑥⑦⑧⑵⑶─□▲△○●★、。”〈〉《》『』〔〕";
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
    DictFactory::initialize("../data/");
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
            <<"0. quit."<<endl
            <<">";
        
        cin>>input;
        choice = atoi(input.c_str());
        cin.clear();
        if(choice <= 0 || choice > 3){
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
    string input = "../data/words/";
    string output = "../data/core.dic";
    if(argc == 4){
        input = argv[2];
        output = argv[3];
    }
    DictFileBuilder::buildDict(input, output);
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
        estimateSegmetors();
        if(argc < 2){
            printHelp(argv[0]);
            return -1;
        }
        
        if(argv[1] == "-t"){
            estimateSegmetors();
        }else if(argv[1] == "-b"){
            buildCoreDict(argc, argv);
        }

    }
    wcout<<L"Press enter to return."<<endl;
    getchar();
    return 0;
}
