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
#include "Matrix.hpp"
#include "SparseInstance.hpp"
#include <math.h>
#include "MinHeap.hpp"
#include "Token.hpp"
using namespace std;


namespace mingspy
{
/*
   * in segmentor application, only index [i - 1] can reach index [i];
   * so we know all prev vertex that can reach current one, we need record
   * the prev word's len and index in prev path, then we can go back to all
   * the path.
   */
struct RoadSign {
    int _from, _index;
    double _weight;
    RoadSign():_from(-1), _index(0), _weight(0)
    {
    }
    RoadSign(int from, int index, double weight)
        :_from(from), _index(index), _weight(weight)
    {
    }

    bool operator != (double i) const
    {
        return _weight != i;
    }

    bool operator < (const RoadSign & r) const
    {
        return _weight < r._weight;
    }

    bool operator > (const RoadSign & r) const
    {
        return _weight > r._weight;
    }

    friend ostream & operator<< (ostream & out, const RoadSign & node)
    {
        out<<"("<<node._from<<","<<node._index<<","<<node._weight<<")";
        return out;
    }
};

int compare(const void *a, const void *b)
{
    RoadSign *pa = (RoadSign*)a;
    RoadSign *pb = (RoadSign*)b;
    double deta = pa->_weight- pb->_weight;
    return deta == 0? 0:(deta > 0?1:-1);  //sort ASC
}

class Path:public SparseInstance<int>
{
public:
    void setScore(double score)
    {
        _score = score;
    }

    double getScore()
    {
        return _score;
    }
private:
    double _score;
};

typedef MinHeap<RoadSign> Signs;

class Paths
{
public:
    Signs& operator[](int idx)
    {
        return _paths[idx];
    }

private:
    hash_map<int,Signs> _paths;
};

class NShortPath
{
private:
    Graph & m_graphRef;
    int   m_maxPaths;
    int m_lastIndex;
    //typedef Matrix<RoadSign> Paths;

    Paths m_paths;
    bool m_isCalced;
public:
    NShortPath(Graph & g, int n, int strlen):m_graphRef(g), m_maxPaths(n),
        m_lastIndex(strlen),m_isCalced(false)
    {
    }

    void calcPaths()
    {
        m_paths[0].ConditionAdd(RoadSign(-1,0, 1.0));
        for(int i = 0; i < m_lastIndex; i++) {
            // 节点能到达的路径，更新路径值
            SparseInstance<double> &ins = m_graphRef[i];
            // 取出一个节点
            for(int j = ins.numValues() - 1; j >= 0; j--) {
                // 该节点能到达的路径
                int to = ins.attrAt(j);
                double weight = -log(ins.valueAt(j));
                // 能到达当前路径的所有节点的累加值
                Signs & prevPaths = m_paths[i];
                for( int k = 0; k < prevPaths.size(); k++) {
                    RoadSign & node = prevPaths.Get(k);
                    // 更新路径
                    m_paths[to].ConditionAdd(RoadSign(i,k, weight + node._weight));
                }
            }
        }
        m_paths[m_lastIndex].Sort();
        m_isCalced = true;

    }

    bool getBestResult(int ith, vector<Token> & result, double *score = NULL)
    {
        Path p;
        if(!getBestPath(ith, p)) return false;
        if(score) {
            *score = p.getScore();
        }
        for(int i = 0; i < p.numValues(); i ++) {
            int start = p.attrAt(i);
            int len = p.valueAt(i) - start;
            result.push_back(Token(start, len));
        }
        return true;
    }

private:
    bool getBestPath(int ith,Path & path)
    {
        if(m_isCalced && ith < m_maxPaths) {
            int to = m_lastIndex;
            if(ith >= m_paths[to].size()) return false;

            path.setScore(m_paths[to].Get(ith)._weight);
            for(int i = 0; i <= m_lastIndex; i++) {
                RoadSign &node = m_paths[to].Get(ith);
                path.setAttrValue(node._from,to);
                to = node._from;
                ith = node._index;
                if(to == 0) break;
            }
            return true;
        }
        return false;
    }

private:
    NShortPath(const NShortPath &);
};
}

