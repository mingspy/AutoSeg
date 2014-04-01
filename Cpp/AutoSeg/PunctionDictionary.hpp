#pragma once

#include <iostream>
#include <string>
#include "MemoryPool.hpp"
#include "MemLeaksCheck.h"

#include "HashMapDef.hpp"
using namespace std;
using namespace stdext;
namespace mingspy
{
class PunctionDictionary
{
private:
    hash_set<wchar_t> _punctions;
public:
    PunctionDictionary()
    {
        //  wstring punctuations = L"£ª£¬¡££¿£¨£©¡±¡°£«£«£«£­£­£­£­£®£¥¡¢£¯£½£¾¡À£¥¡Á¡Á¡Á¡ª¡ª¡ª¡ª¡ª¡ª¡ª£­¡®¡¯¡­¡­¡ë¡ù¡ú¡Î¡Ã¡Ù¢Ù¢Ú¢Û¢Ü¢Ý¢Þ¢ß¢à¢Æ¢Ç©¤¡õ¡ø¡÷¡ð¡ñ¡ï¡¢¡£¡±¡´¡µ¡¶¡·¡º¡»¡²¡³";
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
        _punctions.insert(L'\¡°');
        _punctions.insert(L'\¡±');
        _punctions.insert(L'£»');
        _punctions.insert(L';');
    }

    bool exists(wchar_t ch) const
    {
        return _punctions.find(ch) != _punctions.end();
    }
};
}