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
#include "MemoryPool.hpp"
#include "MemLeaksCheck.h"

#include "HashMapDef.hpp"
using namespace std;

namespace mingspy
{
class PunctionDictionary
{
private:
    hash_set<wchar_t> _punctions;
public:
    PunctionDictionary()
    {
        //  wstring punctuations = L"£ª£¬¡££¿£¨£©¡±¡°£«£«£«£­£­£­£­£®£¥¡¢£¯£½£¾¡À£¥¡Á¡Á¡Á
        // ¡ª¡ª¡ª¡ª¡ª¡ª¡ª£­¡®¡¯¡­¡­¡ë¡ù¡ú¡Î¡Ã¡Ù¢Ù¢Ú¢Û¢Ü¢Ý¢Þ¢ß¢à¢Æ¢Ç©¤¡õ¡ø¡÷¡ð¡ñ¡ï¡¢¡£¡±
        //¡´¡µ¡¶¡·¡º¡»¡²¡³";
        _punctions.insert(L'¡£');
        _punctions.insert(L'£¬');
        _punctions.insert(L'£¡');
        _punctions.insert(L'£¨');
        _punctions.insert(L'£©');
        _punctions.insert(L'¡¢');
        _punctions.insert(L'£¿');
        _punctions.insert(L',');
        _punctions.insert(L'!');
        _punctions.insert(L'?');
        _punctions.insert(L'(');
        _punctions.insert(L')');
        _punctions.insert(L'£º');
        _punctions.insert(L':');
        _punctions.insert(L'\"');
        _punctions.insert(L'¡°');
        _punctions.insert(L'¡±');
        _punctions.insert(L'£»');
        _punctions.insert(L';');
        _punctions.insert(L'¡¶');
        _punctions.insert(L'¡·');
    }

    bool exists(wchar_t ch) const
    {
        return _punctions.find(ch) != _punctions.end();
    }
};
}