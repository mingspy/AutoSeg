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
#include <stdlib.h>
#include <iostream>
#include "Dictionary.hpp"
#include "ResGuard.hpp"
#include "Configuration.hpp"

#if _MSC_VER > 1000
#include "PunctionDictionary.hpp"
#else
#include "PunctionDictionaryu.hpp"
#endif

using namespace std;
namespace mingspy
{
class DictFactory
{
private:
    static Dictionary * _coreDict;
    static Dictionary * _inverseCoreDict;
    static ShiftContext * _lexicalDict;
    static bool _loaded;
    static ResGuard _resGard;
    static PunctionDictionary _puncDict;
public:
    static void initialize()
    {
        ResGuard::Lock lock(_resGard);
        if(_loaded) return;
        Configuration &conf = Configuration::instance();
        _coreDict = new Dictionary(conf.getString(KEY_CORE_PATH));

        if(conf.getBool(KEY_ISLOAD_INVS)) {
            _inverseCoreDict = new Dictionary(conf.getString(KEY_INVS_PATH));
        }
        _lexicalDict = new ShiftContext(conf.getString(KEY_LEXICAL_PATH));
        _loaded = true;
        atexit(clean);
    }

    static const Dictionary & CoreDict()
    {
        if(!_loaded) {
            initialize();
        }
        return *_coreDict;
    }

    static const Dictionary & BigramDict()
    {
        if(!_loaded) {
            initialize();
        }
        return *_coreDict;
    }

    static const Dictionary & InverseCoreDict()
    {
        if(!_loaded) {
            initialize();
        }
        return *_inverseCoreDict;
    }

    static const ShiftContext & LexicalDict()
    {
        if(!_loaded) {
            initialize();
        }
        return *_lexicalDict;
    }

    static const PunctionDictionary & Puntions()
    {
        if(!_loaded) {
            initialize();
        }
        return _puncDict;
    }

    static void clean()
    {
        ResGuard::Lock lock(_resGard);
        if(_loaded) {
            if(_coreDict) {
                delete _coreDict;
                _coreDict = NULL;
            }
            if(_inverseCoreDict) {
                delete _inverseCoreDict;
                _inverseCoreDict = NULL;
            }
            _loaded = false;
        }
    }
};

Dictionary * DictFactory::_coreDict = NULL;
Dictionary * DictFactory::_inverseCoreDict = NULL;
ShiftContext * DictFactory::_lexicalDict = NULL;
bool DictFactory::_loaded = false;
ResGuard DictFactory::_resGard;
PunctionDictionary DictFactory::_puncDict;
}
