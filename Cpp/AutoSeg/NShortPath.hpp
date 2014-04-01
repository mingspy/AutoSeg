#pragma once

#include <iostream>
#include "Matrix.hpp"
#include "SparseInstance.hpp"
#include <math.h>
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
    return deta == 0? 0:(deta > 0?1:-1);  //从小到大排序
}

typedef SparseInstance<int> Path;

class NShortPath
{
private:
    Graph & m_graphRef;
    int   m_maxPaths;
    int m_lastIndex;
    typedef Matrix<RoadSign> Paths;

    Paths m_paths;
    bool m_isCalced;
public:
    NShortPath(Graph & g, int n, int strlen):m_graphRef(g), m_maxPaths(n),
        m_lastIndex(strlen),m_isCalced(false)
    {
    }

    void calcPaths()
    {
        m_paths[0].setAttrValue(0,RoadSign(-1,0, 1.0));
        for(int i = 0; i < m_lastIndex; i++) {
            // 节点能到达的路径，更新路径值
            SparseInstance<double> &ins = m_graphRef[i];
            for(int j = ins.numValues() - 1; j >= 0; j--) {
                int to = ins.indexAt(j);
                double weight = -log(ins.valueAt(j));
                SparseInstance<RoadSign> & prevPaths = m_paths.getRow(i);
                for( int k = 0; k < prevPaths.numValues(); k++) {
                    RoadSign & node = prevPaths.valueAt(k);
                    setPath(i, to, k, weight + node._weight);
                }
            }
        }

        qsort(m_paths[m_lastIndex].getAttrValues(),m_paths[m_lastIndex].numValues(), sizeof(RoadSign), compare);
        m_isCalced = true;

    }

    bool getBestPath(int ith,Path & path)
    {
        if(m_isCalced && ith < m_maxPaths) {
            int to = m_lastIndex;
            while(true) {
                RoadSign node = m_paths[to].valueAt(ith);
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
    void setPath(int from, int to, int index, int weight)
    {
        if(m_paths[to].numValues() < m_maxPaths) {
            m_paths[to].setAttrValue(m_paths[to].numValues(), RoadSign(from,index, weight));
        }
    }

};
}

