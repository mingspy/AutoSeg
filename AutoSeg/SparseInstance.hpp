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

class Matrix;
/*
* Instance is a row of a matrix.
*/
class SparseInstance
{
private:
    int * m_Indices;
    double * m_AttValues;
    int m_NumValues; // the actual attributes this instance holds.
    friend class Matrix;
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
        if(&refer != this)
        {
            if(m_NumValues > 0)
            {
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
    inline int index(int position) const
    {
        assert(position < m_NumValues);
        return m_Indices[position];
    }

    /**
    * Locates the greatest index that is not greater than the given index.
    *
    * @return the internal index of the attribute index. Returns -1 if no index
    *         with this property could be found
    */
    int locateIndex(int index)
    {
        int min = 0, max = m_NumValues - 1;
        if (max == -1)
        {
            return -1;
        }

        // Binary search
        while ((m_Indices[min] <= index) && (m_Indices[max] >= index))
        {
            int current = (max + min) / 2;
            if (m_Indices[current] > index)
            {
                max = current - 1;
            }
            else if (m_Indices[current] < index)
            {
                min = current + 1;
            }
            else
            {
                return current;
            }
        }

        if (m_Indices[max] < index)
        {
            return max;
        }
        else
        {
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
    void setValue(int attIndex, double value)
    {
        int index = locateIndex(attIndex);

        if ((index >= 0) && (m_Indices[index] == attIndex))
        {
            m_AttValues[index] = value;
        }
        else
        {
            // need insert a new value after index.
            index ++; // now insert at index.
            if (value != 0)
            {
                double * tempValues = new double[m_NumValues + 1];
                int * tempIndices = new int[m_NumValues + 1];
                if(m_NumValues > 0)
                {
                    // move old data before(include) index.
                    memcpy(tempIndices, m_Indices, index * sizeof(int));
                    memcpy(tempValues, m_AttValues, index * sizeof(double));
                    if(index  < m_NumValues )
                    {
                        memcpy(tempIndices + index + 1, m_Indices + index,
                               (m_NumValues - index) * sizeof(int));
                        memcpy(tempValues + index + 1, m_AttValues + index ,
                               (m_NumValues - index) * sizeof(double));
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
    inline double getValue(int attIndex)
    {
        double val = 0;
        int index = locateIndex(attIndex);
        if ((index >= 0) && (m_Indices[index] == attIndex))
        {
            val =  m_AttValues[index];
        }

        return val;
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
    double removeAttr(int attIndex)
    {
        double old_value = 0;
        int index = locateIndex(attIndex);
        if ((index >= 0) && (m_Indices[index] == attIndex))
        {
            old_value = m_AttValues[index];
            m_AttValues[index] = 0;;
        }
        return old_value;
    }

    friend ostream & operator<< (ostream & out, const SparseInstance & ins)
    {
        out<<"{size:"<<ins.m_NumValues<<", indices:[";
        if(ins.m_NumValues > 0)
        {
            for(int i = 0; i < ins.m_NumValues; i++)
            {
                if(i > 0)
                {
                    out<<",";
                }
                out<<ins.m_Indices[i];
            }
        }
        out<<"], attributes:[";
        if(ins.m_NumValues > 0)
        {
            for(int i = 0; i < ins.m_NumValues; i++)
            {
                if(i > 0)
                {
                    out<<",";
                }
                out<<ins.m_AttValues[i];
            }
        }
        out<<"]}";
        return out;
    }

private:
    void copyOf(const SparseInstance & refer)
    {
        int attribute_num = 0;
        for(int i = 0; i < refer.m_NumValues; i++)
        {
            if(refer.m_AttValues[i] != 0)
            {
                attribute_num ++;
            }
        }
        m_NumValues = attribute_num;
        if(attribute_num > 0)
        {
            m_Indices = new int[attribute_num];
            m_AttValues = new double[attribute_num];
            int k = 0;
            for(int i = 0; i < refer.m_NumValues; i++)
            {
                if(refer.m_AttValues[i] != 0)
                {
                    m_AttValues[k] = refer.m_AttValues[i];
                    m_Indices[k++] = refer.m_Indices[i];
                }
            }
        }
        else
        {
            m_AttValues = NULL;
            m_Indices = NULL;
        }
    }

    friend void WriteInstanceDataToFile(FILE * file, const void * data);
    friend  void * ReadInstanceDataFromFile(FILE * file,MemoryPool<> * pmem);
};

inline void InstanceFreer(void * ptr)
{
    SparseInstance * inst = static_cast<SparseInstance *>(ptr);
    delete inst;
}

/**
* Write data to a given file, which used for tail to serialize.
*/
inline void WriteInstanceDataToFile(FILE * file, const void * data)
{
    long old_pos = ftell(file);
    SparseInstance * inst = static_cast<SparseInstance *>((void *)data);
    if(inst != NULL)
    {
        if(!file_write_int32(file, inst->m_NumValues))
        {
            goto exit_write;
        }

        if(inst->m_NumValues > 0)
        {
            if(fwrite(inst->m_Indices, sizeof(int), inst->m_NumValues, file) != inst->m_NumValues)
            {
                goto exit_write;
            }

            if(fwrite(inst->m_AttValues, sizeof(double), inst->m_NumValues, file) != inst->m_NumValues)
            {
                goto exit_write;
            }
        }

        return;
    }

exit_write:
    fseek(file, old_pos, SEEK_SET);
    assert(false);
}

/*
* Reads data from given file, which used for tail unserialize.
*/
inline void * ReadInstanceDataFromFile(FILE * file, MemoryPool<> * pmem)
{
    long old_pos = ftell(file);
    int num = 0;
    SparseInstance * inst = NULL;

    if(!file_read_int32(file, &num))
    {
        goto exit_read;
    }
    
    if(pmem != NULL)
    {
        inst = (SparseInstance *)pmem->allocAligned(sizeof(SparseInstance));
    }
    else
    {
        inst = new SparseInstance();
    }
    inst->m_NumValues = num;
    if(num > 0)
    {
        if(pmem != NULL)
        {
            inst->m_Indices = (int *)pmem->allocAligned(num * sizeof(int));
        }
        else
        {
            inst->m_Indices = new int[num];
        }

        if(fread(inst->m_Indices, sizeof(int), inst->m_NumValues, file) != inst->m_NumValues)
        {
            goto exit_read_inst;
        }

        if(pmem != NULL)
        {
            inst->m_AttValues = (double *)pmem->allocAligned(num * sizeof(double));
        }
        else
        {
            inst->m_AttValues = new double[num];
        }

        if(fread(inst->m_AttValues, sizeof(double), inst->m_NumValues, file) != inst->m_NumValues)
        {
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
}

