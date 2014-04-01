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

#if _MSC_VER > 1000
#include <hash_map>
#else
#include "CodeUtils.hpp"
// in linux..
#include <ext/hash_map>
using namespace __gnu_cxx;
using namespace std;

namespace __gnu_cxx
{
template<> struct hash<string> {
    size_t operator()(const string& s) const
    {
        return __stl_hash_string(s.c_str());
    }
};

template<> struct hash<wstring> {
    size_t operator()(const wstring& s) const
    {
        string str = ws2s(s);
        return __stl_hash_string(str.c_str());
    }
};
}
#endif
