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

using namespace std;
namespace mingspy
{
class Tail
{

    class TailBlock
    {
    public :
        int next_free;
        void * data;
        TrieChar * suffix;
    };

private:
    static const int TAIL_START_BLOCKNO = 1;
    static const int TAIL_SIGNATURE = 0xffddabaa;
private:
    int num_tails;
    TailBlock * tails;
    int first_free;
    TailDataFree _data_free_func;
    WriteTailDataToFile _data_writer;
    ReadTailDataFromFile _data_reader;
    MemoryPool<> * pmem;
public:
    Tail()
    {
        num_tails = 1;
        tails = (TailBlock *) malloc(sizeof(TailBlock));
        tails[0].data = NULL;
        tails[0].suffix = NULL;
        tails[0].next_free = TAIL_SIGNATURE;
        first_free = 0;
        _data_free_func = NULL;
        _data_writer = NULL;
        _data_reader = NULL;
        pmem = NULL;
    }

    inline void setDataFreer( TailDataFree fn )
    {
        _data_free_func = fn;
    }

    inline void setDataWriter( WriteTailDataToFile fn )
    {
        _data_writer = fn;
    }

    inline void setDataReader( ReadTailDataFromFile fn )
    {
        _data_reader = fn;
    }

    inline void setMemPool(MemoryPool<> * pmemory)
    {
        pmem = pmemory;
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
    inline TrieChar * getSuffix(int index) const
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
     *            : the new suffix
     *            Set suffix of existing entry of given index in tail.
     */
    bool setSuffix(int index, const TrieChar * suffix)
    {
        index -= TAIL_START_BLOCKNO;
        if (index < num_tails)
        {
            /*
             * suffix and tails[index].suffix may overlap; so, dup it before
             * it's overwritten
             */
            int len = TrieStrLen(suffix);
            TrieChar * tmp = NULL;
            if(!pmem)
            {
                tmp = new TrieChar[len + 1];
            }
            else
            {
                tmp = (TrieChar *)pmem->allocAligned((len + 1) * sizeof(TrieChar));
            }
            memcpy(tmp, suffix, (len+1)*sizeof(TrieChar));

            if(tails[index].suffix != NULL&&!pmem)
            {
                delete [] tails[index].suffix;
            }

            tails[index].suffix = tmp;
            return true;
        }
        return false;
    }

    /**
     * @brief Add a new suffix
     *
     * @param t
     *            : the tail data
     * @param suffix
     *            : the new suffix
     *
     * @return the index of the newly added suffix.
     *
     *         Add a new suffix entry to tail.
     */
    int addSuffix(const TrieChar * suffix)
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
     *
     * @return bool indicating success
     *         Set data associated to suffix entry index in tail data.
     */
    inline bool setData(int index, void * data)
    {
        index -= TAIL_START_BLOCKNO;
        if (index < num_tails)
        {
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
    int walkStr(int s, int * suffix_idx, TrieChar * str, int len)
    {
        TrieChar * suffix = getSuffix(s);
        if (suffix == NULL)
            return 0;

        int i = 0;
        int j = *suffix_idx;
        while (i < len)
        {
            if (str[i] != suffix[j])
                break;
            ++i;
            /* stop and stay at null-terminator */
            if ((TrieChar)0 == suffix[j])
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
    inline bool walkChar(int s, int * suffix_idx, TrieChar c) const
    {
        int suffix_char;

        TrieChar * suffix = getSuffix(s);
        if (suffix == NULL)
            return false;

        suffix_char = suffix[*suffix_idx];
        if (suffix_char == c)
        {
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
        if (!file_write_int32 (file, TAIL_SIGNATURE) ||
                !file_write_int32 (file, first_free)  ||
                !file_write_int32 (file, num_tails))
        {
            return false;
        }

        if(fwrite(tails, sizeof(TailBlock), num_tails, file) != num_tails)
        {
            return false;
        }

        for (int i = 1; i < num_tails; i++)
        {
            if(tails[i].data != NULL)
            {
                _data_writer(file, tails[i].data);
            }

            if(tails[i].suffix != NULL)
            {
                WriteTrieStrToFile(file, tails[i].suffix);
            }
        }

        return true;
    }

    bool readFromFile(FILE * file)
    {
        long        save_pos;
        int   i;
        int      sig;

        /* check signature */
        save_pos = ftell (file);
        if (!file_read_int32 (file, &sig) || TAIL_SIGNATURE != sig)
            goto exit_file_read;
        clear();

        if (!file_read_int32 (file, &first_free) ||
                !file_read_int32 (file, &num_tails))
        {
            goto exit_file_read;
        }
        if (num_tails > SIZE_MAX / sizeof (TailBlock))
            goto exit_file_read;
        tails = (TailBlock *) malloc (num_tails * sizeof (TailBlock));
        if (!tails)
            goto exit_file_read;

        if(fread(tails, sizeof(TailBlock), num_tails, file) != num_tails)
        {
            free(tails);
            goto exit_file_read;
        }
        for (i = 1; i < num_tails; i++)
        {
            if(tails[i].data != NULL)
            {
                void * data = _data_reader(file, pmem);
                if(!data)
                {
                    goto exit_in_loop;
                }
                tails[i].data = data;
            }

            if(tails[i].suffix != NULL)
            {
                TrieChar * str = (TrieChar *)ReadTrieStrFromFile(file, pmem);
                if(!str)
                {
                    goto exit_in_loop;
                }
                tails[i].suffix = str;
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
        if(tails != NULL)
        {
            if(!pmem)
            {
                for(int i = 1; i < num_tails; i++)
                {
                    if(_data_free_func != NULL && tails[i].data != NULL)
                    {
                        _data_free_func(tails[i].data);
                    }

                    if(tails[i].suffix != NULL)
                    {
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
        if (0 != first_free)
        {
            block = first_free;
            first_free = tails[block].next_free;
        }
        else
        {
            block = num_tails;
            /*
            TailBlock * tmp = (TailBlock *)new TailBlock *[++num_tails];
            memcpy(tmp, tails, block * sizeof(TailBlock ));
            delete [] tails;
            tails = tmp;
            */
            tails = (TailBlock *)realloc(tails, ++num_tails*sizeof(TailBlock));
        }
        tails[block].next_free = -1;
        tails[block].data = NULL;
        tails[block].suffix = NULL;

        return block + TAIL_START_BLOCKNO;
    }

    void freeBlock(int block)
    {
        block -= TAIL_START_BLOCKNO;
        if (block >= num_tails)
            return;
        if(tails[block].suffix != NULL)
        {
            delete [] tails[block].suffix;
        }

        if(tails[block].data != NULL && _data_free_func != NULL)
        {
            _data_free_func(tails[block].data);
        }
        tails[block].data = NULL;
        tails[block].suffix = NULL;

        /* find insertion point */
        int i, j = 0;
        for (i = first_free; i != 0 && i < block; i = tails[i].next_free)
            j = i;

        /* insert free block between j and i */
        tails[block].next_free = i;
        if (0 != j)
            tails[j].next_free = block;
        else
            first_free = block;
    }

};
}

