#pragma once

#include <iostream>
#include "Matrix.hpp"
#include "SparseInstance.hpp"
#include <math.h>
#include "MinHeap.hpp"
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

    bool operator < (const RoadSign & r) const{
        return _weight < r._weight;
    }

    bool operator > (const RoadSign & r) const{
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
    return deta == 0? 0:(deta > 0?1:-1);  //��С��������
}

typedef SparseInstance<int> Path;
typedef MinHeap<RoadSign> Signs;

class Paths{
public:   
    Signs& operator[](int idx){
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
            // �ڵ��ܵ����·��������·��ֵ
            SparseInstance<double> &ins = m_graphRef[i];
            for(int j = ins.numValues() - 1; j >= 0; j--) {
                int to = ins.indexAt(j);
                double weight = -log(ins.valueAt(j));
                Signs & prevPaths = m_paths[i];
                for( int k = 0; k < prevPaths.size(); k++) {
                    RoadSign & node = prevPaths.Get(k);
                    m_paths[to].ConditionAdd(RoadSign(i,k, weight + node._weight));
                }
            }
        }
        m_paths[m_lastIndex].Sort();
        m_isCalced = true;

    }

    bool getBestPath(int ith,Path & path)
    {
        if(m_isCalced && ith < m_maxPaths) {
            int to = m_lastIndex;
            while(true) {
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

