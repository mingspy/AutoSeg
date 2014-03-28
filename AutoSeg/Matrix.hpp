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
#include <list>
#include "Attribute.hpp"
#include "SparseInstance.hpp"
#include <vector>
#include "HashMapDef.hpp"

using namespace std;
using namespace stdext;

namespace mingspy
{

/*
*/
class Matrix
{
protected:
    //vector<Attribute> _attributes;
    hash_map<int, SparseInstance> _rows;
public:
    Matrix() {}
    ~Matrix() {}
    /*
    void addAtrribute(const Attribute & attr)
    {
        _attributes.push_back(attr);
    }
    const Attribute & getAttribute(int index) const
    {
        assert(index < _attributes.size());
        return _attributes[index];
    }

    int getAttributeSize() const
    {
        return _attributes.size();
    }

    */

    /*
    * add the row at the given index;
    */
    void addRow(int row, const SparseInstance & instance)
    {
        _rows[row] = instance;
    }

    /*
    * Add a row, the rows number
    * will increment automatically.
    * This is used for add a sequence number of rows.
    */
    int addRow(const SparseInstance & instance)
    {
        int row = _rows.size();
        _rows[row] = instance;
        return row;
    }

    SparseInstance & getRow(int index)
    {
        assert(index < _rows.size());
        return _rows[index];
    }

    void removeRow(int index)
    {
        if(_rows.find(index) != _rows.end())
        {
            _rows.erase(index);
        }
    }

    int getRowSize()
    {
        return _rows.size();
    }

    SparseInstance & operator[](int index)
    {
        return _rows[index];
    }

    double val(int row, int col)
    {
        if(_rows.find(row) != _rows.end())
        {
            return _rows[row].getValue(col);
        }
        return 0;
    }

    void setVal(int row, int col, double val)
    {
        _rows[row].setValue(col, val);
    }

    friend ostream & operator<< (ostream & out, const Matrix & matrix) {
        out<<"{matrix rows:"<<matrix._rows.size()<<endl;
        for(hash_map<int, SparseInstance>::const_iterator it = matrix._rows.begin(); 
            it != matrix._rows.end(); it++){
            out<<"\t["<<it->first<<"]:"<<it->second<<endl;
        }
        out<<"}";
        return out;
    }
};
}
