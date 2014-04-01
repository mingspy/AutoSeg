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

using namespace std;
namespace mingspy
{
class DictFactory
{
private:
    static Dictionary * _coreDict;
    static Dictionary * _inverseCoreDict;
    static bool _loaded;
    static ResGuard _resGard;

public:
    static void initialize(const string & dir)
    {
        ResGuard::Lock lock(_resGard);
        if(_loaded) return;
        _coreDict = new Dictionary(dir+"core.dic");
        _inverseCoreDict = new Dictionary(dir+"inverseCore.dic");

        _loaded = true;
        atexit(clean);
    }

    static const Dictionary & CoreDict()
    {
        if(!_coreDict) {
            initialize("../data/");
        }
        return *_coreDict;
    }

    static const Dictionary & InverseCoreDict()
    {
        if(!_inverseCoreDict) {
            initialize("../data/");
        }
        return *_inverseCoreDict;
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
bool DictFactory::_loaded = false;
ResGuard DictFactory::_resGard;
}
