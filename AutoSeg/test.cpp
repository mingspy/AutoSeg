
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "SparseInstance.hpp"
#include "Matrix.hpp"
#include "WordDictionary.hpp"
#include "DictFileBuilder.hpp"
#include "DATrie.hpp"
#include "MemLeaksCheck.h"
#include "Tokenizer.hpp"
#include "MSTimer.hpp"
#include "InverseTokenizer.hpp"

using namespace std;
using namespace mingspy;

void testDAT()
{
    DATrie trie(NULL);
    ifstream inf;
    inf.open("../words.txt"); //reading utf-8 file.
    string line;
    vector<wstring> words;
    int i = 0;
    while(getline(inf, line))
    {
        wstring word = Utf8ToUnicode(line);
        //wcout<<word<<endl;
        words.push_back(word);
        if(++i == 100) break;
    }
    inf.close();

    // test add
    MSTimer timer;
    i = 0;
    for(vector<wstring>::iterator it = words.begin(); it != words.end(); it ++)
    {
        i++;
        trie.add(it->c_str(),(void *)it->c_str());
        if(i % 1000 == 0)
        {
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
    TrieChar * p = NULL;
    timer.restart();
    for(int j = 0; j < words.size(); j++)
    {
        void * result = trie2.retrieve(words[j].c_str());
        if( result == NULL)
        {
            notfound ++;
        }
        else
        {
            p = static_cast<TrieChar *>(result);
            if(p == NULL || words[j] != p)
            {
                notfound ++;
            }
            else
            {
                if(j % 10000 == 0)
                {
                    wcout << *p <<endl;
                }
            }
        }

    }

    cout<<"notfound = "<<notfound<<timer<<endl;
}

void testSparseInstance()
{
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

void testMatrix()
{
    Matrix matrix;
    cout<<matrix.val(0,0)<<endl;
}

void testWordDictionary()
{
    //DictFileBuilder::buildDict("..\\data\\coreWordInfo.txt","..\\data\\core.dic");

    MSTimer timer;
    WordDictionary * dict = new WordDictionary("..\\data\\core.dic");
    cout<<"load dictionary"<<timer<<endl;
    cout<<*dict->getWordInfo(L"咨询")<<endl;

    timer.restart();
    delete dict;
    cout<<"unload dictionary"<<timer<<endl;
}

void testSegment()
{
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

void estimateTokenizer(const vector<wstring>& test_datas, int test_size,
                       const vector<vector<wstring>>& refer_datas,
                       Tokenizer & tokenizer)
{
    wstring punctuations = L"＊，。？（）”“＋＋＋－－－－．％、／＝＞±％×××———————－‘’……‰\
※→∥∶≠①②③④⑤⑥⑦⑧⑵⑶─□▲△○●★、。”〈〉《》『』〔〕";
    MSTimer timer;
    vector<vector<wstring>> seg_results;
    for(int i = 0; i < test_datas.size(); i++)
    {
        vector<Token> tokens;
        vector<wstring> words;
        tokenizer.maxSplit(test_datas[i], tokens);
        tokenizer.output(test_datas[i],tokens, words);
        seg_results.push_back(words);
    }

    long elapsed = timer.elapsed();
    cout<<"segment "<<test_size<<" bytes data used "<<elapsed<<"ms, speed "
        <<(test_size/1024.0/1024.0/((elapsed+0.0000001)/1000.0))<<"m/s"
        <<endl;
    // estimates

    int refer_words = 0;
    int total_correct = 0;
    int total_segmented = 0;
    for(int i = 0; i < seg_results.size(); i++)
    {
        refer_words += refer_datas[i].size();
        total_segmented += seg_results[i].size();
        for(int j = 0; j < refer_datas[i].size(); j++)
        {
            if(punctuations.find(refer_datas[i][j]) != wstring::npos)
            {
                refer_words --;
            }
        }
        for(int j = 0; j < seg_results[i].size(); j++)
        {
            if(punctuations.find(seg_results[i][j]) != wstring::npos)
            {
                total_segmented --;
                continue;
            }

            int m = max(0, j - 4);
            int n = min(j+4, refer_datas[i].size());
            for(int k = m; k < n; k++)
            {
                if(seg_results[i][j] == refer_datas[i][k])
                {
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
    //DictFileBuilder::buildDict("../data/estimate/coreWordInfo.txt","../data/estimate/core.dic");
    //DictFileBuilder::buildInverseDict("../data/estimate/coreWordInfo.txt","../data/estimate/inverseCore.dic");
    DictFactory::initialize("../data/estimate/");
    cout<<"load dict "<<timer<<endl;
    timer.restart();
    // load test data.
    vector<wstring> test_datas;
    size_t test_size = UTF8FileReader::size("../data/estimate/test_data.txt");
    {
        UTF8FileReader testDataReader("../data/estimate/test_data.txt");
        wstring * line;
        while((line = testDataReader.getLine()) != NULL)
        {
            test_datas.push_back(*line);
        }
    }

    cout<<"load test data "<<timer<<endl;
    timer.restart();
    // load refer data.
    vector<vector<wstring>> refer_datas;
    {
        wstring * line;
        UTF8FileReader referDataReader("../data/estimate/test_refer.txt");
        while((line = referDataReader.getLine()) != NULL)
        {
            vector<wstring> words;
            refer_datas.push_back(words);
            split(*line, L"  ", words);
            vector<wstring> & refer = refer_datas[refer_datas.size() - 1];
            for(int i = 0; i < words.size(); i++)
            {
                int idx = words[i].find_first_of(L'/');
                wstring word = words[i].substr(0,idx);
                if(word[0] == L'[')
                {
                    word = word.substr(1);
                }

                refer.push_back(word);
            }
        }
    }
    cout<<"load test refer data "<<timer<<endl;

    cout<<endl
        <<"estimate forward tokenizer."<<endl;
    estimateTokenizer(test_datas,test_size,refer_datas, Tokenizer());

    cout<<endl
        <<"estimate inverse tokenizer."<<endl;
    estimateTokenizer(test_datas,test_size,refer_datas, InverseTokenizer());

}

int main(void)
{
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

        estimateSegmetors();
    }
    wcout<<L"Press enter to return."<<endl;
    getchar();
    return 0;
}
