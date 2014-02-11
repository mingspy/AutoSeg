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
#include "WordDictionary.hpp"
#include "ResGuard.hpp"

using namespace std;
namespace mingspy{
    class DictFactory{
    private:
        static WordDictionary * _coreDict;
        static bool _loaded;
        static ResGuard _resGard;
        static void initialize(const string & dir){
            ResGuard::Lock lock(_resGard);
            if(_loaded) return;
            _coreDict = new WordDictionary(dir+"core.dic");
            _loaded = true;
            atexit(clean);
        }
    public:
        
        static const WordDictionary & CoreDict(){
            if(!_coreDict){
                initialize("../data/");
            }
            return *_coreDict;
        }

        static void clean(){
            ResGuard::Lock lock(_resGard);
            if(_loaded){
                if(_coreDict){
                    delete _coreDict;
                    _coreDict = NULL;
                }
                _loaded = false;
            }
        }
    };
    
    WordDictionary * DictFactory::_coreDict = NULL;
    bool DictFactory::_loaded = false;
    ResGuard DictFactory::_resGard;
}