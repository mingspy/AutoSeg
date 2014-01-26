/*
 * Matrix Library
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

namespace mingspy
{

/*
* Instance is a row of a matrix.
*/
class SparseInstance
{
private:
    int * m_Indices;
    double * m_AttValues;
    int m_NumAttributes; // all the attributes type of this instance have.
    int m_NumValues; // the actual attributes this instance holds.

public:
    SparseInstance()
    {
        m_NumValues = 0;
        m_NumAttributes = 0;
        m_Indices = NULL;
        m_AttValues = NULL;
    }

    ~SparseInstance()
    {
        delete [] m_Indices;
        delete [] m_AttValues;
    }

    /**
    * Returns the index of the attribute stored at the given position.
    * @param position the position
    * @return the index of the attribute stored at the given position
    */
    int index(int position)
    {
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
    * Returns the number of attributes.
    * @return the number of attributes as an integer
    */
    int numAttributes()
    {
        return m_NumAttributes;
    }

    /**
    * Returns the number of values in the sparse vector.
    * @return the number of values
    */
    int numValues()
    {
        return m_NumValues;
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
            if (value != 0)
            {
                double * tempValues = new double[m_NumValues + 1];
                int * tempIndices = new int[m_NumValues + 1];
                if(m_NumValues > 0)
                {
                    memcpy(tempIndices, m_Indices, index * sizeof(int));
                    memcpy(tempValues, m_AttValues, index * sizeof(double));
                }

                if(index  < m_NumValues)
                {
                    memcpy(tempIndices + index + 1, m_Indices + index,
                           (m_NumValues - index) * sizeof(int));
                    memcpy(tempValues + index + 1, m_AttValues + index,
                           (m_NumValues - index) * sizeof(double));
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
    double getValue(int attIndex)
    {
        int index = locateIndex(attIndex);
        if ((index >= 0) && (m_Indices[index] == attIndex))
        {
            return m_AttValues[index];
        }
        else
        {
            return 0.0;
        }
    }

};
}
