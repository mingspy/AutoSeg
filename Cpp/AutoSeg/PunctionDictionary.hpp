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
        //  wstring punctuations = L"����������������������������������������������������������������������������������Ρá٢٢ڢۢܢݢޢߢ�Ƣǩ������������������������������";
        _punctions.insert(L'��');
        _punctions.insert(L'��');
        _punctions.insert(L'��');
        _punctions.insert(L'��');
        _punctions.insert(L'��');
        _punctions.insert(L'��');
        _punctions.insert(L'��');
        _punctions.insert(L',');
        _punctions.insert(L'!');
        _punctions.insert(L'?');
        _punctions.insert(L'(');
        _punctions.insert(L')');
        _punctions.insert(L'��');
        _punctions.insert(L':');
        _punctions.insert(L'\"');
        _punctions.insert(L'\��');
        _punctions.insert(L'\��');
        _punctions.insert(L'��');
        _punctions.insert(L';');
    }

    bool exists(wchar_t ch) const
    {
        return _punctions.find(ch) != _punctions.end();
    }
};
}