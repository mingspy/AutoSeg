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
using namespace std;

namespace mingspy
{
/**
 * Class for handling an attribute. Once an attribute has been created,
 * it can't be changed.
 *
 * Attribute is same as the title of each column in a matrix.
 * At present, int value is sufficient for all my usage.
 */
class Attribute
{
private:
    wstring _name;
    int _index;
    int _value;
public:
    Attribute(const wstring & name, int index = -1, int val = 0)
        :_name(name), _index(index), _value(val)
    {

    }

    const wstring & getName() const
    {
        return _name;
    }
    int getIndex()
    {
        return _index;
    }
    int getValue()
    {
        return _value;
    }
    void setIndex(int index)
    {
        _index = index;
    }
    void setValue(int val)
    {
        _value = val;
    }
};
}

