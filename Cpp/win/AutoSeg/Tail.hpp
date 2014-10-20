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
#include <memory>
#include <cassert>
#include <limits.h>
#include "TrieDef.hpp"
#include "FileUtils.hpp"
#include "MemoryPool.hpp"
#include "MemLeaksCheck.h"
#include <wchar.h>

#ifndef SIZE_MAX
#ifdef _WIN64
#define SIZE_MAX _UI64_MAX
#else
#define SIZE_MAX UINT_MAX
#endif
#endif

using namespace std;
namespace mingspy
{

// A help class used to
// make the suffix be safe-use.


class Tail
{

    class TailBlock
    {
    public :
        int next_free;
        void * data;
        wchar_t * suffix;
    };

private:
    static const int TAIL_START_BLOCKNO = 1;
    static const int TAIL_SIGNATURE = 0xffddabaa;
    static const int INCREASEMENT_SIZE = 16;
private:
    int num_tails;
    TailBlock * tails;
    int first_free;
    MemoryPool<> * _pmem;
    TailDataReader _data_reader;
    TailDataWriter _data_writer;
    TailDataFreer  _data_freer;
public:
    Tail()
    {
        num_tails = 1;
        tails = (TailBlock *) malloc(sizeof(TailBlock));
        tails[0].data = NULL;
        tails[0].suffix = NULL;
        tails[0].next_free = TAIL_SIGNATURE;
        first_free = 0;
        _pmem = NULL;
        _data_reader = NULL;
        _data_writer = NULL;
        _data_freer =  NULL;
    }

    inline void setMemPool(MemoryPool<> * pmemory)
    {
        _pmem = pmemory;
    }

    inline void setDataReader(TailDataReader reader)
    {
        _data_reader = reader;
    }

    inline void setDataWriter(TailDataWriter writer)
    {
        _data_writer = writer;
    }

    inline void setDataFreer(TailDataFreer freer)
    {
        _data_freer = freer;
    }

    ~Tail()
    {
        clear();
    }

    /**
     * @brief Get suffix
     *
     * @param t
     *            : the tail data
     * @param index
     *            : the index of the suffix
     *
     * @return an allocated string of the indexed suffix.
     *         Get suffix from tail with given index. The returned string is
     *         allocated. The caller should free it with free().
     */
    inline const wchar_t * getSuffix(int index) const
    {
        index -= TAIL_START_BLOCKNO;
        return (index < num_tails) ? tails[index].suffix : NULL;
    }

    /**
     * @brief Set suffix of existing entry
     *
     * @param t
     *            : the tail data
     * @param index
     *            : the index of the suffix
     * @param suffix
     *            : the suffix to be set, notice:the suffix[len - 1] must be zero.
     * @param len : the len of suffix.
     */
    bool setSuffix(int index, const wstring & suffix)
    {
        index -= TAIL_START_BLOCKNO;

        if (index < num_tails) {
            /*
             * suffix and tails[index].suffix may overlap; so, dup it before
             * it's overwritten
             */
            int size = suffix.length() + 1;
            wchar_t * tmp = NULL;
            if(!_pmem) {
                tmp = new wchar_t[size];
            } else {
                tmp = (wchar_t *)_pmem->allocAligned((size)*sizeof(wchar_t));
            }
            memcpy(tmp, suffix.c_str(), (size)*sizeof(wchar_t));
            if(tails[index].suffix != NULL&&!_pmem) {
                delete [] tails[index].suffix;
            }

            tails[index].suffix = tmp;
            //tails[index].suffix_len = len;
            return true;
        }
        return false;
    }

    /**
     * @brief Add a new suffix
     *
     * @param suffix
     *            : the new suffix notice:the suffix[len - 1] must be zero.
     * @param len : the len of suffix.
     * @return the index of the newly added suffix.
     */
    int addSuffix(const wstring & suffix)
    {
        int new_block = allocBlock();
        setSuffix(new_block, suffix);
        return new_block;
    }


    /**
     * @brief Get data associated to suffix entry
     *
     * @param t
     *            : the tail data
     * @param index
     *            : the index of the suffix
     *
     * @return the data associated to the suffix entry
     *         Get data associated to suffix entry index in tail data.
     */
    inline void * getData(int index) const
    {
        index -= TAIL_START_BLOCKNO;
        return (index < num_tails) ? tails[index].data : NULL;
    }

    /**
     * @brief Set data associated to suffix entry
     *
     * @param t
     *            : the tail data
     * @param index
     *            : the index of the suffix
     * @param data
     *            : the data to set
     * @param bytes : how many bytes the data has.
     * @return bool indicating success
     *         Set data associated to suffix entry index in tail data.
     */
    inline bool setData(int index, void * data)
    {
        index -= TAIL_START_BLOCKNO;
        if (index < num_tails) {
            if(tails[index].data && _data_freer) {
                _data_freer(tails[index].data);
            }
            tails[index].data = data;
            return true;
        }
        return false;
    }

    /**
     * @brief Delete suffix entry
     *
     * @param t
     *            : the tail data
     * @param index
     *            : the index of the suffix to delete
     *            Delete suffix entry from the tail data.
     */
    void remove(int index)
    {
        freeBlock(index);
    }

    /**
     * @brief Walk in tail with a string
     *
     * @param t
     *            : the tail data
     * @param s
     *            : the tail data index
     * @param suffix_idx
     *            : pointer to current character index in suffix
     * @param str
     *            : the string to use in walking
     * @param len
     *            : total characters in str to walk
     *
     * @return total number of characters successfully walked
     *         Walk in the tail data t at entry s, from given character position
     *         *suffix_idx, using len characters of given string str. On
     *         return,*suffix_idx is updated to the position after the last
     *         successful walk, and the function returns the total number of
     *         character successfully walked.
     */
    int walkStr(int s, int * suffix_idx, wchar_t * str, int len)
    {
        const wchar_t * suffix = getSuffix(s);
        if (suffix == NULL)
            return 0;

        int i = 0;
        int j = *suffix_idx;
        while (i < len) {
            if (str[i] != suffix[j])
                break;
            ++i;
            /* stop and stay at null-terminator */
            if (0 == suffix[j])
                break;
            ++j;
        }
        *suffix_idx = j;
        return i;
    }

    /**
     * @brief Walk in tail with a character
     *
     * @param t
     *            : the tail data
     * @param s
     *            : the tail data index
     * @param suffix_idx
     *            : pointer to current character index in suffix
     * @param c
     *            : the character to use in walking
     *
     * @return true indicating success Walk in the tail data t at entry s, from
     *         given character position suffix_idx, using given character c. If
     *         the walk is successful, it returns true, and *suffix_idx is
     *         updated to the next character. Otherwise, it returns false, and
     *         *suffix_idx is left unchanged.
     */
    inline bool walkChar(int s, int * suffix_idx, int c) const
    {
        int suffix_char;

        const wchar_t * suffix = getSuffix(s);
        if (suffix == NULL)
            return false;

        suffix_char = suffix[*suffix_idx];
        if (suffix_char == c) {
            if (0 != suffix_char)
                *suffix_idx = *suffix_idx + 1;
            return true;
        }
        return false;
    }

    /*
    * Write tail to a file.
    * When call this function, ensure the _data_writer must not be NULL.
    */
    bool writeToFile(FILE * file)
    {
        Serializer serializer(file);
        if (!serializer.writeInt32(TAIL_SIGNATURE) ||
                !serializer.writeInt32(first_free)  ||
                !serializer.writeInt32(num_tails)) {
            return false;
        }

        if(!serializer.write(tails, sizeof(TailBlock), num_tails)) {
            return false;
        }

        for (int i = 1; i < num_tails; i++) {
            if(tails[i].data != NULL) {
                _data_writer(file,tails[i].data);
            }

            if(tails[i].suffix != NULL) {
                int len = wcslen(tails[i].suffix);
                serializer.writeInt16(len);
                serializer.writeWstrData(tails[i].suffix, len);
            }
        }

        return true;
    }

    bool readFromFile(FILE * file)
    {
        long save_pos = ftell (file);
        int suffixlen;
        Serializer serializer(file);
        /* check signature */
        int sig;
        if ((sig = serializer.readInt32()) != TAIL_SIGNATURE)
            goto exit_file_read;
        clear();

        first_free = serializer.readInt32();
        num_tails = serializer.readInt32();
        if (num_tails > SIZE_MAX / sizeof (TailBlock))
            goto exit_file_read;
        tails = (TailBlock *) malloc (num_tails * sizeof (TailBlock));
        if (!tails)
            goto exit_file_read;

        if(!serializer.read(tails, sizeof(TailBlock), num_tails)) {
            free(tails);
            goto exit_file_read;
        }
        int i = 0;
        for (i = 1; i < num_tails; i++) {
            if(tails[i].data != NULL) {
                tails[i].data = _data_reader(file);
            }

            if(tails[i].suffix != NULL) {
                suffixlen = serializer.readInt16();
                if(_pmem) {
                    tails[i].suffix = (wchar_t * )_pmem->allocAligned((suffixlen+1)*sizeof(wchar_t));
                } else {
                    tails[i].suffix = new wchar_t[suffixlen+1];
                }
                if(!serializer.readWstrData(suffixlen,tails[i].suffix)) {
                    goto exit_in_loop;
                }
            }
        }

        return true;
exit_in_loop:
        num_tails = i;
        clear();

exit_file_read:
        fseek (file, save_pos, SEEK_SET);
        assert(false);
        return false;
    }

private:

    void clear()
    {
        if(tails != NULL) {

            if(_data_freer) {
                for(int i = 1; i < num_tails; i++) {
                    if(tails[i].data != NULL) {
                        _data_freer(tails[i].data);
                    }
                }
            }

            if(!_pmem) {
                for(int i = 1; i < num_tails; i++) {
                    if(tails[i].suffix != NULL) {
                        delete [] tails[i].suffix;
                    }
                }

            }

            free(tails);
        }

        tails = NULL;
        num_tails = 0;
    }

    int allocBlock()
    {
        int block;
        if (0 != first_free) {
            block = first_free;
            first_free = tails[block].next_free;
        } else {
            tails = (TailBlock *)realloc(tails, (INCREASEMENT_SIZE+num_tails)*sizeof(TailBlock));
            memset(tails+num_tails, 0, sizeof(TailBlock) * INCREASEMENT_SIZE);
            block = num_tails;
            num_tails += INCREASEMENT_SIZE;
            first_free = block + 1;
            for(int i = first_free; i < num_tails - 1; i++) {
                tails[i].next_free = i + 1;
            }
            tails[num_tails - 1].next_free = 0;

        }
        return block + TAIL_START_BLOCKNO;
    }

    void freeBlock(int block)
    {
        block -= TAIL_START_BLOCKNO;
        if (block >= num_tails)
            return;
        if(tails[block].suffix != NULL&&!_pmem) {
            delete [] tails[block].suffix;
        }

        if(tails[block].data != NULL&& _data_freer) {
            _data_freer(tails[block].data);
        }

        tails[block].data = NULL;
        tails[block].suffix = NULL;

        // insert block to free list, as the
        // first one.
        tails[block].next_free = first_free;
        first_free = block;
    }

};
}
