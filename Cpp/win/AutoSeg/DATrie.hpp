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


#include "TrieDef.hpp"
#include "Tail.hpp"
#include "DoubleArray.hpp"
#include "MemLeaksCheck.h"



namespace mingspy
{


class DATrie
{
private:
    DoubleArray _da;
    Tail _tail;
    bool is_dirty;

public:
    DATrie(TailDataFreer fn = NULL):is_dirty(false)
    {
        _tail.setDataFreer(fn);
    }

    Tail & getTail()
    {
        return _tail;
    }

    /**
     * @brief Store a value for an entry to trie
     *
     * @param trie
     *            : the trie
     * @param key
     *            : the key for the entry to retrieve
     * @param data
     *            : the data associated to the entry
     *
     * @return boolean value indicating the success of the process Store a data
     *         for the given key in trie. If key does not exist in trie, it will
     *         be appended. If it does, its current data will be overwritten.
     */
    inline bool add(const wstring & key, void * val)
    {
        return storeConditionally(key, val, true);
    }
    /**
    * @brief Delete an entry from trie
    *
    * @param trie
    *            : the trie
    * @param key
    *            : the key for the entry to delete
    *
    * @return boolean value indicating whether the key exists and is removed
    *         Delete an entry for the given key from trie.
    */
    bool remove(const wstring & key)
    {
        int t;
        /* walk through branches */
        int s = _da.getRoot();
        int keylen = key.length();
        const wchar_t * p = key.c_str();
        for (; !isSeparate(s); p++) {
            if (!_da.walk(&s, *p)) {
                return false;
            }
            if (0 == *p)
                break;
        }

        /* walk through tail */
        t = getTailIndex(s);
        int suffix_idx = 0;
        for (; ; p++) {
            if (!_tail.walkChar(t, &suffix_idx, *p)) {
                return false;
            }
            if (0 == *p)
                break;
        }

        _tail.remove(t);
        _da.setBase(s, TRIE_INDEX_ERROR);
        _da.prune(s);

        is_dirty = true;
        return true;
    }

    /**
     * @brief Retrieve an entry from trie
     *
     * @param trie
     *            : the trie
     * @param key
     *            : the key for the entry to retrieve
     * @param o_data
     *            : the storage for storing the entry data on return
     *
     * @return boolean value indicating the existence of the entry.
     *         Retrieve an entry for the given key from trie. On return, if key
     *         is found and o_data is not NULL, *o_data is set to the data
     *         associated to key.
     */
    void * retrieve(const wstring & key) const
    {
        /* walk through branches */
        int s = _da.getRoot();
        const wchar_t * p = key.c_str();
        for (; !isSeparate(s); p++) {
            if (!_da.walk(&s, *p)) {
                return NULL;
            }
            if (0 == *p)
                break;
        }

        /* walk through tail */
        s = getTailIndex(s);
        int suffix_idx = 0;
        for (; ; p++) {
            if (!_tail.walkChar(s, &suffix_idx, *p)) {
                return NULL;
            }
            if (0 == *p)
                break;
        }

        /* found, set the val and return */
        return _tail.getData(s);
    }

    bool containsPrefix(const wstring & prefix) const
    {
        /* walk through branches */
        int s = _da.getRoot();
        const wchar_t * p = prefix.c_str();
        for (; !isSeparate(s); p++) {
            if (0 == *p)
                return true;
            if (!_da.walk(&s, *p)) {
                return false;
            }
        }

        /* walk through tail */
        s = getTailIndex(s);
        int suffix_idx = 0;
        for (; ; p++) {
            if (0 == *p)
                return true;
            if (!_tail.walkChar(s, &suffix_idx, *p)) {
                return false;
            }

        }

        return true;
    }


    inline void setMemPool( MemoryPool<>* mem_pool )
    {
        _tail.setMemPool(mem_pool);
    }

    bool writeToFile(const char * file)
    {
        FILE * pfile = fopen(file, "wb");
        if(!pfile) {
            return false;
        }

        bool res = writeToFile(pfile);

        fflush(pfile);
        fclose(pfile);
        return res;
    }

    bool writeToFile(FILE * pfile)
    {
        if(!_da.writeToFile(pfile) || !_tail.writeToFile(pfile)) {
            return false;
        }
        return true;
    }

    bool readFromFile(const char * file)
    {
        FILE * pfile = fopen(file, "rb");
        if(!pfile) {
            return false;
        }

        bool res = readFromFile(pfile);

        fclose(pfile);
        return res;
    }

    bool readFromFile(FILE * pfile)
    {
        if(!_da.readFromFile(pfile) || !_tail.readFromFile(pfile)) {
            fclose(pfile);
            return false;
        }

        return false;
    }

private:
    inline bool isSeparate(int s) const
    {
        return _da.getBase(s) < 0;
    }

    inline int getTailIndex(int s) const
    {
        return -_da.getBase(s);
    }

    /**
     * @brief Store a value for an entry to trie only if the key is not present
     *
     * @param trie
     *            : the trie
     * @param key
     *            : the key for the entry to retrieve
     * @param data
     *            : the data associated to the entry
     *
     * @return boolean value indicating the success of the process Store a data
     *         for the given key in trie. If key does not exist in trie, it will
     *         be appended. If it does, the function will return failure and the
     *         existing value will not be touched. This can be useful for
     *         multi-thread applications, as race condition can be avoided.
     *         Available since: 0.2.4
     */
    inline bool storeIfAbsent(const wstring & key, void * data)
    {
        return storeConditionally(key, data,false);
    }

    bool storeConditionally(const wstring & key, void * data, bool is_overwrite)
    {
        /* walk through branches */
        int s = _da.getRoot();
        const wchar_t * p = key.c_str();
        for (; !isSeparate(s); p++) {
            if (!_da.walk(&s, *p)) {
                return branchInBranch(s, p, data);
            }
            if (0 == *p)
                break;
        }

        /* walk through tail */
        const wchar_t * sep = p;
        int t = getTailIndex(s);
        int suffix_idx = 0;
        for (; ; p++) {
            if (!_tail.walkChar(t, &suffix_idx, *p)) {
                return branchInTail(s, sep, data);

            }
            if (0 == *p)
                break;
        }

        /* duplicated key, overwrite val if flagged */
        if (!is_overwrite) {
            return false;
        }
        _tail.setData(t, data);
        is_dirty = true;
        return true;
    }

    inline void setTailIndex(int s, int v)
    {
        _da.setBase(s, -v);
    }

    bool branchInBranch(int sep_node, const wchar_t * suffix, void * data)
    {
        int new_da, new_tail;

        int i = 0;
        new_da = _da.insertBranch(sep_node, *suffix);
        if (TRIE_INDEX_ERROR == new_da)
            return false;

        if (0 != *suffix)
            ++suffix;
        new_tail = _tail.addSuffix(suffix);
        _tail.setData(new_tail, data);
        setTailIndex(new_da, new_tail);

        is_dirty = true;
        return true;
    }

    bool branchInTail(int sep_node,const wchar_t * suffix, void * data)
    {

        /* adjust separate point in old path */
        int old_tail = getTailIndex(sep_node);
        const wchar_t * old_suffix = _tail.getSuffix(old_tail);
        if (old_suffix == NULL)
            return false;

        int s = sep_node;
        const wchar_t * p = old_suffix;
        for (; *p == *suffix; p++, suffix ++) {
            int t = _da.insertBranch(s, *suffix);
            if (TRIE_INDEX_ERROR == t) {
                _da.pruneUpto(sep_node, s);
                setTailIndex(sep_node, old_tail);
                return false;
            }
            s = t;
        }

        int old_da = _da.insertBranch(s, *p);
        if (TRIE_INDEX_ERROR == old_da) {
            _da.pruneUpto(sep_node, s);
            setTailIndex(sep_node, old_tail);
            return false;
        }

        // when run here, all equals prefix are added.
        // save old left suffix.
        if (0 != *p)
            ++p;
        _tail.setSuffix(old_tail, p);
        setTailIndex(old_da, old_tail);

        /* insert the new branch at the new separate point */
        return branchInBranch(s, suffix, data);
    }

};

}
