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
#include <vector>
#include "TrieDef.hpp"
#include "MemLeaksCheck.h"


using namespace std;

namespace mingspy
{
class DoubleArray
{

private:
    static const int DA_ROOT = 2; // root address
    static const int DA_POOL_BEGIN = 3; //
    static const int DA_SIGNATURE = 0xfdeeabcd;

    /**
     * The address of free list header.<br>
     * As r1r2...ri...rm are m free cells.<br>
     * base[1] = -rm<br>
     * check[1] = -r1<br>
     * .....<br>
     * base[ri]= -r(i-1)<br>
     * check[ri] = -r(i+1)<br>
     * .....<br>
     * base[rm]= -r(m-1)<br>
     * check[ri] = -1<br>
     */
    static const int FREE_LIST_HEAD = 1; //

    // How many cell it call allocate.
    static const int TRIE_INDEX_MAX = 0x0fffffff;
private:
    struct Cell {
        int base;
        int check;
    };
    Cell * _cell;
    int num_cells;
public:

    DoubleArray()
    {
        init();
    }

    ~DoubleArray()
    {
        free(_cell);
    }


    inline int getRoot() const
    {
        return DA_ROOT;
    }

    inline int getBase(int position) const
    {
        return position < num_cells ? _cell[position].base : TRIE_INDEX_ERROR;
    }

    inline int getCheck(int position) const
    {
        return position < num_cells ? _cell[position].check : TRIE_INDEX_ERROR;
    }

    inline void setBase(int position, int value)
    {
        _cell[position].base = value;
    }

    inline bool isWorkable(int s, int c) const
    {
        return getCheck(getBase(s) + c) == s;
    }

    /**
     * Walk the double-array trie from state s, using input character c. If
     * there exists an edge from s with arc labeled c, this function returns
     * TRUE and s is updated to the new state. Otherwise, it returns FALSE and s
     * is left unchanged.
     *
     * @param s
     * @param c
     * @return
     */
    inline bool walk(int * s, wchar_t c) const
    {
        int next = getBase(*s) + (unsigned)c;
        if (getCheck(next) == *s) {
            *s = next;
            return true;
        }
        return false;
    }

    /**
     * @brief Insert a branch from trie node
     *
     * @param s
     *            : the state to add branch to
     * @param c
     *            : the character for the branch label
     *
     * @return the index of the new node
     *         Insert a new arc labelled with character  c from the trie node
     *         represented by index  s in double-array structure  d. Note
     *         that it assumes that no such arc exists before inserting.
     */
    int insertBranch(int s, int c)
    {

        int base = getBase(s);
        int next;
        if (base > 0) {
            next = base + c;
            /* if already there, do not actually insert */
            if (getCheck(next) == s)
                return next;

            /*
             * if (base + c) > TRIE_INDEX_MAX which means 'next' is overflow, or
             * cell [next] is not free, relocate to a free slot
             */
            if (base > TRIE_INDEX_MAX - c || !checkFreeCell(next)) {

                int new_base;
                /* relocate BASE[s] */
                vector<wchar_t> symbols;
                findAllChildren(s, symbols);
                symbols.push_back(c);
                new_base = findFreeBase(symbols);

                if (TRIE_INDEX_ERROR == new_base)
                    return TRIE_INDEX_ERROR;

                relocateBase(s, new_base);
                next = new_base + c;
            }
        } else {
            int new_base;
            vector<wchar_t>symbols;
            symbols.push_back(c);
            new_base = findFreeBase(symbols);

            if (TRIE_INDEX_ERROR == new_base)
                return TRIE_INDEX_ERROR;

            setBase(s, new_base);
            next = new_base + c;
        }
        allocCell(next);
        setCheck(next, s);

        return next;
    }

    /**
    * @brief Prune the single branch
     *
     * @param d
     *            : the double-array structure
     * @param s
     *            : the dangling state to prune off
     *
     *            Prune off a non-separate path up from the final state  s. If
     *            s still has some children states, it does nothing. Otherwise,
     *            it deletes the node and all its parents which become
     *            non-separate.
     */
    void prune(int s)
    {
        pruneUpto(getRoot(), s);
    }

    /**
     * @brief Prune the single branch up to given parent
     *
     * @param d
     *            : the double-array structure
     * @param p
     *            : the parent up to which to be pruned
     * @param s
     *            : the dangling state to prune off
     *            Prune off a non-separate path up from the final state  s to
     *            the given parent  p. The pruning stop when either the
     *            parent  p is met, or a first non-separate node is found.
     */
    void pruneUpto(int p, int s)
    {
        while (p != s && !hasChildren(s)) {
            int parent = getCheck(s);
            freeCell(s);
            s = parent;
        }
    }


    bool hasChildren(int s)
    {
        int base = getBase(s);
        if (TRIE_INDEX_ERROR == base || base < 0)
            return false;

        int max_c = min(TRIE_CHILD_MAX,TRIE_INDEX_MAX - base);
        for (int c = 0; c < max_c; c++) {
            if (getCheck(base + c) == s)
                return true;
        }

        return false;
    }

    bool writeToFile(FILE * file)
    {
        if (!file_write_int32 (file, DA_SIGNATURE) ||
                !file_write_int32 (file, num_cells)) {
            return false;
        }

        if(fwrite(_cell, sizeof(Cell), num_cells, file) != num_cells) {
            assert(false);
            return false;
        }

        return true;
    }

    bool readFromFile(FILE * file)
    {
        long pos = ftell(file);
        int sig;
        if(!file_read_int32(file, &sig)
                || sig != DA_SIGNATURE
                || !file_read_int32(file, &num_cells)) {
            cerr<<"error: read Double Array signature failed!!!"<<endl;
            cerr<<"DA_SIG="<<DA_SIGNATURE<<" read sig ="<<sig<<endl;
            goto exist_read;
        }

        free(_cell);
        _cell = (Cell *) malloc(num_cells * sizeof(Cell));
        if(!_cell) {
            cerr<<"error: malloc cells failed!!!"<<endl;
            goto exist_read;
        }

        if(fread(_cell, sizeof(Cell), num_cells, file) != num_cells) {
            cerr<<"error: read cell failed!!!"<<endl;
            goto exist_read;
        }

        return true;
exist_read:
        fseek(file, pos, SEEK_SET);
        assert(false);
        return false;
    }

private:
    void init()
    {
        _cell = (Cell *) malloc(DA_POOL_BEGIN * sizeof(Cell));
        num_cells = DA_POOL_BEGIN;
        _cell[0].base = DA_SIGNATURE; // mark of datrie
        _cell[0].check = DA_POOL_BEGIN; // number of cells
        _cell[1].base = -1; // address of last free cell
        _cell[1].check = -1; // address of first free cell
        _cell[2].base = DA_POOL_BEGIN; // set root's base at DA_POOL_BEGIN
        _cell[2].check = 0; //
    }

    void setCheck(int position, int value)
    {
        _cell[position].check = value;
    }

    bool checkFreeCell(int s)
    {
        return extendPool(s) && getCheck(s) < 0;
    }

    void allocCell(int index)
    {
        int prev = -getBase(index);
        int next = -getCheck(index);

        /* remove the cell from free list */
        setCheck(prev, -next);
        setBase(next, -prev);

    }

    void freeCell(int cell)
    {
        /* find insertion point */
        int i = -getCheck(FREE_LIST_HEAD);
        while (i != FREE_LIST_HEAD && i < cell)
            i = -getCheck(i);
        int prev = -getBase(i);

        /* insert cell before i */
        setCheck(cell, -i);
        setBase(cell, -prev);
        setCheck(prev, -cell);
        setBase(i, -cell);
    }

    bool extendPool(int to_index)
    {
        if (to_index <= 0 || TRIE_INDEX_MAX <= to_index)
            return false;

        if (to_index < num_cells)
            return true;

        int new_begin = num_cells;
        _cell = (Cell *)realloc(_cell, (to_index + 1)*sizeof(Cell));
        num_cells = to_index + 1;

        /* initialize new free list */
        for (int i = new_begin; i < to_index; i++) {
            setCheck(i, -(i + 1));
            setBase(i + 1, -i);
        }

        /* merge the new circular list to the old */
        int free_tail = -getBase(FREE_LIST_HEAD);
        setCheck(free_tail, -new_begin);
        setBase(new_begin, -free_tail);
        setCheck(to_index, -FREE_LIST_HEAD);
        setBase(FREE_LIST_HEAD, -to_index);

        /* update header cell */
        _cell[0].check = to_index + 1;

        return true;
    }


    void relocateBase(int s, int new_base)
    {
        int old_base = getBase(s);
        vector<wchar_t> symbols;
        findAllChildren(s, symbols);

        for (int i = 0; i < symbols.size(); i++) {
            int old_next = old_base + symbols[i];
            int new_next = new_base + symbols[i];
            int old_next_base = getBase(old_next);

            /* allocate new next node and copy BASE value */
            allocCell(new_next);
            setCheck(new_next, s); // set new state as children of s.
            setBase(new_next, old_next_base); // save old next state to new one.

            /*
             * old_next node is now moved to new_next so, all cells belonging to
             * old_next must be given to new_next
             */
            /* preventing the case of TAIL pointer */
            if (old_next_base > 0) {
                int c, max_c;

                max_c = min(TRIE_CHILD_MAX, TRIE_INDEX_MAX - old_next_base);
                for (c = 0; c < max_c; c++) {
                    if (getCheck(old_next_base + c) == old_next)
                        setCheck(old_next_base + c, new_next);
                }
            }

            /* free old_next node */
            freeCell(old_next);
        }
        /* finally, make BASE[s] point to new_base */
        setBase(s, new_base);
    }

    int findFreeBase(const vector<wchar_t> & children)
    {
        /* find first free cell that is beyond the first symbol */
        int first_child = children[0];
        int s = -getCheck(FREE_LIST_HEAD);
        while (s != FREE_LIST_HEAD && s < first_child + DA_POOL_BEGIN) {
            s = -getCheck(s);
        }
        if (s == FREE_LIST_HEAD) {
            for (s = first_child + DA_POOL_BEGIN;; ++s) {
                if (!extendPool(s))
                    return TRIE_INDEX_ERROR;
                if (getCheck(s) < 0)
                    break;
            }
        }

        /* search for next free cell that fits the symbols set */
        while (!fitAllChildren(s - first_child, children)) {
            /* extend pool before getting exhausted */
            if (-getCheck(s) == FREE_LIST_HEAD) {
                if (!extendPool(num_cells))
                    return TRIE_INDEX_ERROR;
            }

            s = -getCheck(s);
        }

        return s - first_child;

    }

    /**
     * Check if the base can save all children.
     *
     * @param base
     * @param children
     * @return
     */
    bool fitAllChildren(int base, const vector<wchar_t> & children)
    {
        for (int i = 0; i < children.size(); i++) {
            wchar_t sym = children[i];

            /*
             * if (base + sym) > TRIE_INDEX_MAX which means it's overflow, or
             * cell [base + sym] is not free, the symbol is not fit.
             */
            if (base > TRIE_INDEX_MAX - sym || !checkFreeCell(base + sym))
                return false;
        }
        return true;
    }

    /**
     * Find all children of s.<br>
     *
     * @param s
     * @return
     */
    void findAllChildren(int s, vector<wchar_t> & children)
    {
        int base = getBase(s);
        int max_c = min(TRIE_CHILD_MAX, TRIE_INDEX_MAX - base);
        for (int c = 0; c < max_c; c++) {
            if (getCheck(base + c) == s)
                children.push_back((wchar_t) c);
        }
    }

};
}

