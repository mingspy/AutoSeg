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
#include <stdio.h>
#include <stdlib.h>
#include "HashMapDef.hpp"
#include <string>
#include "FileUtils.hpp"
#include "StrUtils.hpp"
#include "ResGuard.hpp"
using namespace std;

namespace mingspy
{
#if _MSC_VER > 1000
static const string DEFAULT_CONF_PATH = "D:/autoseg/data/autoseg.conf";
static const string DEFAULT_CORE_DICT_PATH = "D:/autoseg/data/core.dic";
static const string DEFAULT_INVS_DICT_PATH = "D:/autoseg/data/invs.dic";
static const string DEFAULT_LEXICAL_DICT_PATH = "D:/autoseg/data/lexical.dic";

#else
static const string DEFAULT_CONF_PATH = "/opt/apps/autoseg/data/autoseg.conf";
static const string DEFAULT_CORE_DICT_PATH = "/opt/apps/autoseg/data/core.dic";
static const string DEFAULT_INVS_DICT_PATH = "/opt/apps/autoseg/data/invs.dic";
static const string DEFAULT_LEXICAL_DICT_PATH = "/opt/apps/autoseg/data/lexical.dic";

#endif
static const string DEFAULT_ISLOAD_INVS = "false";

const string ENV_AUTOSEG_CONF_PATH = "AUTOSEG_CONF_PATH";
const string KEY_CONF_PATH = "CONF_PATH";
const string KEY_CORE_PATH = "CORE_DICT_PATH";
const string KEY_INVS_PATH = "INVS_DICT_PATH";
const string KEY_LEXICAL_PATH = "LEXICAL_DICT_PATH";
const string KEY_ISLOAD_INVS = "ISLOAD_INVS";

static ResGuard _confGuard;

/*
* Utility to read configurations.
* The default configuration path was set to $DEFAULT_CONF_PATH.
* user can put config under the path. Or Set environment AUTOSEG_CONF_PATH to
* let the configuration know where the config file located.
*/
class Configuration
{
public:

    static Configuration & instance()
    {
        ResGuard::Lock lock(_confGuard);
        static Configuration _theConf;
        return _theConf;
    }

    string getString(const string & key)
    {
        if(_confs.find(key) != _confs.end()) {
            return _confs[key];
        }
        return "";
    }

    int getInt(const string & key, int default_val = 0)
    {
        if(_confs.find(key) != _confs.end()) {
            return atoi(_confs[key].c_str());
        }

        return default_val;
    }

    bool getBool(const string &key, bool default_val = false)
    {
        if(_confs.find(key) != _confs.end()) {
            string & val = _confs[key];
            return ((val == "TRUE") || (val == "true"));
        }

        return default_val;
    }

    double getDouble(const string & key, double default_val = 0)
    {
        if(_confs.find(key) != _confs.end()) {
            return atof(_confs[key].c_str());
        }

        return default_val;
    }
private:
    Configuration()
    {
        setDefaultConfigurations();
        loadSettings();
    }

    void loadSettings()
    {
        char * pconf = getenv(ENV_AUTOSEG_CONF_PATH.c_str());
        if(pconf != NULL) {
            _confs[KEY_CONF_PATH] = pconf;
        }
        loadSettingsFromConf(_confs[KEY_CONF_PATH]);
    }

    void loadSettingsFromConf(const string & path)
    {
        LineFileReader reader(path);
        string * line = NULL;
        while((line = reader.getLine()) != NULL) {
            int idx = line->find_first_of('=');
            if(idx != line->npos) {
                string key = trim(line->substr(0, idx));
                string val = trim(line->substr(idx+1));
                _confs[key] = val;
            }
        }
    }

    void setDefaultConfigurations()
    {
        _confs[KEY_CONF_PATH] = DEFAULT_CONF_PATH;
        _confs[KEY_CORE_PATH] = DEFAULT_CORE_DICT_PATH;
        _confs[KEY_INVS_PATH] = DEFAULT_INVS_DICT_PATH;
        _confs[KEY_LEXICAL_PATH] = DEFAULT_LEXICAL_DICT_PATH;
        _confs[KEY_ISLOAD_INVS] = DEFAULT_ISLOAD_INVS;
    }
private:
    hash_map<string, string> _confs;

};
}

