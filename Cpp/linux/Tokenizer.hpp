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
#include <string>
#include "DictFactory.hpp"
#include "wcharHelper.hpp"
#include "Matrix.hpp"
#include "NShortPath.hpp"
using namespace std;

namespace mingspy
{

struct Token {
    int _attr; // type of this token or attribute index.
    int _off; // start index.
    int _len; // len
    //wstring _word; // maybe empty
    Token(int start = 0, int len = 0, int attr = 0)
        :_off(start),_len(len),_attr(attr)
    {
    }
};

static const double TOTAL_FREQ = 2700000;
static const double SMOTH_PROB = 1.0/TOTAL_FREQ;

class ITokenizer
{
public:
    /*
    * according dictionary, forward split the str. From the head char
    * of input, this method will lookup the dictionary, then take the
    * longest word from the str step by step.
    * For example: "AB" "ABCD" "BCD" "BCDE" "EF" are
    * words in dictionary; when given str="ABCDEF", the split result
    * will be: ABCD/ EF/
    * @param str : the input str to split.
    * @param result : result words
    */
    virtual void maxSplit(const wstring &str, vector<Token> & result) = 0;

    /*
    * split out all possible words in the given str.
    * @param str : the input str to split.
    * @param result : result words.
    */
    virtual void fullSplit(const wstring &str, vector<Token> & result) = 0;

    /*
    * Split the word, and try to resolve ambiguities by one gram.
    * @param str : the input str to split.
    * @param result : result words.
    */
    virtual void oneGramSplit(const wstring & str, vector<Token> & result) = 0;

    static void printTokens(const vector<Token> & result)
    {
        for(int i = 0; i< result.size(); i++) {
            wcout<<"("<<result[i]._off<<","<<result[i]._len<<")";
        }
        wcout<<endl;
    }

    static void output(const wstring & str, const vector<Token> & result, vector<wstring> & words)
    {
        for(int i = 0; i< result.size(); i++) {
            words.push_back(str.substr(result[i]._off, result[i]._len));
        }
    }
};

class Tokenizer: public ITokenizer
{
public:
    Tokenizer()
    {
        m_maxPaths = 8;
    }

    /*
    * according dictionary, forward split the str. From the head char
    * of input, this method will lookup the dictionary, then take the
    * longest word from the str step by step.
    * For example: "AB" "ABCD" "BCD" "BCDE" "EF" are
    * words in dictionary; when given str="ABCDEF", the split result
    * will be: ABCD/ EF/
    * @param str : the input str to split.
    * @param result : result words
    */
    virtual void maxSplit(const wstring &str, vector<Token> & result)
    {
        vector<Token> atoms;
        atomSplit(str,atoms);
        maxMatch(DictFactory::CoreDict(),str, atoms, result);
    }

    /*
    * split out all possible words in the given str.
    * @param str : the input str to split.
    * @param result : result words.
    */
    virtual void fullSplit(const wstring &str, vector<Token> & result)
    {
        vector<Token> atoms;
        atomSplit(str,atoms);
        fullMatch(DictFactory::CoreDict(),str,atoms,result);
    }

    /*
    * Split the word, and try to resolve ambiguities by one gram.
    * @param str : the input str to split.
    * @param result : result words.
    */
    void oneGramSplit(const wstring & str, vector<Token> & result)
    {
        Graph wordGraph;
        // get all words
        genWordGraph(DictFactory::CoreDict(), str, wordGraph);

        // n-short path
        NShortPath shortPath(wordGraph, m_maxPaths, str.length());
        shortPath.calcPaths();

        Path p;
        if(!shortPath.getBestPath(0, p)) return;
        for(int i = 0; i < p.numValues(); i ++) {
            int start = p.indexAt(i);
            int len = p.valueAt(i) - start;
            result.push_back(Token(start, len));
        }
    }

    void setMaxPaths(int maxs)
    {
        if(maxs >= 1 && maxs < 1000) {
            m_maxPaths = maxs;
        }
    }
protected:

    /*
    * try to find all the possible words.
    * assume str = "ABCDE";
    * and AB ABC BCD E in the word dictionary;
    * the result will be :AB, ABC,BCD,D,E
    */
    void fullMatch(const Dictionary & dict,
                   const wstring &str, const vector<Token> & atoms, vector<Token> &result)
    {
        const int atome_size = atoms.size();
        int lastj = -1;
        for(int i = 0; i < atome_size; i++) {
            for(int j = i + 1; j < atome_size; j++) {
                wstring word = str.substr(atoms[i]._off, atoms[j]._off + atoms[j]._len - atoms[i]._off);
                if(dict.getWordInfo(word)) {
                    result.push_back(Token(atoms[i]._off, atoms[j]._off + atoms[j]._len - atoms[i]._off));
                    lastj = j;
                } else if(!dict.existPrefix(word)) {
                    break;
                }
            }
            if(i > lastj) {
                result.push_back(Token(atoms[i]._off, atoms[i]._len));
            }

        }

    }

    /*
    * try to split the str with huge words.
    * assume str = "ABCDE";
    * and AB ABC BCD E in the word dictionary;
    * the result will be : ABC,D,E
    * @param dict: the dictionary to lookup.
    * @param str : the input string.
    * @param atoms: the atom split result of str
    * @param result: save the split words.
    */
    void maxMatch(const Dictionary & dict,
                  const wstring &str, const vector<Token> & atoms,
                  vector<Token> &result)
    {
        for(int i = 0; i < atoms.size(); ) {
            int j = i + 1;
            for(; j < atoms.size(); j++) {
                wstring word = str.substr(atoms[i]._off, atoms[j]._off + atoms[j]._len - atoms[i]._off);
                if(!dict.existPrefix(word)) {
                    break;
                }
            }

            result.push_back(Token(atoms[i]._off,atoms[j-1]._off + atoms[j-1]._len - atoms[i]._off));
            i = j;
        }
    }

    /*
    * Split atoms from given str:
    * Atoms are in form of:
    *   A Chinese char
    *   An English str
    *   A number
    * @param str: the input string to split
    * @param atoms: save the split result
    */
    void atomSplit(const wstring & str, vector<Token> & atoms)
    {
        int len = str.length();
        for(int i = 0; i < len; ) {
            // merge number and alpha
            wtype itype = getWcharType(str.at(i));
            if(itype == DIGIT || itype == ALPHA) {
                int j = i+1;
                while(j <len && (getWcharType(str.at(j)) == itype)) {
                    j++;
                }
                atoms.push_back(Token( i, j - i, (int)itype));
                i = j;
            } else {
                atoms.push_back(Token(i++, 1, (int)itype));
            }
        }
    }

    /*
    * According documents of ICTCLAS, generate all possible split path and save
    * in to a word graph.
    * In the graph, each word is a vertex, words are connected by edge.
    * An edge from word1 to word2 exists only if:
    *     word1._off + word1._len = word2._off;
    * In my implement, i use a sparse matrix to perform the graph,
    * suppose we have a dictionary has words:
    *    A B C D E F G AB AC ABC CD EF BC ...
    * Given input string: "ABCDE"
    * all possible split path will be:
    *     A->B->C->D->E
    *     A->BC->D->E
    *     A->B->CD->E
    *     AB->C->D->E
    *     AB->CD->E
    *     ABC->D->E
    * save these path in a matrix will be as follows:
    *      1     2     3     4     5
    * -------------------------------
    * 1|   A     AB    ABC
    * 2|         B     BC
    * 3|               C     CD
    * 4|                     D
    * 5|                           E
    * In the WordGraph, row is the word's start index, column is the end index.
    */
    void genWordGraph(const Dictionary & dict,const wstring &str, Graph & graph)
    {
        vector<Token> atoms;
        atomSplit(str, atoms);
        const int atome_size = atoms.size();
        int lastj = -1;
        const WordNature * info = NULL;
        for(int i = 0; i < atome_size; i++) {
            for(int j = i; j < atome_size; j++) {
                wstring word = str.substr(atoms[i]._off, atoms[j]._off + atoms[j]._len - atoms[i]._off);
                if((info = dict.getWordInfo(word))) {
                    double totalfrequnce = info->getSumValue() + 1.0;
                    graph.setVal(atoms[i]._off, atoms[j]._off + atoms[j]._len, totalfrequnce/TOTAL_FREQ);
                    lastj = j;
                } else if(!dict.existPrefix(word)) {
                    break;
                }
            }

            // add word that not exist in dictionary.
            if(i > lastj) {
                graph.setVal(atoms[i]._off, atoms[i]._off + atoms[i]._len, SMOTH_PROB);
            }

        }
    }

private:
    int m_maxPaths;
};
}
