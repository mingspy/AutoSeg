package com.mingspy.utils.dat.array;

import java.util.Arrays;

public class ValueList<V> {
	private final int INCREASE_RATIO_NUMERATOR;

	private final int INCREASE_RATIO_DENOMINATOR;

	private final int FIXED_INCREASE;

	private Object[] data;

	/**
	 * The size of this ArrayList.
	 */
	private int size;

	/**
	 * Constructs an empty list with an default capacity
	 */
	public ValueList() {
		this(16);
	}

	/**
	 * Constructs an empty list with the specified initial capacity.
	 * 
	 * @param initialCapacity
	 *            the initial capacity of the list
	 * @exception IllegalArgumentException
	 *                if the specified initial capacity is negative
	 */
	public ValueList(int initialCapacity) {
		this(initialCapacity, 1, 1, 1000);
	}

	/**
	 * Package protected constructor for use by factories. Besides the initial
	 * capacity, this constructor allows for specifying the growth
	 * characteristics. Whenever an increase of the storing array is needed, its
	 * new size is calculated as
	 * 
	 * <p>
	 * newCapacity = oldCapacity*(numerator/denominator) + fixed
	 * </p>
	 * 
	 * These values cannot be changed after construction.
	 * 
	 * @param initialCapacity
	 *            The initial capacity of the list
	 * @param incRatioNom
	 *            The numerator of the capacity increase fraction
	 * @param incRatioDenom
	 *            The denominator of the capacity increase fraction
	 * @param fixedInc
	 *            The fixed value added after the multiplication
	 */
	ValueList(int initialCapacity, int incRatioNom, int incRatioDenom, int fixedInc) {
		if (initialCapacity < 0)
			throw new IllegalArgumentException("Negative capacity specified " + initialCapacity);
		this.data = new Object[initialCapacity];
		this.INCREASE_RATIO_NUMERATOR = incRatioNom;
		this.INCREASE_RATIO_DENOMINATOR = incRatioDenom;
		this.FIXED_INCREASE = fixedInc;
	}

	/**
	 * Ensures that this instance has enough capacity to hold at least
	 * <tt>capacity</tt> values. Runs in O(n) worst case, O(1) amortized.
	 * 
	 * @param capacity
	 *            the desired minimum capacity
	 */
	private void ensureCapacity(int capacity) {
		int oldCapacity = data.length;
		if (capacity > oldCapacity) {
			int newCapacity = (oldCapacity * INCREASE_RATIO_NUMERATOR) / INCREASE_RATIO_DENOMINATOR + FIXED_INCREASE;
			if (newCapacity < capacity)
				newCapacity = capacity;
			data = Arrays.copyOf(data, newCapacity);
		}
	}

	public int size() {
		return size;
	}

	public boolean isEmpty() {
		return size == 0;
	}

	public V get(int index) {
		checkValidIndex(index);
		return (V) data[index];
	}

	public V set(int index, V value) {
		checkValidIndex(index);

		Object oldValue = data[index];
		data[index] = value;
		return (V) oldValue;
	}

	public boolean add(V value) {
		ensureCapacity(size + 1);
		data[size++] = value;
		return true;
	}

	public void add(int index, int value) {
		if (index > size || index < 0)
			throw new IndexOutOfBoundsException("Index: " + index + ", Size: " + size);

		ensureCapacity(size + 1);
		System.arraycopy(data, index, data, index + 1, size - index);
		data[index] = value;
		size++;
	}

	public V remove(int index) {
		checkValidIndex(index);
		V oldValue = (V) data[index];
		int numMoved = size - index - 1;
		if (numMoved > 0) {
			System.arraycopy(data, index + 1, data, index, numMoved);
		}
		size--;
		return oldValue;
	}

	/**
	 * Checks if the given index is less than the size of this ArrayList. The
	 * other half of the check (if it is &lt;0) is performed by the array
	 * implementation. This check is needed because the array can be larger that
	 * <tt>size</tt> so accesses beyond that will be allowed by the JVM.
	 */
	private void checkValidIndex(int index) {
		if (index >= size)
			throw new IndexOutOfBoundsException("Index: " + index + ", Size: " + size);
	}

	@Override
	public String toString() {
		if (size() == 0)
			return "[]";

		StringBuilder sb = new StringBuilder();
		sb.append('[');
		for (int i = 0; i < size(); i++) {
			V e = get(i);
			sb.append(e);
			if (i == size() - 1)
				sb.append(']').toString();
			sb.append(", ");
		}
		return sb.toString();
	}
}
