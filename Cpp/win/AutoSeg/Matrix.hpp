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



namespace mingspy
{

/*
*/
template<typename T>
class Matrix
{
protected:
    //vector<Attribute> _attributes;
    hash_map<int, SparseInstance<T> > _rows;
public:
    Matrix() {}
    ~Matrix()
    {
    }

    bool hasRow(int row)
    {
        return _rows.find(row) != _rows.end();
    }

    int addRow(int row, const SparseInstance<T> & instance)
    {
        _rows[row] = instance;
        return row;
    }

    /*
    * Add a row, the rows number
    * will increment automatically.
    * This is used for add a sequence number of rows.
    */
    int addRow(const SparseInstance<T> & instance)
    {
        return addRow(_rows.size(), instance);
    }

    SparseInstance<T> & getRow(int row)
    {
        return _rows[row];
    }

    void removeRow(int index)
    {
        _rows.erase(index);
    }

    int RowSize()
    {
        return _rows.size();
    }


    SparseInstance<T> & operator[](int row)
    {
        return _rows[row];
    }

    T & val(int row, int col)
    {
        return _rows[row].getAttrValue(col);
    }

    void setVal(int row, int col, const T& val)
    {
        _rows[row].setAttrValue(col, val);
    }

    friend ostream & operator<< (ostream & out,  Matrix<T> & matrix)
    {
        out<<"{matrix rows:"<<matrix._rows.size()<<endl;
        for(int i = 0; i < matrix._rows.size(); i++) {
            out<<i<<"->"<<matrix._rows[i]<<endl;
        }
        out<<"}";
        return out;
    }
};

typedef Matrix<double> Graph;
}
