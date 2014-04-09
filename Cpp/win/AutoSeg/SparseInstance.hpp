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
#include <ostream>
#include <cassert>
#include "TrieDef.hpp"
#include "MemoryPool.hpp"
#include "MemLeaksCheck.h"


using namespace std;


namespace mingspy
{

template<typename T>
class Matrix;

/*
* SparseInstance is a vector used to save sparse data.
* For example, a vector has thousands of features,
* however, for a instance only a few features has
* values. Assume vector space {x1,x2,x3....x1000},here
* an instance is only have (x1 = 10, x10 = 11, x20 = 99),
* to present this instance, we can use a sparseInstance:
*     indices:  1, 10, 20
*     values : 10, 11, 99
*/
template<typename T>
class SparseInstance
{
public:
    static T ZERO;
private:
    int * m_Indices;
    T * m_AttValues;
    int m_NumValues; // the actual attributes this instance holds.
    friend class Matrix<T>;

public:
    SparseInstance()
    {
        m_NumValues = 0;
        m_Indices = NULL;
        m_AttValues = NULL;
    }

    ~SparseInstance()
    {
        delete [] m_Indices;
        delete [] m_AttValues;
    }

    SparseInstance(const SparseInstance & refer)
    {
        copyOf(refer);
    }

    SparseInstance & operator=(const SparseInstance & refer)
    {
        if(&refer != this) {
            if(m_NumValues > 0) {
                delete [] m_Indices;
                delete [] m_AttValues;
                copyOf(refer);
            }
        }
        return *this;
    }
    /**
    * Returns the index of the attribute stored at the given position.
    * @param position the position
    * @return the index of the attribute stored at the given position
    */
    inline int indexAt(int position) const
    {
        assert(position < m_NumValues);
        return m_Indices[position];
    }

    /*
    * position is the direct index of m_AttValues.
    */
    inline T & valueAt(int position) const
    {
        assert(position < m_NumValues);
        return m_AttValues[position];
    }

    inline void setValue(int position, const T & val) {
        assert(position < m_NumValues);
        m_AttValues[position] = val;
    }
    /**
    * Locates the greatest index that is not greater than the given index.
    *
    * @return the internal index of the attribute index. Returns -1 if no index
    *         with this property could be found
    */
    int locateAttrIndex(int attrindex) const
    {
        int min = 0, max = m_NumValues - 1;
        if (max == -1) {
            return -1;
        }

        // Binary search
        while ((m_Indices[min] <= attrindex) && (m_Indices[max] >= attrindex)) {
            int current = (max + min) / 2;
            if (m_Indices[current] > attrindex) {
                max = current - 1;
            } else if (m_Indices[current] < attrindex) {
                min = current + 1;
            } else {
                return current;
            }
        }

        if (m_Indices[max] < attrindex) {
            return max;
        } else {
            return min - 1;
        }
    }

    /**
    * Returns the number of values in the sparse vector.
    * @return the number of values
    */
    inline int numValues() const
    {
        return m_NumValues;
    }

    // get the attribute indexes.
    const int * getAttrIndices() const
    {
        return m_Indices;
    }

    T * getAttrValues()
    {
        return m_AttValues;
    }

    T  sumOfValues() const
    {
        T sum = 0;
        for(int i = 0; i < m_NumValues; i++) {
            sum += m_AttValues[i];
        }
        return sum;
    }
    /**
    * Sets a specific value in the instance to the given value (internal
    * floating-point format). Performs a deep copy of the vector of attribute
    * values before the value is set.
    *
    * @param attIndex the attribute's index
    * @param value the new attribute value (If the corresponding attribute is
    *          nominal (or a string) then this is the new value's index as a
    *          double).
    */
    void setAttrValue(int attIndex, const T & value)
    {
        int index = locateAttrIndex(attIndex);

        if ((index >= 0) && (m_Indices[index] == attIndex)) {
            m_AttValues[index] = value;
        } else {
            // need insert a new value after index.
            index ++; // now insert at index.
            if(value != 0) {
                T * tempValues = new T[m_NumValues + 1];
                int * tempIndices = new int[m_NumValues + 1];
                if(m_NumValues > 0) {
                    // move old data before(include) index.
                    memcpy(tempIndices, m_Indices, index * sizeof(int));
                    memcpy(tempValues, m_AttValues, index * sizeof(T));
                    if(index  < m_NumValues ) {
                        memcpy(tempIndices + index + 1, m_Indices + index,
                               (m_NumValues - index) * sizeof(int));
                        memcpy(tempValues + index + 1, m_AttValues + index ,
                               (m_NumValues - index) * sizeof(T));
                    }
                }

                tempIndices[index] = attIndex;
                tempValues[index] = value;
                delete [] m_AttValues;
                delete [] m_Indices;

                m_AttValues = tempValues;
                m_Indices = tempIndices;
                m_NumValues ++;
            }
        }
    }

    /**
     * Returns an instance's attribute value in internal format.
     *
     * @param attIndex the attribute's index
     * @return the specified value as a double (If the corresponding attribute is
     *         nominal (or a string) then it returns the value's index as a
     *         double).
     */
    inline T & getAttrValue(int attIndex) const
    {

        int index = locateAttrIndex(attIndex);
        if ((index >= 0) && (m_Indices[index] == attIndex)) {
            return m_AttValues[index];
        }

        return ZERO;
    }

    /**
     * Remove the attribute at given attribute index. Here only mark the attribute
     * as the default value (0).
     *
     * @param attIndex the attribute' index
     * @return the specified value as a double (If the corresponding attribute is
     *         nominal (or a string) then it returns the value's index as a
     *         double).
     */
    T removeAttr(int attIndex)
    {
        T old_value = 0;
        int index = locateAttrIndex(attIndex);
        if ((index >= 0) && (m_Indices[index] == attIndex)) {
            old_value = m_AttValues[index];
            m_AttValues[index] = 0;;
        }
        return old_value;
    }

    friend ostream & operator<< (ostream & out, const SparseInstance & ins)
    {
        out<<"{size:"<<ins.m_NumValues<<", indices:[";
        if(ins.m_NumValues > 0) {
            for(int i = 0; i < ins.m_NumValues; i++) {
                if(i > 0) {
                    out<<",";
                }
                out<<ins.m_Indices[i];
            }
        }
        out<<"], values:[";
        if(ins.m_NumValues > 0) {
            for(int i = 0; i < ins.m_NumValues; i++) {
                if(i > 0) {
                    out<<",";
                }
                out<<ins.m_AttValues[i];
            }
        }
        out<<"]}";
        return out;
    }

    static void DoWriteSIToFile(FILE * file, const SparseInstance<T> * data)
    {
        long old_pos = ftell(file);
        if(data != NULL) {
            if(!file_write_int32(file, data->m_NumValues)) {
                goto exit_write;
            }

            if(data->m_NumValues > 0) {
                if(fwrite(data->m_Indices, sizeof(int), data->m_NumValues, file) != data->m_NumValues) {
                    goto exit_write;
                }

                if(fwrite(data->m_AttValues, sizeof(T), data->m_NumValues, file) != data->m_NumValues) {
                    goto exit_write;
                }
            }

            return;
        }

exit_write:
        fseek(file, old_pos, SEEK_SET);
        assert(false);
    }

    static SparseInstance<T> * DoReadSIFromFile(FILE * file, MemoryPool<> * pmem)
    {
        long old_pos = ftell(file);
        int num = 0;
        SparseInstance<T> * inst = NULL;

        if(!file_read_int32(file, &num)) {
            goto exit_read;
        }

        if(pmem != NULL) {
            inst = (SparseInstance<T> *)pmem->allocAligned(sizeof(SparseInstance<T>));
        } else {
            inst = new SparseInstance<T>();
        }
        inst->m_NumValues = num;
        if(num > 0) {
            if(pmem != NULL) {
                inst->m_Indices = (int *)pmem->allocAligned(num * sizeof(int));
            } else {
                inst->m_Indices = new int[num];
            }

            if(fread(inst->m_Indices, sizeof(int), inst->m_NumValues, file) != inst->m_NumValues) {
                goto exit_read_inst;
            }

            if(pmem != NULL) {
                inst->m_AttValues = (T *)pmem->allocAligned(num * sizeof(T));
            } else {
                inst->m_AttValues = new T[num];
            }

            if(fread(inst->m_AttValues, sizeof(T), inst->m_NumValues, file) != inst->m_NumValues) {
                goto exit_read_inst;
            }
        }

        return inst;
exit_read_inst:
        if(!pmem)
            delete inst;
exit_read:
        fseek(file, old_pos, SEEK_SET);
        assert(false);
        return NULL;
    }

private:
    void copyOf(const SparseInstance & refer)
    {
        int attribute_num = 0;
        for(int i = 0; i < refer.m_NumValues; i++) {
            if(refer.m_AttValues[i] != 0) {
                attribute_num ++;
            }
        }
        m_NumValues = attribute_num;
        if(attribute_num > 0) {
            m_Indices = new int[attribute_num];
            m_AttValues = new T[attribute_num];
            int k = 0;
            for(int i = 0; i < refer.m_NumValues; i++) {
                if(refer.m_AttValues[i] != 0) {
                    m_AttValues[k] = refer.m_AttValues[i];
                    m_Indices[k++] = refer.m_Indices[i];
                }
            }
        } else {
            m_AttValues = NULL;
            m_Indices = NULL;
        }
    }
    //friend  void InstanceFreer(void * ptr);
    //friend  void WriteInstanceDataToFile(FILE * file, const void * data);
    //friend  void * ReadInstanceDataFromFile(FILE * file,MemoryPool<> * pmem);

};
template<typename T>
T SparseInstance<T>::ZERO = 0;

typedef SparseInstance<int> WordNature;

void WordNatureFreer(void * ptr)
{
    WordNature * inst = static_cast<WordNature *>(ptr);
    delete inst;
}
/*
* Reads data from given file, which used for tail unserialize.
*/
void * ReadWordNatureFromFile(FILE * file, MemoryPool<> * pmem)
{
    return WordNature::DoReadSIFromFile(file, pmem);
}


void WriteWordNatureToFile(FILE * file, const void * data)
{
    WordNature::DoWriteSIToFile(file, static_cast<WordNature *>(const_cast<void *>(data)));
}



}

