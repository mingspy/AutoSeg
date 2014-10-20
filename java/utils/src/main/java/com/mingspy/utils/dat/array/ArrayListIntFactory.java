/*
 * Copyright 2010 Christos Gioran
 *
 * This file is part of DoubleArrayTrie.
 *
 * DoubleArrayTrie is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DoubleArrayTrie is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with DoubleArrayTrie.  If not, see <http://www.gnu.org/licenses/>.
 */
package com.mingspy.utils.dat.array;

/**
 * Implementation of a factory class for IntegerArrayLists. Holds the
 * configuration for creating ArrayLists for <tt>int</tt>s with specified
 * initial size and growth characteristics.
 *
 *
 */
public class ArrayListIntFactory
{

    private static final int initialCapacity = 0;
    private static final int numerator = 5;
    private static final int denominator = 4;
    private static final int fixedInc = 1;

    /**
     * @see org.digitalstain.datrie.store.IntegerListFactory#newListInt()
     */
    public static ListInt newListInt()
    {
        return new ArrayListInt(initialCapacity, numerator, denominator, fixedInc);
    }

    public static ListInt newListInt(int[] array, int from)
    {
        ListInt list = new ArrayListInt(array.length - from, numerator, denominator, fixedInc);
        for (int i = from; i < array.length; i++) {
            list.add(array[i]);
        }

        return list;
    }

}
